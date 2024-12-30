/*
 * Change Logs:
 * Date           Author       Notes
 * 2023-11-23     WangShun     the first version
 * 2024-09-19     WangShun     support rv32
 * 2024-11-13     HPMicro      support rvbacktrace in trap
 */

#include "../include/rvbacktrace.h"
#include "rvcoredump.h"
#include <string.h>

/* Please check that the following symbols are defined in the linked scripts ！*/ 
/* If not, define the following symbols at the beginning and end of the text segment */
extern char *__etext;

extern unsigned int rvstack_frame[STACK_FRAME_LEN]; // stack frame
extern unsigned int rvstack_frame_len; // stack frame len
extern struct rv_coredump_regs core_regs;

static int lvl;

/* 
    Some instructions are placed in the RAM area for acceleration
    so PC checking are not limited to text segments 
*/

#define BT_CHK_SP_AVAIL(sp)   ((uintptr_t)(sp) < (uintptr_t)(&_stack))

#define BT_FUNC_LIMIT   0x2000
#define BT_LVL_LIMIT    64

#define BT_PC2ADDR(pc)        ((char *)(((uintptr_t)(pc))))

/* get framesize from c ins32 */
static int riscv_backtrace_framesize_get1(unsigned int inst)
{
    unsigned int imm = 0;
    /* addi sp, sp, -im */
    if ((inst & 0x800FFFFF) == 0x80010113) {
        imm = (inst >> 20) & 0x7FF;
        imm = (~imm & 0x7FF) + 1;
        return imm >> 3;
    }

    return -1;
}

/* get framesize from c ins */
static int riscv_backtrace_framesize_get(unsigned short inst)
{
    unsigned int imm = 0;
    /* addi sp, sp, -im */
    if ((inst & 0xFF83) == 0x1101) {
        imm = (inst >> 2) & 0x1F;
        imm = (~imm & 0x1F) + 1;
        return imm;
    }

    /* c.addi16sp sp, nzuimm6<<4 */
    if ((inst & 0xFF83) == 0x7101) {
        imm = (inst >> 3) & 0x3;
        imm = (imm << 1) | ((inst >> 5) & 0x1);
        imm = (imm << 1) | ((inst >> 2) & 0x1);
        imm = (imm << 1) | ((inst >> 6) & 0x1);
        imm = ((~imm & 0x1f) + 1) << 4;
        return imm;
    }

    return -1;
}

static int riscv_backtrace_ra_offset_get1(unsigned int inst)
{
    unsigned int imm = 0;
    /* sw ra,552(sp) */
    if ((inst & 0x81FFF07F) == 0x112023) {
        imm = (inst >> 7) & 0x1F;
        imm |= ((inst >> 25) & 0x7F) << 5;
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm >> 3;
    }

    return -1;
}

/* get ra position in the stack */
static int riscv_backtrace_ra_offset_get(unsigned short inst)
{
    unsigned int imm = 0;
    /* c.fsdsp rs2, uimm6<<3(sp) */
    if ((inst & 0xE07F) == 0xE006) {
        imm = (inst >> 7) & 0x7;
        imm = (imm << 3) | ((inst >> 10) & 0x7);
        /* The unit is size_t, So we don't have to move 3 bits to the left */
        return imm;
    }
    /* 
        c.swsp rs2, uimm(x2)
        110 imm<<2 rs2 10
         3    6     5   2
    */
    if ((inst & 0xE07F) == 0xC006) {
        imm = (inst >> 7) & 0x3;
        imm = (imm << 4) | ((inst >> 9) & 0xF);

        return imm << 2;
    }

    return -1;
}

/* get the offset between the jump instruction and the return address */
static int backtraceFindLROffset(char *LR)
{
    int offset = 0;
    char *LR_indeed;
    unsigned int ins32;

    LR_indeed = BT_PC2ADDR(LR);

    /* Usually jump using the JAL instruction */
    ins32 = *(unsigned int *)(LR_indeed - 4);
    if ((ins32 & 0x3) == 0x3) {
        offset = 4;
    } else {
        offset = 2;
    }

    return offset;
}

