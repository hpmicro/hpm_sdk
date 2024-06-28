/*
 * Copyright (c) 2022, Egahp
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "chry_readline.h"

#if defined(__CC_ARM) || defined(__CLANG_ARM) || defined(__GNUC__) || defined(__ADSPBLACKFIN__)
#ifndef __unused
#define __unused __attribute__((unused))
#endif
#else
#ifndef __unused
#define __unused
#endif
#endif

#define chry_readline_waitkey(__rl, __c)            \
    do {                                            \
        while (0 == (__rl)->sget((__rl), (__c), 1)) \
            ;                                       \
    } while (0)

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK

#define chry_readline_getkey(__rl, __c)                        \
    do {                                                       \
        (__rl)->noblock = 0 == (__rl)->sget((__rl), (__c), 1); \
    } while (0)

#else

#define chry_readline_getkey chry_readline_waitkey

#endif

#if defined(CONFIG_READLINE_DEBUG) && CONFIG_READLINE_DEBUG

#define CHRY_READLINE_PARAM_CHECK(__expr, __ret) \
    do {                                         \
        if (!(__expr)) {                         \
            return __ret;                        \
        }                                        \
    } while (0)

#define chry_readline_put(__rl, __pbuf, __size, __ret)          \
    do {                                                        \
        uint16_t _size_ = (__size);                             \
        if (_size_ != (__rl)->sput((__rl), (__pbuf), _size_)) { \
            return __ret;                                       \
        }                                                       \
    } while (0)

#else

#define CHRY_READLINE_PARAM_CHECK(__expr, __ret) ((void)0)

#define chry_readline_put(__rl, __pbuf, __size, __ret) \
    do {                                               \
        (__rl)->sput((__rl), (__pbuf), (__size));      \
    } while (0)

#endif

#define chry_readline_seqgen_ris chry_readline_seqgen_reset
#define chry_readline_seqgen_cuu chry_readline_seqgen_cursor_up
#define chry_readline_seqgen_cud chry_readline_seqgen_cursor_down
#define chry_readline_seqgen_cuf chry_readline_seqgen_cursor_forward
#define chry_readline_seqgen_cub chry_readline_seqgen_cursor_backward
#define chry_readline_seqgen_cnl chry_readline_seqgen_cursor_nextline
#define chry_readline_seqgen_cpl chry_readline_seqgen_cursor_prevline
#define chry_readline_seqgen_cha chry_readline_seqgen_cursor_absolute
#define chry_readline_seqgen_cup chry_readline_seqgen_cursor_position
#define chry_readline_seqgen_ed  chry_readline_seqgen_erase_display
#define chry_readline_seqgen_el  chry_readline_seqgen_erase_line
#define chry_readline_seqgen_il  chry_readline_seqgen_insert_line
#define chry_readline_seqgen_dl  chry_readline_seqgen_delete_line
#define chry_readline_seqgen_dch chry_readline_seqgen_delete_char
#define chry_readline_seqgen_ech chry_readline_seqgen_erase_char
#define chry_readline_seqgen_su  chry_readline_seqgen_scroll_up
#define chry_readline_seqgen_sd  chry_readline_seqgen_scroll_down
#define chry_readline_seqgen_scp chry_readline_seqgen_save_cursor_position
#define chry_readline_seqgen_rcp chry_readline_seqgen_restore_cursor_position
#define chry_readline_seqgen_nsb chry_readline_seqgen_normal_screen
#define chry_readline_seqgen_asb chry_readline_seqgen_alternate_screen
#define chry_readline_seqgen_rss chry_readline_seqgen_report_screen_size

static const uint8_t vtmap[25] = {
    CHRY_READLINE_EXEC_NUL,  /*!< 0           */
    CHRY_READLINE_EXEC_MVHM, /*!< 1  home     */
    CHRY_READLINE_EXEC_NUL,  /*!< 2  insert   */
    CHRY_READLINE_EXEC_DEL,  /*!< 3  delete   */
    CHRY_READLINE_EXEC_MVED, /*!< 4  end      */
    CHRY_READLINE_EXEC_NUL,  /*!< 5  page up  */
    CHRY_READLINE_EXEC_NUL,  /*!< 6  page dn  */
    CHRY_READLINE_EXEC_MVHM, /*!< 7  home     */
    CHRY_READLINE_EXEC_MVED, /*!< 8  end      */
    CHRY_READLINE_EXEC_NUL,  /*!< 9           */
    CHRY_READLINE_EXEC_NUL,  /*!< 10          */
    CHRY_READLINE_EXEC_F1,   /*!< 11 F1       */
    CHRY_READLINE_EXEC_F2,   /*!< 12 F2       */
    CHRY_READLINE_EXEC_F3,   /*!< 13 F3       */
    CHRY_READLINE_EXEC_F4,   /*!< 14 F4       */
    CHRY_READLINE_EXEC_F5,   /*!< 15 F5       */
    CHRY_READLINE_EXEC_NUL,  /*!< 16          */
    CHRY_READLINE_EXEC_F6,   /*!< 17 F6       */
    CHRY_READLINE_EXEC_F7,   /*!< 18 F7       */
    CHRY_READLINE_EXEC_F8,   /*!< 19 F8       */
    CHRY_READLINE_EXEC_F9,   /*!< 20 F9       */
    CHRY_READLINE_EXEC_F10,  /*!< 21 F10      */
    CHRY_READLINE_EXEC_NUL,  /*!< 22          */
    CHRY_READLINE_EXEC_F11,  /*!< 23 F11      */
    CHRY_READLINE_EXEC_F12,  /*!< 24 F12      */
};

static const uint8_t xmap[20] = {
    CHRY_READLINE_EXEC_NUL,  /*!<             */
    CHRY_READLINE_EXEC_PRVH, /*!< A up        */
    CHRY_READLINE_EXEC_NXTH, /*!< B down      */
    CHRY_READLINE_EXEC_MVRT, /*!< C right     */
    CHRY_READLINE_EXEC_MVLT, /*!< D left      */
    CHRY_READLINE_EXEC_NUL,  /*!< E           */
    CHRY_READLINE_EXEC_MVED, /*!< F end       */
    CHRY_READLINE_EXEC_NUL,  /*!< G           */
    CHRY_READLINE_EXEC_MVHM, /*!< H home      */
    CHRY_READLINE_EXEC_NUL,  /*!< I           */
    CHRY_READLINE_EXEC_NUL,  /*!< J           */
    CHRY_READLINE_EXEC_NUL,  /*!< K           */
    CHRY_READLINE_EXEC_NUL,  /*!< L           */
    CHRY_READLINE_EXEC_NUL,  /*!< M           */
    CHRY_READLINE_EXEC_NUL,  /*!< N           */
    CHRY_READLINE_EXEC_NUL,  /*!< O           */
    CHRY_READLINE_EXEC_F1,   /*!< P F1        */
    CHRY_READLINE_EXEC_F2,   /*!< Q F2        */
    CHRY_READLINE_EXEC_F3,   /*!< R F3        */
    CHRY_READLINE_EXEC_F4    /*!< S F4        */
};

#if defined(CONFIG_READLINE_CTRLMAP) && CONFIG_READLINE_CTRLMAP
static const uint8_t ctrlmap[32] = {
#else
static uint8_t ctrlmap[32] = {
#endif
    CHRY_READLINE_EXEC_NUL,     /*!< @ NUL */
    CHRY_READLINE_EXEC_MVHM,    /*!< A SOH */
    CHRY_READLINE_EXEC_MVLT,    /*!< B STX */
    CHRY_READLINE_EXEC_SIGINT_, /*!< C ETX */
    CHRY_READLINE_EXEC_EOF_,    /*!< D EOT */
    CHRY_READLINE_EXEC_MVED,    /*!< E ENQ */
    CHRY_READLINE_EXEC_MVRT,    /*!< F ACK */
    CHRY_READLINE_EXEC_NUL,     /*!< G BEL */
    CHRY_READLINE_EXEC_BS,      /*!< H BS  */
    CHRY_READLINE_EXEC_CPLT,    /*!< I HT  */
    CHRY_READLINE_EXEC_NLN,     /*!< J LF  */
    CHRY_READLINE_EXEC_DELN,    /*!< K VT  */
    CHRY_READLINE_EXEC_CLR,     /*!< L FF  */
    CHRY_READLINE_EXEC_NLN,     /*!< M CR  */
    CHRY_READLINE_EXEC_NXTH,    /*!< N SO  */
    CHRY_READLINE_EXEC_NLN,     /*!< O SI  */
    CHRY_READLINE_EXEC_PRVH,    /*!< P DLE */
    CHRY_READLINE_EXEC_SIGCONT, /*!< Q DC1 */
    CHRY_READLINE_EXEC_NUL,     /*!< R DC2 */
    CHRY_READLINE_EXEC_SIGSTOP, /*!< S DC3 */
    CHRY_READLINE_EXEC_NUL,     /*!< T DC4 */
    CHRY_READLINE_EXEC_DHLN,    /*!< U NAK */
    CHRY_READLINE_EXEC_NUL,     /*!< V SYN */
    CHRY_READLINE_EXEC_DLWD,    /*!< W ETB */
    CHRY_READLINE_EXEC_NUL,     /*!< X CAN */
    CHRY_READLINE_EXEC_NUL,     /*!< Y EM  */
    CHRY_READLINE_EXEC_SIGTSTP, /*!< Z SUB */
    CHRY_READLINE_EXEC_NUL,     /*!< [ ESC */
    CHRY_READLINE_EXEC_SIGQUIT, /*!< \ FS  */
    CHRY_READLINE_EXEC_NUL,     /*!< ] GS  */
    CHRY_READLINE_EXEC_SWNM,    /*!< ^ RS  */
    CHRY_READLINE_EXEC_HELP,    /*!< - US  */
};

#if defined(CONFIG_READLINE_ALTMAP) && CONFIG_READLINE_ALTMAP
static const uint8_t altmap[26] = {
#else
static uint8_t altmap[26] = {
#endif
    CHRY_READLINE_EXEC_NUL, /*!< A */
    CHRY_READLINE_EXEC_NUL, /*!< B */
    CHRY_READLINE_EXEC_NUL, /*!< C */
    CHRY_READLINE_EXEC_NUL, /*!< D */
    CHRY_READLINE_EXEC_NUL, /*!< E */
    CHRY_READLINE_EXEC_NUL, /*!< F */
    CHRY_READLINE_EXEC_NUL, /*!< G */
    CHRY_READLINE_EXEC_NUL, /*!< H */
    CHRY_READLINE_EXEC_NUL, /*!< I */
    CHRY_READLINE_EXEC_NUL, /*!< J */
    CHRY_READLINE_EXEC_NUL, /*!< K */
    CHRY_READLINE_EXEC_NUL, /*!< L */
    CHRY_READLINE_EXEC_NUL, /*!< M */
    CHRY_READLINE_EXEC_NUL, /*!< N */
    CHRY_READLINE_EXEC_NUL, /*!< O */
    CHRY_READLINE_EXEC_NUL, /*!< P */
    CHRY_READLINE_EXEC_NUL, /*!< Q */
    CHRY_READLINE_EXEC_NUL, /*!< R */
    CHRY_READLINE_EXEC_NUL, /*!< S */
    CHRY_READLINE_EXEC_NUL, /*!< T */
    CHRY_READLINE_EXEC_NUL, /*!< U */
    CHRY_READLINE_EXEC_NUL, /*!< V */
    CHRY_READLINE_EXEC_NUL, /*!< W */
    CHRY_READLINE_EXEC_NUL, /*!< X */
    CHRY_READLINE_EXEC_NUL, /*!< Y */
    CHRY_READLINE_EXEC_NUL, /*!< Z */
};

/*****************************************************************************
* @brief        print decimal integer (0 - 65535)
* 
* @param[in]    buf         print buf
* @param[in]    idx         print idx
* @param[in]    value       16bit unsigned integer
* 
*****************************************************************************/
static void chry_readline_print_integer(uint8_t *buf, size_t *idx, uint16_t value)
{
    uint8_t array[5] = {0};
    uint8_t *intbuf = array;
    uint8_t *outbuf = buf + *idx;
    size_t count = 0;

    do {
        const uint8_t digit = (uint8_t)(value % 10);
        *intbuf++ = (uint8_t)('0' + digit);
        value /= 10;
        count++;
    } while (value);

    do {
        *outbuf++ = *--intbuf;
        (*idx)++;
    } while (--count);
}

/*****************************************************************************
* @brief        parse decimal integer (0 - 65535)
* 
* @param[in]    buf         parse buf
* @param[out]   value       16bit unsigned integer pointer
* 
*****************************************************************************/
static void chry_readline_parse_integer(uint8_t *buf, uint16_t *value)
{
    *value = 0;

    while (('0' <= *buf) && (*buf <= '9')) {
        *value *= 10;
        *value += *buf - '0';
        buf++;
    }
}

/*****************************************************************************
* @brief        Reset to Initial State
*****************************************************************************/
__unused static void chry_readline_seqgen_reset(uint8_t *buf, size_t *idx)
{
    buf[*idx + 0] = '\033';
    buf[*idx + 1] = 'c';
    *idx += 2;
}

/*****************************************************************************
* @brief        output csi prefix
*****************************************************************************/
__unused static void chry_readline_seqgen_csi(uint8_t *buf, size_t *idx)
{
    buf[*idx + 0] = '\033';
    buf[*idx + 1] = '[';
    *idx += 2;
}

/*****************************************************************************
* @brief        output csi 0 pn
*****************************************************************************/
__unused static void chry_readline_seqgen_csi0(uint8_t *buf, size_t *idx, char c)
{
    chry_readline_seqgen_csi(buf, idx);
    buf[(*idx)++] = c;
}

/*****************************************************************************
* @brief        output csi 1 pn
*****************************************************************************/
__unused static void chry_readline_seqgen_csi1(uint8_t *buf, size_t *idx, char c, uint16_t pn)
{
    chry_readline_seqgen_csi(buf, idx);
    chry_readline_print_integer(buf, idx, pn);
    buf[(*idx)++] = c;
}

/*****************************************************************************
* @brief        output csi 2 pn
*****************************************************************************/
__unused static void chry_readline_seqgen_csi2(uint8_t *buf, size_t *idx, char c, uint16_t pn, uint16_t pn2)
{
    chry_readline_seqgen_csi(buf, idx);
    chry_readline_print_integer(buf, idx, pn);
    buf[(*idx)++] = ';';
    chry_readline_print_integer(buf, idx, pn2);
    buf[(*idx)++] = c;
}

/*****************************************************************************
* @brief        output csi 3 pn
*****************************************************************************/
__unused static void chry_readline_seqgen_csi3(uint8_t *buf, size_t *idx, char c, uint16_t pn, uint16_t pn2, uint16_t pn3)
{
    chry_readline_seqgen_csi(buf, idx);
    chry_readline_print_integer(buf, idx, pn);
    buf[(*idx)++] = ';';
    chry_readline_print_integer(buf, idx, pn2);
    buf[(*idx)++] = ';';
    chry_readline_print_integer(buf, idx, pn3);
    buf[(*idx)++] = c;
}

/*****************************************************************************
* @brief        Cursor Up Pn Times (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_up(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'A', pn);
}

/*****************************************************************************
* @brief        Cursor Down Pn Times (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_down(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'B', pn);
}

/*****************************************************************************
* @brief        Cursor Forward Pn Times (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_forward(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'C', pn);
}

/*****************************************************************************
* @brief        Cursor Backward Pn Times (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_backward(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'D', pn);
}

/*****************************************************************************
* @brief        Cursor Next Line Pn Times (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_nextline(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'E', pn);
}

/*****************************************************************************
* @brief        Cursor Preceding Line Pn Times (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_prevline(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'F', pn);
}

/*****************************************************************************
* @brief        Cursor Character Absolute [column] (default = [row,1])
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_absolute(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'G', pn);
}

/*****************************************************************************
* @brief        Cursor Position [row;column] (default = [1,1])
*****************************************************************************/
__unused static void chry_readline_seqgen_cursor_position(uint8_t *buf, size_t *idx, uint16_t pn1, uint16_t pn2)
{
    chry_readline_seqgen_csi2(buf, idx, 'H', pn1, pn2);
}

/*****************************************************************************
* @brief        Erase in Display
*                   Pn = 0 → Erase Below (default)
*                   Pn = 1 → Erase Above
*                   Pn = 2 → Erase All
*                   Pn = 3 → Erase Saved Lines (xterm)
*****************************************************************************/
__unused static void chry_readline_seqgen_erase_display(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'J', pn);
}

/*****************************************************************************
* @brief        Erase in Line
*                   Pn = 0 → Erase to Right (default)
*                   Pn = 1 → Erase to Left
*                   Pn = 2 → Erase All
*****************************************************************************/
__unused static void chry_readline_seqgen_erase_line(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'K', pn);
}

/*****************************************************************************
* @brief        Insert Pn Line(s) (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_insert_line(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'L', pn);
}

/*****************************************************************************
* @brief        Delete Pn Line(s) (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_delete_line(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'M', pn);
}

/*****************************************************************************
* @brief        Delete Pn Character(s) (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_delete_char(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'P', pn);
}

/*****************************************************************************
* @brief        Erase Pn Character(s) (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_erase_char(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'X', pn);
}

/*****************************************************************************
* @brief        Scroll up Pn lines (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_scroll_up(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'S', pn);
}

/*****************************************************************************
* @brief        Scroll down Pn lines (default = 1)
*****************************************************************************/
__unused static void chry_readline_seqgen_scroll_down(uint8_t *buf, size_t *idx, uint16_t pn)
{
    chry_readline_seqgen_csi1(buf, idx, 'T', pn);
}

/*****************************************************************************
* @brief        Save Current Cursor Position
*****************************************************************************/
__unused static void chry_readline_seqgen_save_cursor_position(uint8_t *buf, size_t *idx)
{
    chry_readline_seqgen_csi0(buf, idx, 's');
}