static int riscv_backtraceFromStack(long **pSP, char **pPC,
                                    int (*print_func)(const char *fmt, ...))
{
    char *CodeAddr = NULL;
    long  *SP      = *pSP;
    char *PC       = *pPC;
    char *LR;
    int   i;
    int   framesize;
    int   offset = 0;
    unsigned int ins32;
    unsigned short ins16;

    /* 1. scan code, find lr pushed */
    for (i = 0; i < BT_FUNC_LIMIT;) {
        /* FIXME: not accurate from bottom to up. how to judge 2 or 4byte inst */
        //CodeAddr = (char *)(((long)PC & (~0x3)) - i);
        CodeAddr = (char *)(PC - i);
        ins32 = *(unsigned int *)(CodeAddr);
        if ((ins32 & 0x3) == 0x3) {
            ins16 = *(unsigned short *)(CodeAddr - 2);
            if ((ins16 & 0x3) != 0x3) {
                i += 4;
                framesize = riscv_backtrace_framesize_get1(ins32);
                if (framesize >= 0) {
                    CodeAddr += 4;
                    break;
                }
                continue;
            }
        }
        i += 2;
        ins16 = (ins32 >> 16) & 0xffff;
        framesize = riscv_backtrace_framesize_get(ins16);
        if (framesize >= 0) {
            CodeAddr += 2;
            break;
        }
    }

    if (i == BT_FUNC_LIMIT) {
        /* error branch */
        if (print_func != NULL) {
            print_func("Backtrace fail!\r\n");
        }
        return -1;
    }

    /* 2. scan code, find ins: sd ra,24(sp) or sd ra,552(sp) */
    for (i = 0; CodeAddr + i < PC;) {
        ins32 = *(unsigned int *)(CodeAddr + i);
        if ((ins32 & 0x3) == 0x3) {
            i += 4;
            offset = riscv_backtrace_ra_offset_get1(ins32);
            if (offset >= 0) {
                break;
            }
        } else {
            i += 2;
            ins16 = ins32 & 0xffff;
            offset = riscv_backtrace_ra_offset_get(ins16);
            if (offset >= 0) {
                break;
            }
        }
    }

    framesize = framesize * sizeof(char) / sizeof(long);
    offset = offset * sizeof(char) / sizeof(long);
    /* 3. output */
    LR     = (char *) * (SP + offset);
    *pSP   = SP + framesize;
    offset = backtraceFindLROffset(LR);

    rvstack_frame[lvl] = (unsigned int)(LR);
    print_func("[%d]Stack interval :[0x%lx - 0x%lx]  ra 0x%lx\n", lvl, SP, SP + framesize, LR);
    *pPC   = LR - offset;

    return offset == 0 ? 1 : 0;
}

static void backtrace_trap_stackframe(long **pSP, struct rv_coredump_regs *cd)
{
    struct rv_coredump_regs *tmp;
    uint32_t size = sizeof(struct rv_coredump_regs) / sizeof(unsigned long);
    
    tmp = (struct rv_coredump_regs *)(*pSP - size);
    if (lvl == BACKTRACE_TRAP_DEPTH) {
        memcpy(cd, tmp, sizeof(struct rv_coredump_regs));
    }
}

static int backtraceFromStack(long **pSP, char **pPC,
                              int (*print_func)(const char *fmt, ...))
{
    if (BT_CHK_SP_AVAIL(*pSP) == 0) {
        return -1;
    }

    return riscv_backtraceFromStack(pSP, pPC, print_func);
}

/* get the return address of the current function */
__attribute__((always_inline)) static inline void *backtrace_get_sp(void)
{
    void *sp;
    __asm__ volatile("mv %0, sp\n" : "=r"(sp));
    return sp;
}

/* get the return address of the current function */
__attribute__((always_inline)) static inline void *backtrace_get_pc(void)
{
    void *pc;
    __asm__ volatile("auipc %0, 0\n" : "=r"(pc));
    return pc;
}

/* printf call stack
   return levels of call stack */
int rvbacktrace_fomit(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    long  *SP;
    int   ret;

    if (print_func == NULL) {
        print_func = printf;
    }

    SP = backtrace_get_sp();
    PC = backtrace_get_pc();

    print_func("\r\n---- RV_Backtrace Call Frame Start: ----\r\n");
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = backtraceFromStack(&SP, &PC, print_func);
        if (ret != 0) {
            rvstack_frame_len = lvl;
            break;
        }
    }
    rvbacktrace_addr2line((uint32_t *)&rvstack_frame[0], print_func);
    print_func("---- RV_Backtrace Call Frame End:----\r\n");
    print_func("\r\n");
    return lvl;
}

int rvbacktrace_fomit_trap(int (*print_func)(const char *fmt, ...))
{
    char *PC;
    long  *SP;
    int   ret;

    if (print_func == NULL) {
        print_func = printf;
    }

    SP = backtrace_get_sp();
    PC = backtrace_get_pc();

    print_func("\r\n---- RV_Backtrace In Trap Start: ----\r\n");
    for (lvl = 0; lvl < BT_LVL_LIMIT; lvl++) {
        ret = riscv_backtraceFromStack(&SP, &PC, print_func);
        backtrace_trap_stackframe(&SP, &core_regs);
        if (ret == 0) {
            rvstack_frame_len = lvl + 1;
        }

        if (BT_CHK_SP_AVAIL(SP) == 0)
            break;
    }
    rvbacktrace_addr2line((uint32_t *)&rvstack_frame[0], print_func);
    print_func("---- RV_Backtrace In Trap End:----\r\n");
    print_func("\r\n");
    return lvl;
}