/*****************************************************************************
* @brief        Restore Saved Cursor Position
*****************************************************************************/
__unused static void chry_readline_seqgen_restore_cursor_position(uint8_t *buf, size_t *idx)
{
    chry_readline_seqgen_csi0(buf, idx, 'u');
}

/*****************************************************************************
* @brief        DEC Private Mode Reset use alternate screen buffer
*****************************************************************************/
__unused static void chry_readline_seqgen_alternate_screen(uint8_t *buf, size_t *idx)
{
    chry_readline_seqgen_csi(buf, idx);
    buf[(*idx) + 0] = '?';
    buf[(*idx) + 1] = '4';
    buf[(*idx) + 2] = '7';
    buf[(*idx) + 3] = 'h';
    (*idx) += 4;
}

/*****************************************************************************
* @brief        DEC Private Mode Set use normal screen buffer
*****************************************************************************/
__unused static void chry_readline_seqgen_normal_screen(uint8_t *buf, size_t *idx)
{
    chry_readline_seqgen_csi(buf, idx);
    buf[(*idx) + 0] = '?';
    buf[(*idx) + 1] = '4';
    buf[(*idx) + 2] = '7';
    buf[(*idx) + 3] = 'l';
    (*idx) += 4;
}

/*****************************************************************************
* @brief        Report the size of the text area in characters 
*               as CSI 8;row;column t
*****************************************************************************/
__unused static void chry_readline_seqgen_report_screen_size(uint8_t *buf, size_t *idx)
{
    chry_readline_seqgen_csi(buf, idx);
    buf[(*idx) + 0] = '1';
    buf[(*idx) + 1] = '8';
    buf[(*idx) + 2] = 't';
    (*idx) += 3;
}

/*****************************************************************************
* @brief        refresh line
* @retval int               0:Success -1:Error
*****************************************************************************/
int chry_readline_edit_refresh(chry_readline_t *rl)
{
    size_t idx;
    uint8_t seq[16];

    uint16_t curoff;
    uint16_t pptoff;
    uint16_t diff;

    char *linebuff;
    uint16_t linesize;

    linebuff = rl->ln.buff->pbuf;

    if (rl->ln.buff == NULL) {
        linesize = 0;
        curoff = 0;
        pptoff = 0;
    } else {
        linesize = rl->ln.buff->size;
        curoff = rl->ln.curoff;
        pptoff = rl->ln.pptoff;
    }

    if ((pptoff + curoff) >= rl->term.col) {
        diff = pptoff + curoff - rl->term.col + 1;
        linebuff += diff;
        linesize -= diff;
        curoff -= diff;
    }

    if ((pptoff + linesize) > rl->term.col) {
        linesize -= (pptoff + linesize) - rl->term.col;
    }

    linebuff[linesize] = '\0';

    idx = 0;

#if defined(CONFIG_READLINE_REFRESH_PROMPT) && CONFIG_READLINE_REFRESH_PROMPT
    /*!< move to line start */
    chry_readline_seqgen_cursor_absolute(seq, &idx, 0);
    chry_readline_put(rl, seq, idx, -1);
    /*!< output prompt */
    chry_readline_put(rl, rl->prompt, rl->ln.pptlen, -1);
#else
    /*!< move to prompt end */
    chry_readline_seqgen_cursor_absolute(seq, &idx, pptoff + 1);
    chry_readline_put(rl, seq, idx, -1);
#endif

    if (!rl->ln.mask) {
        /*!< output linebuff */
        chry_readline_put(rl, linebuff, linesize, -1);
    }

    /*!< erase to end of display and restore cursor */
    idx = 0;
    chry_readline_seqgen_erase_display(seq, &idx, 0);
    chry_readline_seqgen_cursor_absolute(seq, &idx, pptoff + curoff + 1);
    chry_readline_put(rl, seq, idx, -1);

    return 0;
}

/*****************************************************************************
* @brief        erase to end of line
*****************************************************************************/
void chry_readline_erase_line(chry_readline_t *rl)
{
    size_t idx;
    uint8_t seq[16];

    idx = 0;

    /*!< move to line start */
    chry_readline_seqgen_cursor_absolute(seq, &idx, 0);
    chry_readline_put(rl, seq, idx, );

    /*!< erase to end of display and restore cursor */
    idx = 0;
    chry_readline_seqgen_erase_display(seq, &idx, 0);
    chry_readline_seqgen_cursor_absolute(seq, &idx, 0);
    chry_readline_put(rl, seq, idx, );
}

/*****************************************************************************
* @brief        clear screen and refresh line
* @retval int               0:Success -1:Error
*****************************************************************************/
int chry_readline_edit_clear(chry_readline_t *rl)
{
    chry_readline_put(rl, "\033[2J\033[1;1H", 10, -1);

#if defined(CONFIG_READLINE_REFRESH_PROMPT) && CONFIG_READLINE_REFRESH_PROMPT
    return chry_readline_edit_refresh(rl);
#else
    /*!< output prompt */
    chry_readline_put(rl, rl->prompt, rl->ln.pptlen, -1);
    return chry_readline_edit_refresh(rl);
#endif
}

/*****************************************************************************
* @brief        insert a character, (only support 0x20-0x7e)
* @retval int               0:Success -1:Error        
*****************************************************************************/
int chry_readline_edit_insert(chry_readline_t *rl, char c)
{
    if (rl->ln.buff->size < rl->ln.lnmax) {
        if (rl->ln.buff->size != rl->ln.curoff) {
            memmove(rl->ln.buff->pbuf + rl->ln.curoff + 1,
                    rl->ln.buff->pbuf + rl->ln.curoff,
                    rl->ln.buff->size - rl->ln.curoff);
        }

        rl->ln.buff->pbuf[rl->ln.curoff++] = c;
        rl->ln.buff->size++;

        return chry_readline_edit_refresh(rl);
    }
    return 0;
}

/*****************************************************************************
* @brief        delete cursor left character
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_backspace(chry_readline_t *rl)
{
    if ((rl->ln.curoff > 0) && (rl->ln.buff->size > 0)) {
        memmove(rl->ln.buff->pbuf + rl->ln.curoff - 1,
                rl->ln.buff->pbuf + rl->ln.curoff,
                rl->ln.buff->size - rl->ln.curoff);
        rl->ln.buff->size--;
        rl->ln.curoff--;
        return chry_readline_edit_refresh(rl);
    }
    return 0;
}

/*****************************************************************************
* @brief        delete cursor right character
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_delete(chry_readline_t *rl)
{
    if ((rl->ln.curoff < rl->ln.buff->size) && (rl->ln.buff->size > 0)) {
        memmove(rl->ln.buff->pbuf + rl->ln.curoff,
                rl->ln.buff->pbuf + rl->ln.curoff + 1,
                rl->ln.buff->size - rl->ln.curoff - 1);
        rl->ln.buff->size--;
        return chry_readline_edit_refresh(rl);
    }

    return 0;
}

/*****************************************************************************
* @brief        move cursor to left
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_moveleft(chry_readline_t *rl)
{
    if (rl->ln.curoff > 0) {
        rl->ln.curoff--;
        return chry_readline_edit_refresh(rl);
    }

    return 0;
}

/*****************************************************************************
* @brief        move cursor to right
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_moveright(chry_readline_t *rl)
{
    if (rl->ln.curoff < rl->ln.buff->size) {
        rl->ln.curoff++;
        return chry_readline_edit_refresh(rl);
    }

    return 0;
}

/*****************************************************************************
* @brief        move cursor to start of line
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_movehome(chry_readline_t *rl)
{
    rl->ln.curoff = 0;
    return chry_readline_edit_refresh(rl);
}

/*****************************************************************************
* @brief        move cursor to end of line
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_moveend(chry_readline_t *rl)
{
    rl->ln.curoff = rl->ln.buff->size;
    return chry_readline_edit_refresh(rl);
}

/*****************************************************************************
* @brief        delete the whole line
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_delline(chry_readline_t *rl)
{
    rl->ln.buff->size = 0;
    rl->ln.curoff = 0;
    return chry_readline_edit_refresh(rl);
}

/*****************************************************************************
* @brief        delete from cursor to end of line
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_delend(chry_readline_t *rl)
{
    rl->ln.buff->size = rl->ln.curoff;
    return chry_readline_edit_refresh(rl);
}

/*****************************************************************************
* @brief        delete previosu word
* @retval int               0:Success -1:Error    
*****************************************************************************/
int chry_readline_edit_delword(chry_readline_t *rl)
{
    uint16_t curoff = rl->ln.curoff;

    while ((curoff > 0) && (rl->ln.buff->pbuf[curoff - 1] == ' ')) {
        curoff--;
    }

    while ((curoff > 0) && (rl->ln.buff->pbuf[curoff - 1] != ' ')) {
        curoff--;
    }

    memmove(rl->ln.buff->pbuf + curoff,
            rl->ln.buff->pbuf + rl->ln.curoff,
            rl->ln.buff->size - rl->ln.curoff + 1);
    rl->ln.buff->size -= rl->ln.curoff - curoff;
    rl->ln.curoff = curoff;

    return chry_readline_edit_refresh(rl);
}

/*****************************************************************************
* @brief        get now word
*****************************************************************************/
__unused static void chry_readline_edit_getword(chry_readline_t *rl, char **pword, uint16_t *size)
{
    uint16_t curoff = rl->ln.curoff;

    if ((curoff == 0) || (rl->ln.buff->pbuf[curoff - 1] == ' ')) {
        goto end;
    }

    while (curoff > 0) {
        curoff--;
        if (rl->ln.buff->pbuf[curoff] == ' ') {
            curoff++;
            break;
        }
    }

end:

    *pword = &(rl->ln.buff->pbuf[curoff]);
    *size = rl->ln.curoff - curoff;

    return;
}

/*****************************************************************************
* @brief        show help
* @retval int               0:Success -1:Error  
*****************************************************************************/
static int chry_readline_help(chry_readline_t *rl)
{
    if (sizeof(CONFIG_READLINE_HELP) - 1) {
        chry_readline_put(rl, CONFIG_READLINE_HELP, sizeof(CONFIG_READLINE_HELP) - 1, -1);
    }
    return 0;
}

/*****************************************************************************
* @brief        dispatch csi sequences
* 
* @param[in]    rl          readline instance
* @param[inout] c           character for dispatch
* @param[in]    pns         params for dispatch
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_dispatch_csi(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    uint8_t idx;

    if (*c == '~') {
        if (pns[0] >= sizeof(vtmap)) {
            pns[0] = 0;
        }

        *c = vtmap[pns[0]];
    } else if (*c == 't') {
        if (pns[0] == 8) {
            rl->term.nsupt = 0;
            rl->term.row = pns[1];
            rl->term.col = pns[2];
#if defined(CONFIG_READLINE_XTERM) && CONFIG_READLINE_XTERM
            /*!< 1.enter alternate screen buffer <esc>[?47h */
            /*!< 2.clear screen buffer           <esc>[2J   */
            /*!< 3.move cursor to (1,1)          <esc>[1;1H */
            /*!< 4.enter normal screen buffer    <esc>[?47l */
            chry_readline_put(rl, "\033[?47h\033[2J\033[1;1H", 16, -1);
            chry_readline_help(rl);
            chry_readline_put(rl, "\033[?47l", 6, -1);
#endif
        }

        *c = CHRY_READLINE_EXEC_NUL;
    } else {
        idx = *c - 0x40;

        if (idx >= sizeof(xmap)) {
            idx = 0;
        }

        *c = xmap[idx];
    }

    return 0;
}

/*****************************************************************************
* @brief        dispatch o sequences
* 
* @param[in]    rl          readline instance
* @param[inout] c           character for dispatch
* @param[in]    pns         params for dispatch     
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_dispatch_o(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    (void)rl;
    (void)pns;

    switch (*c) {
        case 'P':
            *c = CHRY_READLINE_EXEC_F1;
            break;
        case 'Q':
            *c = CHRY_READLINE_EXEC_F2;
            break;
        case 'R':
            *c = CHRY_READLINE_EXEC_F3;
            break;
        case 'S':
            *c = CHRY_READLINE_EXEC_F4;
            break;
        default:
            *c = CHRY_READLINE_EXEC_NUL;
            break;
    }

    return 0;
}

/*****************************************************************************
* @brief        dispatch alt sequences
* 
* @param[in]    rl          readline instance
* @param[inout] c           character for dispatch
* @param[in]    pns         params for dispatch     
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_dispatch_alt(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    (void)rl;
    (void)pns;

    if ((*c < 'a') || ('z' < *c)) {
        *c = CHRY_READLINE_EXEC_NUL;
        return 0;
    }

#if defined(CONFIG_READLINE_ALTMAP) && CONFIG_READLINE_ALTMAP
    *c = rl->altmap[*c - 'a'];
#else
    *c = altmap[*c - 'a'];
#endif

    return 0;
}

/*****************************************************************************
* @brief        dispatch ctrl sequences
* 
* @param[in]    rl          readline instance
* @param[inout] c           character for dispatch
* @param[in]    pns         params for dispatch     
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_dispatch_ctrl(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    (void)rl;
    (void)pns;

#if defined(CONFIG_READLINE_CTRLMAP) && CONFIG_READLINE_CTRLMAP
    *c = rl->ctrlmap[*c];
#else
    *c = ctrlmap[*c];
#endif

    return 0;
}

/*****************************************************************************
* @brief        parse csi sequences
* 
* @param[in]    rl          readline instance
* @param[out]   csiend      final csi seq character
* @param[out]   expre       extend prefix character
* @param[out]   pns         array of number param
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_seqexec_pcsi(chry_readline_t *rl, uint8_t *csiend, uint8_t *expre, uint16_t *pns)
{
    uint8_t csibuf[8];
    uint8_t *pbuf = csibuf;

    /*!< should have buffered key or it will be blocked */
    chry_readline_waitkey(rl, pbuf);

    if (('0' <= *pbuf) && (*pbuf <= '9')) {
        pbuf++;
    } else if ('?' == *pbuf) {
        *expre = *pbuf;
    } else if ((('a' <= *pbuf) && (*pbuf <= 'z')) ||
               (('A' <= *pbuf) && (*pbuf <= 'Z'))) {
        *csiend = *pbuf;
        return 0;
    }

    for (;;) {
        /*!< should have buffered key or it will be blocked */
        chry_readline_waitkey(rl, pbuf);

        if (('0' <= *pbuf) && (*pbuf <= '9')) {
            pbuf++;
        } else if (*pbuf == ';') {
            pbuf = csibuf;
            chry_readline_parse_integer(csibuf, pns++);
        } else if ((*pbuf == '~') ||
                   (('a' <= *pbuf) && (*pbuf <= 'z')) ||
                   (('A' <= *pbuf) && (*pbuf <= 'Z'))) {
            *csiend = *pbuf;
            chry_readline_parse_integer(csibuf, pns++);
            return 0;
        } else {
            return -1;
        }
    }
}

/*****************************************************************************
* @brief        execute CSI sequences
* 
* @param[in]    rl          readline instance
* @param[out]   c           character for dispatch
* @param[out]   pns         array of number param
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_seqexec_csi(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    uint8_t expre = 0;

    if (chry_readline_seqexec_pcsi(rl, c, &expre, pns)) {
        return -1;
    }

    if (expre) {
        *c = CHRY_READLINE_C0_NUL;
    }

    return chry_readline_dispatch_csi(rl, c, pns);
}

/*****************************************************************************
* @brief        execute ALT sequences
* 
* @param[in]    rl          readline instance
* @param[inout]   c         character for dispatch
* @param[out]   pns         array of number param
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_seqexec_alt(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    return chry_readline_dispatch_alt(rl, c, pns);
}

/*****************************************************************************
* @brief        execute O sequences
* 
* @param[in]    rl          readline instance
* @param[out]   c           character for dispatch
* @param[out]   pns         array of number param
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_seqexec_o(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    /*!< should have buffered key or it will be blocked */
    chry_readline_waitkey(rl, c);

    return chry_readline_dispatch_o(rl, c, pns);
}

/*****************************************************************************
* @brief        execute sequences
* 
* @param[in]    rl          readline instance
* @param[inout] c           character for dispatch
* @param[out]   pns         array of number param
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
static int chry_readline_seqexec(chry_readline_t *rl, uint8_t *c, uint16_t *pns)
{
    /*!< should have buffered key or it will be blocked */
    chry_readline_waitkey(rl, c);

    switch (*c) {
        case 'O':
            return chry_readline_seqexec_o(rl, c, pns);
        case '[':
            return chry_readline_seqexec_csi(rl, c, pns);
        default:
            return chry_readline_seqexec_alt(rl, c, pns);
    }

    return 0;
}

#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY

/*****************************************************************************
* @brief        reverse memcpy 2byte align
* 
* @param[in]    dst         
* @param[in]    src         
* @param[in]    n           byte
* 
*****************************************************************************/
static void chry_readline_rvsememcpy2(void *dst, const void *src, size_t n)
{
    uint16_t *pout = dst;
    uint16_t *pin = (uint16_t *)src - 1;

    for (; n > 0; n -= 2) {
        *pout++ = *pin--;
    }
}

/*****************************************************************************
* @brief        align 2 up
* 
* @param[in]    size        size
* 
* @retval                   align size
*****************************************************************************/
static uint32_t chry_readline_align2(uint32_t size)
{
    if (size & 0x1) {
        return (size & ~0x1) + 2;
    } else {
        return size;
    }
}

/*****************************************************************************
* @brief        cache linbuff to history list
* 
* @param[in]    rl          readline instance
* 
*****************************************************************************/
static void chry_readline_history_cache(chry_readline_t *rl)
{
    uint16_t unused;
    uint16_t offset;
    uint16_t remain;
    uint16_t lnsize;
    uint8_t *dst;

    /*!< fifo unused size */
    unused = rl->hist.size - (rl->hist.cache - rl->hist.out);

    /*!< line and size total size, alignup 2 */
    lnsize = chry_readline_align2(rl->ln.buff->size + 2);

    /*!< free the oldest history */
    if (lnsize > unused) {
        rl->hist.out += lnsize - unused;
    }

    /*!< fifo write offset */
    offset = rl->hist.cache & rl->hist.mask;

    /*!< unused size before ringloop */
    remain = rl->hist.size - offset;
    remain = remain > lnsize ? lnsize : remain;

    dst = (uint8_t *)(rl->ln.buff) + lnsize;

    chry_readline_rvsememcpy2(rl->hist.pbuf + offset, dst, remain);
    chry_readline_rvsememcpy2(rl->hist.pbuf, dst - remain, lnsize - remain);

    rl->hist.in = rl->hist.cache + lnsize;
}

/*****************************************************************************
* @brief        store linebuff to history list
* 
* @param[in]    rl          readline instance
*                
*****************************************************************************/
static void chry_readline_history_store(chry_readline_t *rl)
{
    if (rl->ln.buff->size > 0) {
        chry_readline_history_cache(rl);

        /*!< update cache index */
        rl->hist.cache = rl->hist.in;
    }
}

/*****************************************************************************
* @brief        load special index history to linebuff
* 
* @param[in]    rl          readline instance
* @param[in]    index       history index
* 
* @retval                   0:Success -1:Error
*****************************************************************************/
static int chry_readline_history_load(chry_readline_t *rl, uint16_t index)
{
    uint16_t used;
    uint16_t offset;
    uint16_t remain;
    uint16_t lnsize;
    uint16_t histin;
    uint16_t count;
    uint16_t prevused;

    if (rl->hist.index == 0) {
        chry_readline_history_cache(rl);
    }

    lnsize = 0;
    count = index;
    histin = rl->hist.in;
    prevused = rl->hist.in - rl->hist.out;

    do {
        histin -= lnsize;

        /*!< fifo used size */
        used = histin - rl->hist.out;

        if ((used == 0) || (used > prevused)) {
            return 0;
        }

        prevused = used;

        /*!< fifo read offset */
        offset = (histin - 2) & rl->hist.mask;

        /*!< read fisrt 2byte and alignup 2 */
        lnsize = chry_readline_align2(((uint32_t *)(rl->hist.pbuf + offset))[0] + 2);

        if (lnsize > used) {
            return 0;
        }

    } while (count--);

    offset = (histin - lnsize) & rl->hist.mask;

    /*!< unused size before ringloop */
    remain = rl->hist.size - offset;
    remain = remain > lnsize ? lnsize : remain;

    /*!< temp value */
    lnsize = lnsize - remain;

    chry_readline_rvsememcpy2((uint8_t *)(rl->ln.buff) + lnsize,
                              rl->hist.pbuf + offset + remain, remain);

    chry_readline_rvsememcpy2(rl->ln.buff, rl->hist.pbuf + lnsize, lnsize);

    rl->ln.curoff = rl->ln.buff->size;
    rl->hist.index = index;
    return chry_readline_edit_refresh(rl);
}

/*****************************************************************************
* @brief        load prev history to linebuff
* 
* @param[in]    rl          readline instance
*
* @retval                   0:Success -1:Error
*****************************************************************************/
static int chry_readline_history_loadprev(chry_readline_t *rl)
{
    uint16_t lnhist = rl->hist.index + 1;

    return chry_readline_history_load(rl, lnhist);
}

/*****************************************************************************
* @brief        load next history to linebuff
* 
* @param[in]    rl          readline instance
*
* @retval                   0:Success -1:Error
*****************************************************************************/
static int chry_readline_history_loadnext(chry_readline_t *rl)
{
    uint16_t lnhist = rl->hist.index > 0 ? rl->hist.index - 1 : 0;

    return chry_readline_history_load(rl, lnhist);
}

#endif

/*****************************************************************************
* @brief        auto completion
* 
* @param[in]    rl          readline instance
* 
* @retval int               0:Success -1:Error 1:Restart
*****************************************************************************/
int chry_readline_complete(chry_readline_t *rl)
{
#if defined(CONFIG_READLINE_COMPLETION) && CONFIG_READLINE_COMPLETION
    uint8_t count;
    char *pre;
    uint16_t word_size;

    const char *argv[CONFIG_READLINE_MAX_COMPLETION];
    uint8_t argl[CONFIG_READLINE_MAX_COMPLETION];

    if (rl->cplt.acb) {
        chry_readline_edit_getword(rl, &pre, &word_size);

        if ((count = rl->cplt.acb(rl, pre, &word_size, argv, argl, CONFIG_READLINE_MAX_COMPLETION)) == 0) {
            /*!< if no completions, return */
            return 0;
        }

        uint16_t match_size = 0;
        uint16_t extend_size;
        if (count == 1) {
            match_size = argl[0];
        } else {
            bool flag = true;
            while (flag) {
                for (uint16_t i = 0; i < count - 1; i++) {
                    char c1 = argv[i + 0][match_size];
                    char c2 = argv[i + 1][match_size];
                    if ((0 == c1) || (0 == c2) || (c1 != c2)) {
                        flag = false;
                        break;
                    }
                }
                if (flag) {
                    match_size++;
                }
            }
        }

        if (match_size > word_size) {
            uint16_t free_size = rl->ln.lnmax - rl->ln.buff->size;
            extend_size = match_size - word_size;
            extend_size = extend_size < free_size ? extend_size : free_size;
            if (0 == extend_size) {
                return 0;
            }

            if (rl->ln.buff->size != rl->ln.curoff) {
                memmove(rl->ln.buff->pbuf + rl->ln.curoff + extend_size,
                        rl->ln.buff->pbuf + rl->ln.curoff,
                        rl->ln.buff->size - rl->ln.curoff);
            }

            memcpy(&(rl->ln.buff->pbuf[rl->ln.curoff]), &argv[0][word_size], extend_size);
            rl->ln.curoff += extend_size;
            rl->ln.buff->size += extend_size;
            return chry_readline_edit_refresh(rl);
        }

        uint16_t longest_size = 0;
        for (uint16_t i = 0; i < count; i++) {
            uint16_t cplt_size = argl[i];
            if (cplt_size > longest_size) {
                longest_size = cplt_size;
            }
        }

        longest_size += 2;

        uint8_t col_count = rl->term.col / longest_size;
        if (col_count < 1) {
            col_count = 1;
        }

        uint16_t row_count = (count + col_count - 1) / col_count;

        for (uint16_t row = 0; row < row_count; row++) {
            chry_readline_put(rl, CONFIG_READLINE_NEWLINE, sizeof(CONFIG_READLINE_NEWLINE) ? sizeof(CONFIG_READLINE_NEWLINE) - 1 : 0, -1);

            for (int col = 0; col < col_count; col++) {
                int idx = (col * row_count) + row;
                if (idx < count) {
                    uint16_t cplt_size = argl[idx];

                    chry_readline_put(rl, (void *)argv[idx], cplt_size, -1);

                    if (((col + 1) * row_count) + row < count) {
                        for (int k = cplt_size; k < longest_size; k++) {
                            chry_readline_put(rl, " ", 1, -1);
                        }
                    }
                }
            }
        }
    }

    chry_readline_put(rl, CONFIG_READLINE_NEWLINE, sizeof(CONFIG_READLINE_NEWLINE) ? sizeof(CONFIG_READLINE_NEWLINE) - 1 : 0, -1);
    return 1;
#else
    (void)rl;
    return 0;
#endif
}

/*****************************************************************************
* @brief        calculate prompt strlen and cursor offset
*****************************************************************************/
static void chry_readline_calculate_prompt(chry_readline_t *rl)
{
    char c;
    uint8_t flag;

    flag = 0;
    rl->ln.pptlen = 0;
    rl->ln.pptoff = 0;

    while (1) {
        c = rl->prompt[rl->ln.pptlen];

        if (c == '\0') {
            return;
        } else if (c == '\r') {
            rl->ln.pptoff = 0;
        } else if (c == '\033') {
            flag = 1;
        } else if ((flag == 0) && (c != '\n')) {
            rl->ln.pptoff++;
        } else if ((flag == 1) && (c == 'm')) {
            flag = 0;
        }

        rl->ln.pptlen++;
        if (rl->ln.pptlen >= rl->ln.pptsize) {
            rl->ln.pptlen = 0;
            rl->ln.pptoff = 0;
            return;
        }
    }
}

/*****************************************************************************
* @brief        wait switch to altscreen
* @retval int               0:Success -1:Error
*****************************************************************************/
__unused static int chry_readline_wait_altscreen(chry_readline_t *rl)
{
    uint32_t c = 0;

    for (;;) {
        chry_readline_waitkey(rl, &c);

        if ((c == CHRY_READLINE_C0_CR) || (c == CHRY_READLINE_C0_LF)) {
            chry_readline_altscreen(rl, 1);
            return 0;
        }
    }
}

/*****************************************************************************
* @brief        readline internal
* 
* @param[in]    rl          readline instance
* 
* @retval char*             line pointer
*****************************************************************************/
static char *chry_readline_inernal(chry_readline_t *rl)
{
#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
    if (rl->noblock && !rl->block) {
        goto restore;
    }
#endif

#if defined(CONFIG_READLINE_XTERM) && CONFIG_READLINE_XTERM
    /*!< wait switch to altscreen */
    if (rl->term.altnsupt == 0) {
        chry_readline_wait_altscreen(rl);
    }
#endif

restart:

    if (rl->auto_refresh) {
#if defined(CONFIG_READLINE_REFRESH_PROMPT) && CONFIG_READLINE_REFRESH_PROMPT
        if (chry_readline_edit_refresh(rl)) {
            return NULL;
        }
#else
        /*!< output prompt */
        chry_readline_put(rl, rl->prompt, rl->ln.pptlen, NULL);

        if (chry_readline_edit_refresh(rl)) {
            return NULL;
        }
#endif
    }

    while (1) {
        uint8_t c = 0;
        uint16_t pn[4];

#if defined(CONFIG_READLINE_XTERM) && CONFIG_READLINE_XTERM
        /*!< wait switch to altscreen */
        if (rl->term.altnsupt == 0) {
            chry_readline_wait_altscreen(rl);
        }
#endif

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
    restore:
#endif

        /*!< get a key */
        chry_readline_getkey(rl, &c);

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
        if (rl->noblock && !rl->block) {
            return NULL;
        }
#endif

        if (c & 0x80) {
            /*!< not support 8bit code */
            continue;
        }

        if (CHRY_READLINE_G0_BEG <= c) {
            if (c <= CHRY_READLINE_G0_END) {
                if (rl->ignore) {
                    continue;
                }
                /*!< printable characters */
                if (chry_readline_edit_insert(rl, c)) {
                    return NULL;
                }
                continue;
            } else {
                /*!< delete */
                c = CHRY_READLINE_EXEC_BS;
            }
        } else if (CHRY_READLINE_C0_ESC == c) {
            if (chry_readline_seqexec(rl, &c, pn)) {
                return NULL;
            }
        } else {
            if (chry_readline_dispatch_ctrl(rl, &c, pn)) {
                return NULL;
            }
        }

        if (rl->ignore) {
            if (c == CHRY_READLINE_EXEC_SIGINT_) {
                c = CHRY_READLINE_EXEC_SIGINT;
            }

            if (c < CHRY_READLINE_EXEC_SIGINT || CHRY_READLINE_EXEC_SIGTSTP < c) {
                continue;
            }
        }

    rehandle:
        /*!< hanlde control */
        switch (c) {
            /*!< ignore */
            case CHRY_READLINE_EXEC_NUL:
                break;

            /*!< clear screen and refresh line */
            case CHRY_READLINE_EXEC_CLR:
                if (chry_readline_edit_clear(rl)) {
                    return NULL;
                }
                break;

            /*!< newline */
            case CHRY_READLINE_EXEC_NLN:
#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY
                if (!rl->ln.mask) {
                    chry_readline_history_store(rl);
                }
#endif
                return rl->ln.buff->pbuf;

            /*!< abortline */
            case CHRY_READLINE_EXEC_ALN:
                rl->ln.buff->size = 0;
                return rl->ln.buff->pbuf;

            /*!< delete */
            case CHRY_READLINE_EXEC_DEL:
                if (chry_readline_edit_delete(rl)) {
                    return NULL;
                }
                break;

            /*!< backspace */
            case CHRY_READLINE_EXEC_BS:
                if (chry_readline_edit_backspace(rl)) {
                    return NULL;
                }
                break;

            /*!< move right */
            case CHRY_READLINE_EXEC_MVRT:
                if (chry_readline_edit_moveright(rl)) {
                    return NULL;
                }
                break;

            /*!< move left */
            case CHRY_READLINE_EXEC_MVLT:
                if (chry_readline_edit_moveleft(rl)) {
                    return NULL;
                }
                break;

            /*!< move end */
            case CHRY_READLINE_EXEC_MVED:
                if (chry_readline_edit_moveend(rl)) {
                    return NULL;
                }
                break;

            /*!< move home */
            case CHRY_READLINE_EXEC_MVHM:
                if (chry_readline_edit_movehome(rl)) {
                    return NULL;
                }
                break;

            /*!< fetch next history */
            case CHRY_READLINE_EXEC_NXTH:
#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY
                if (!rl->ln.mask) {
                    if (chry_readline_history_loadnext(rl)) {
                        return NULL;
                    }
                }
#endif
                break;

            /*!< fetch prev history */
            case CHRY_READLINE_EXEC_PRVH:
#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY
                if (!rl->ln.mask) {
                    if (chry_readline_history_loadprev(rl)) {
                        return NULL;
                    }
                }
#endif
                break;

            /*!< delete prev word */
            case CHRY_READLINE_EXEC_DLWD:
                if (chry_readline_edit_delword(rl)) {
                    return NULL;
                }
                break;

            /*!< delete whole line */
            case CHRY_READLINE_EXEC_DHLN:
                if (chry_readline_edit_delline(rl)) {
                    return NULL;
                }
                break;

            /*!< delete cursor to end */
            case CHRY_READLINE_EXEC_DELN:
                if (chry_readline_edit_delend(rl)) {
                    return NULL;
                }
                break;

            /*!< auto complete */
            case CHRY_READLINE_EXEC_CPLT:
#if defined(CONFIG_READLINE_COMPLETION) && CONFIG_READLINE_COMPLETION
            {
                /*!< disable completion in mask mode */
                if (rl->ln.mask) {
                    break;
                }

                int ret = chry_readline_complete(rl);
                if (ret < 0) {
                    return NULL;
                } else if (ret == 0) {
                    break;
                } else {
                    goto restart;
                }
            }
#else
                break;
#endif

            case CHRY_READLINE_EXEC_ACPLT: {
                /*!< if cursor left char is space */
                if ((rl->ln.curoff == 0) || (rl->ln.buff->pbuf[rl->ln.curoff - 1] == ' ')) {
                    int spcnt = CONFIG_READLINE_SPACE - (rl->ln.curoff % CONFIG_READLINE_SPACE);
                    for (uint8_t i = 0; i < spcnt; i++) {
                        chry_readline_edit_insert(rl, ' ');
                    }
                    break;
                } else {
                    /*!< disable completion in mask mode */
                    if (rl->ln.mask) {
                        break;
                    }

                    int ret = chry_readline_complete(rl);
                    if (ret < 0) {
                        return NULL;
                    } else if (ret == 0) {
                        break;
                    } else {
                        goto restart;
                    }
                }
            }

            /*!< switch to normal screen */
            case CHRY_READLINE_EXEC_SWNM:
                chry_readline_altscreen(rl, 0);
                break;

            case CHRY_READLINE_EXEC_HELP:
                if (chry_readline_help(rl)) {
                    return NULL;
                }
                rl->ln.buff->size = 0;
                return rl->ln.buff->pbuf;

            case CHRY_READLINE_EXEC_EOF_:
                if (rl->ln.buff->size == 0) {
                    c = CHRY_READLINE_EXEC_EOF;
                    goto rehandle;
                }
                if (chry_readline_edit_delete(rl)) {
                    return NULL;
                }
                break;

            case CHRY_READLINE_EXEC_SIGINT_:
                if (rl->ln.buff->size == 0) {
                    c = CHRY_READLINE_EXEC_SIGINT;
                    goto rehandle;
                } else {
                    rl->ln.buff->size = 0;
                    return rl->ln.buff->pbuf;
                }
                break;

            case CHRY_READLINE_EXEC_EOF:
            case CHRY_READLINE_EXEC_SIGINT:
            case CHRY_READLINE_EXEC_SIGQUIT:
            case CHRY_READLINE_EXEC_SIGCONT:
            case CHRY_READLINE_EXEC_SIGSTOP:
            case CHRY_READLINE_EXEC_SIGTSTP:
            case CHRY_READLINE_EXEC_F1:
            case CHRY_READLINE_EXEC_F2:
            case CHRY_READLINE_EXEC_F3:
            case CHRY_READLINE_EXEC_F4:
            case CHRY_READLINE_EXEC_F5:
            case CHRY_READLINE_EXEC_F6:
            case CHRY_READLINE_EXEC_F7:
            case CHRY_READLINE_EXEC_F8:
            case CHRY_READLINE_EXEC_F9:
            case CHRY_READLINE_EXEC_F10:
            case CHRY_READLINE_EXEC_F11:
            case CHRY_READLINE_EXEC_F12:
            default:
                if (rl->ucb) {
                    int ret = rl->ucb(rl, c);
                    if (ret == 0) {
                        goto restart;
                    } else if (ret < 0) {
                        return NULL;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
        }
    }
}

/*****************************************************************************
* @brief        enable or disable altscreen buffer (xterm only) 
* 
* @param[in]    rl          readline instance
* @param[in]    enable      altscreen enable
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int chry_readline_altscreen(chry_readline_t *rl, uint8_t enable)
{
    (void)enable;

    if (rl->term.nsupt) {
        return -1;
    }

#if defined(CONFIG_READLINE_XTERM) && CONFIG_READLINE_XTERM
    if (enable) {
        chry_readline_put(rl, "\033[?47h", 6, );
        rl->term.alt = 1;
    } else {
        chry_readline_put(rl, "\033[?47l", 6, );
        rl->term.alt = 0;
    }
#endif

    return 0;
}

/*****************************************************************************
* @brief        Fill Character Attributes SGR set to sgrraw
* 
* @param[in]    buf         buffer for fill, max 17byte
* @param[in]    sgrraw      sgr attributes, 0 to reset
* 
*****************************************************************************/
__unused static uint8_t chry_readline_sgrset(char *buf, uint16_t sgrraw)
{
    chry_readline_sgr_t sgr = { .raw = sgrraw };
    size_t idx = 0;
    chry_readline_seqgen_csi((void *)buf, &idx);

    if (sgrraw == 0) {
        buf[idx++] = 'm';
        buf[idx] = '\0';
        return idx;
    }

    if (sgr.bold) {
        buf[idx + 0] = '1';
        buf[idx + 1] = ';';
        idx += 2;
    }

    if (sgr.underline) {
        buf[idx + 0] = '4';
        buf[idx + 1] = ';';
        idx += 2;
    }

    if (sgr.blink) {
        buf[idx + 0] = '5';
        buf[idx + 1] = ';';
        idx += 2;
    }

    if (sgr.inverse) {
        buf[idx + 0] = '8';
        buf[idx + 1] = ';';
        idx += 2;
    }

    if (sgr.foreground) {
        buf[idx + 0] = '3';
        buf[idx + 1] = '0' + (sgr.foreground - 1);
        buf[idx + 2] = ';';
        idx += 3;
    }

    if (sgr.background) {
        buf[idx + 0] = '4';
        buf[idx + 1] = '0' + (sgr.background - 1);
        buf[idx + 2] = ';';
        idx += 3;
    }

    buf[idx - 1] = 'm';
    buf[idx] = '\0';

    return idx;
}

/*****************************************************************************
* @brief        output CONFIG_READLINE_NEWLINE
* 
* @param[in]    rl          readline instance
* 
*****************************************************************************/
void chry_readline_newline(chry_readline_t *rl)
{
    chry_readline_put(rl, CONFIG_READLINE_NEWLINE, sizeof(CONFIG_READLINE_NEWLINE) ? sizeof(CONFIG_READLINE_NEWLINE) - 1 : 0, );
}

/*****************************************************************************
* @brief        clear screen and refresh line
* 
* @param[in]    rl          readline instance
* 
*****************************************************************************/
void chry_readline_clear(chry_readline_t *rl)
{
    CHRY_READLINE_PARAM_CHECK(NULL != rl, );

    chry_readline_edit_clear(rl);
}

/*****************************************************************************
* @brief        enable or disable block mode
* 
* @param[in]    rl          readline instance
* @param[in]    enable      block enable
* 
*****************************************************************************/
void chry_readline_block(chry_readline_t *rl, uint8_t enable)
{
    CHRY_READLINE_PARAM_CHECK(NULL != rl, );

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
    rl->block = enable ? 1 : 0;
#else
    (void)rl;
    (void)enable;
#endif
}

/*****************************************************************************
* @brief        enable or disable ignore mode
* 
* @param[in]    rl          readline instance
* @param[in]    enable      ignore enable
* 
*****************************************************************************/
void chry_readline_ignore(chry_readline_t *rl, uint8_t enable)
{
    CHRY_READLINE_PARAM_CHECK(NULL != rl, );

    rl->ignore = enable ? 1 : 0;
}

/*****************************************************************************
* @brief        enable or disable ignore mode
* 
* @param[in]    rl          readline instance
* @param[in]    enable      ignore enable
* 
*****************************************************************************/
void chry_readline_auto_refresh(chry_readline_t *rl, uint8_t enable)
{
    CHRY_READLINE_PARAM_CHECK(NULL != rl, );
    rl->auto_refresh = enable ? 1 : 0;
}

/*****************************************************************************
* @brief        enable or disable mask mode
* 
* @param[in]    rl          readline instance
* @param[in]    enable      mask enable
* 
*****************************************************************************/
void chry_readline_mask(chry_readline_t *rl, uint8_t enable)
{
    CHRY_READLINE_PARAM_CHECK(NULL != rl, );

    rl->ln.mask = enable ? 1 : 0;

    if (NULL != rl->ln.buff) {
        chry_readline_edit_refresh(rl);
    }
}

/*****************************************************************************
* @brief        readline
* 
* @param[in]    rl          readline instance
* @param[in]    linebuff    linebuff pointer
* @param[in]    buffsize    linebuff size
* @param[in]    linesize    readline size (set NULL to ignore)
* 
* @retval char*             line pointer
*****************************************************************************/
char *chry_readline(chry_readline_t *rl, char *linebuff, uint16_t buffsize, uint16_t *linesize)
{
    char *ret;
    CHRY_READLINE_PARAM_CHECK(NULL != rl, NULL);
    CHRY_READLINE_PARAM_CHECK(NULL != linebuff, NULL);

    if (buffsize <= 3) {
        return NULL;
    }

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
    if (rl->noblock && !rl->block) {
        goto restore;
    }
#endif

    rl->ln.buff = (void *)linebuff;
    rl->ln.buff->size = 0;
    rl->ln.lnmax = buffsize - 3; /*!< reserved for \0 and size */
    rl->ln.curoff = 0;

#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY
    rl->hist.index = 0;
#endif

    /*!< calculate prompt offset and size */
    chry_readline_calculate_prompt(rl);

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
restore:
#endif

    ret = chry_readline_inernal(rl);

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
    if (rl->noblock && !rl->block) {
        return (void *)-1;
    }
#endif

    /*!< new line */
    chry_readline_put(rl, CONFIG_READLINE_NEWLINE, sizeof(CONFIG_READLINE_NEWLINE) ? sizeof(CONFIG_READLINE_NEWLINE) - 1 : 0, NULL);

    if (NULL != linesize) {
        *linesize = rl->ln.buff->size;
    }

    rl->ln.buff->pbuf[rl->ln.buff->size] = '\0';
    rl->ln.buff = NULL;

    return ret;
}

/*****************************************************************************
* @brief        init readline
* 
* @param[in]    rl          readline instance
* @param[in]    init        init param
* 
* @retval int               0:Success -1:Error              
*****************************************************************************/
int chry_readline_init(chry_readline_t *rl, chry_readline_init_t *init)
{
    CHRY_READLINE_PARAM_CHECK(NULL != rl, -1);
    CHRY_READLINE_PARAM_CHECK(NULL != init, -1);
    CHRY_READLINE_PARAM_CHECK(NULL != init->sget, -1);
    CHRY_READLINE_PARAM_CHECK(NULL != init->sput, -1);
    CHRY_READLINE_PARAM_CHECK(NULL != init->prompt, -1);
#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY
    CHRY_READLINE_PARAM_CHECK(NULL != init->history, -1);
#endif

    if ((init->histsize < 2) || (init->histsize & (init->histsize - 1))) {
        return -1;
    }

    rl->prompt = init->prompt;
    rl->sget = init->sget;
    rl->sput = init->sput;

#if defined(CONFIG_READLINE_HISTORY) && CONFIG_READLINE_HISTORY
    rl->hist.pbuf = init->history;
    rl->hist.in = 0;
    rl->hist.out = 0;
    rl->hist.mask = init->histsize - 1;
    rl->hist.size = init->histsize;
    rl->hist.cache = 0;
#endif

#if defined(CONFIG_READLINE_COMPLETION) && CONFIG_READLINE_COMPLETION
    rl->cplt.acb = NULL;
#endif

#if defined(CONFIG_READLINE_PROMPTEDIT) && CONFIG_READLINE_PROMPTEDIT
    memset(rl->pptseglen, 0, CONFIG_READLINE_PROMPTSEG + 1);
#endif

    rl->term.nsupt = 1;
    rl->term.alt = 0;
    rl->term.row = CONFIG_READLINE_DFTROW;
    rl->term.col = CONFIG_READLINE_DFTCOL;

    rl->ln.buff = NULL;
    rl->ln.mask = 0;
    rl->ln.pptsize = init->pptsize;

    rl->ucb = NULL;

    rl->ignore = false;
    rl->auto_refresh = true;

#if defined(CONFIG_READLINE_CTRLMAP) && CONFIG_READLINE_CTRLMAP
    memcpy(rl->ctrlmap, ctrlmap, sizeof(ctrlmap));
#endif

#if defined(CONFIG_READLINE_ALTMAP) && CONFIG_READLINE_ALTMAP
    memcpy(rl->altmap, altmap, sizeof(altmap));
#endif

#if defined(CONFIG_READLINE_NOBLOCK) && CONFIG_READLINE_NOBLOCK
    rl->noblock = false;
    rl->block = false;
#endif

    return 0;
}

/*****************************************************************************
* @brief        detects if it is xterm, result will be processed automatically
*               if not xterm it will be blocked on next character
* 
* @param[in]    rl          readline instance
*****************************************************************************/
void chry_readline_detect(chry_readline_t *rl)
{
    uint8_t c;
    uint16_t pn[4];

    /*!< test sput and get screen size */
    if (5 != rl->sput(rl, "\033[18t", 5)) {
        return;
    }

    chry_readline_waitkey(rl, &c);

    if (c == CHRY_READLINE_C0_ESC) {
        /*!< execute sequence */
        chry_readline_seqexec(rl, &c, pn);
    }
}

/*****************************************************************************
* @brief        set auto complete callback
* 
* @param[in]    rl          readline instance
* @param[in]    acb         callback
* 
*****************************************************************************/
void chry_readline_set_completion_cb(chry_readline_t *rl, uint8_t (*acb)(chry_readline_t *rl, char *pre, uint16_t *size, const char **argv, uint8_t *argl, uint8_t argcmax))
{
    (void)rl;
    (void)acb;
#if defined(CONFIG_READLINE_COMPLETION) && CONFIG_READLINE_COMPLETION
    rl->cplt.acb = acb;
#endif
}

/*****************************************************************************
* @brief        set user event exec callback
* 
* @param[in]    rl          readline instance
* @param[in]    ucb         callback
* 
*****************************************************************************/
void chry_readline_set_user_cb(chry_readline_t *rl, int (*ucb)(chry_readline_t *rl, uint8_t exec))
{
    rl->ucb = ucb;
}

/*****************************************************************************
* @brief        set ctrl mapping
* 
* @param[in]    rl          readline instance
* @param[in]    mapidx      map index
* @param[in]    exec        exec code
* 
*****************************************************************************/
void chry_readline_set_ctrlmap(chry_readline_t *rl, uint8_t mapidx, uint8_t exec)
{
    (void)rl;

#if defined(CONFIG_READLINE_CTRLMAP) && CONFIG_READLINE_CTRLMAP
    rl->ctrlmap[mapidx] = exec;
#else
    ctrlmap[mapidx] = exec;
#endif
}

/*****************************************************************************
* @brief        set alt mapping
* 
* @param[in]    rl          readline instance
* @param[in]    mapidx      map index
* @param[in]    exec        exec code
* 
*****************************************************************************/
void chry_readline_set_altmap(chry_readline_t *rl, uint8_t mapidx, uint8_t exec)
{
    (void)rl;

#if defined(CONFIG_READLINE_ALTMAP) && CONFIG_READLINE_ALTMAP
    rl->altmap[mapidx] = exec;
#else
    altmap[mapidx] = exec;
#endif
}

#if defined(CONFIG_READLINE_DEBUG) && CONFIG_READLINE_DEBUG
/*****************************************************************************
* @brief        debug for keycode test
* 
* @param[in]    rl          readline instance
* 
*****************************************************************************/
void chry_readline_debug(chry_readline_t *rl)
{
    chry_readline_put(rl, "debug keycodes mode\r\n", 22, );
    char fifo[5] = { 0 };

    while (1) {
        int8_t len;
        uint8_t c = 0;
        char buf[32];

        /*!< get a key */
        chry_readline_waitkey(rl, &c);

        if (c < CHRY_READLINE_G0_BEG) {
            len = snprintf(buf, 32, "'^%c' %02x (%3d) /exit to break\r\n", c + 0x40, c, c);
            chry_readline_put(rl, buf, len, );
        } else if ((CHRY_READLINE_G0_BEG <= c) && (c <= CHRY_READLINE_G0_END)) {
            /*!< printable characters */
            len = snprintf(buf, 32, " '%c' %02x (%3d) /exit to break\r\n", c, c, c);
            chry_readline_put(rl, buf, len, );
        } else {
            len = snprintf(buf, 32, " '?' %02x (%3d) /exit to break\r\n", c, c);
            chry_readline_put(rl, buf, len, );
        }

        fifo[4] = fifo[3];
        fifo[3] = fifo[2];
        fifo[2] = fifo[1];
        fifo[1] = fifo[0];
        fifo[0] = c;

        if (memcmp(fifo, "tixe/", 5) == 0) {
            break;
        }
    }
}
#endif

/*****************************************************************************
* @brief        edit the specified segment of the prompt,
*               segment will be create at first edit
* 
* @param[in]    rl          readline instance
* @param[in]    secidx      segment index, < PPTSEG
* @param[in]    sgrraw      sgr sequense raw data
* @param[in]    format      string end with \0
* @param[in]    ...         params
* 
* @retval int               0:Success -1:Error -2:No space
*****************************************************************************/
int chry_readline_prompt_edit(chry_readline_t *rl, uint8_t segidx, uint16_t sgrraw, const char *format, ...)
{
#if defined(CONFIG_READLINE_PROMPTEDIT) && CONFIG_READLINE_PROMPTEDIT
    va_list ap;
    char sgrbuf[17];
    uint16_t pptlen;
    int freelen;
    int seglen;
    uint16_t segoff = 0;
    uint8_t sgrlen;

    if (segidx >= CONFIG_READLINE_PROMPTSEG) {
        return -1;
    }

    /*!< not allowed any free seg exists before this seg */
    for (uint8_t i = 0; i < segidx; i++) {
        segoff += rl->pptseglen[i];
        if (rl->pptseglen[i] == 0) {
            return -1;
        }
    }

    /*!< calculate total prompt len */
    pptlen = 0;
    for (uint8_t i = 0; i < CONFIG_READLINE_PROMPTSEG; i++) {
        if (i == segidx) {
            continue;
        }
        pptlen += rl->pptseglen[i];
    }

    /*!< freelen reserved 1 for \0, 3 for \e[m */
    freelen = rl->ln.pptsize - 1 - 3 - pptlen;

    /*!< output sgr sequence and get len */
    sgrlen = chry_readline_sgrset(sgrbuf, sgrraw);

    if (freelen < (int)sgrlen) {
        return -2;
    }

    /*!< if next segment is not used */
    if (rl->pptseglen[segidx + 1] == 0) {
        /*!< copy sgrbuff to prompt */
        memcpy(&(rl->prompt[segoff]), sgrbuf, sgrlen);
        freelen -= sgrlen;

        /*!< output format to prompt */
        va_start(ap, format);
        seglen = vsnprintf(&(rl->prompt[segoff + sgrlen]), freelen + 1, format, ap);
        va_end(ap);

        if (seglen > freelen) {
            /*!< set segment len */
            rl->pptseglen[segidx] = sgrlen + freelen;

            /*!< end with \e0m\0 */
            memcpy(&(rl->prompt[segoff + sgrlen + freelen]), "\033[m", 5);
            return -2;
        } else {
            /*!< set segment len */
            rl->pptseglen[segidx] = sgrlen + seglen;

            /*!< end with \e0m\0 */
            memcpy(&(rl->prompt[segoff + sgrlen + seglen]), "\033[m", 5);

            return 0;
        }
    }

    /*!< get the seg len */
    va_start(ap, format);
    seglen = vsnprintf(NULL, 0, format, ap);
    va_end(ap);

    int actlen = sgrlen + seglen;
    actlen = freelen > actlen ? actlen : freelen;

    memmove(
        &(rl->prompt[segoff + actlen]),
        &(rl->prompt[segoff + rl->pptseglen[segidx]]),
        pptlen - segoff);

    /*!< copy sgrbuff to prompt */
    memcpy(&(rl->prompt[segoff]), sgrbuf, sgrlen);
    freelen -= sgrlen;

    /*!< backup vsnprintf overwrite char */
    char bakc = rl->prompt[segoff + actlen];

    /*!< output format to prompt */
    va_start(ap, format);
    seglen = vsnprintf(&(rl->prompt[segoff + sgrlen]), freelen + 1, format, ap);
    va_end(ap);

    rl->prompt[segoff + actlen] = bakc;

    /*!< set segment len */
    rl->pptseglen[segidx] = actlen;

    /*!< end with \e0m\0 */
    memcpy(&(rl->prompt[pptlen + actlen]), "\033[m", 5);

    if (seglen > freelen) {
        return -2;
    } else {
        return 0;
    }
#else
    (void)rl;
    (void)segidx;
    (void)sgrraw;
    (void)format;
    return 0;
#endif
}

/*****************************************************************************
* @brief        clear all seglen, next edit is first
* 
* @param[in]    rl          readline instance
* 
*****************************************************************************/
void chry_readline_prompt_clear(chry_readline_t *rl)
{
#if defined(CONFIG_READLINE_PROMPTEDIT) && CONFIG_READLINE_PROMPTEDIT
    memset(rl->pptseglen, 0, CONFIG_READLINE_PROMPTSEG + 1);
#else
    (void)rl;
#endif
}
