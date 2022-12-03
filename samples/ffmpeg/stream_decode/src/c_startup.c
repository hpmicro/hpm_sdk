
#include <stdint.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"
#include "hpm_interrupt.h"


extern void system_init(void);


void c_startup(void)
{
    uint32_t i, size;
    /* vectors */
    extern volatile uint8_t __vector_ram_start__[], __vector_ram_end__[], __vector_load_addr__[];
    size = __vector_ram_end__ - __vector_ram_start__;
    for (i = 0; i < size; i++) {
        *(__vector_ram_start__ + i) = *(__vector_load_addr__ + i);
    }


    /* ilm section */

    extern volatile uint8_t __ilm_start__[], __ilm_end__[], __ilm0_load_addr__[];
    size = __ilm_end__ - __ilm_start__;
    for (i = 0; i < size; i++) {
        *(__ilm_start__ + i) = *(__ilm0_load_addr__ + i);
    }

    /* text section */

    extern volatile uint8_t __text_start__[], __text_end__[], __text_load_addr__[];
    size = __text_end__ - __text_start__;
    for (i = 0; i < size; i++) {
        *(__text_start__ + i) = *(__text_load_addr__ + i);
    }

    extern volatile uint8_t __etext[];
    extern volatile uint8_t __bss_start__[], __bss_end__[];
    extern volatile uint8_t __data_start__[], __data_end__[];
    extern volatile uint8_t __noncacheable_bss_start__[], __noncacheable_bss_end__[];
    extern volatile uint8_t __ramfunc_start__[], __ramfunc_end__[];
    extern volatile uint8_t __noncacheable_init_start__[], __noncacheable_init_end__[];

    /* bss section */
    size = __bss_end__ - __bss_start__;
    for (i = 0; i < size; i++) {
        *(__bss_start__ + i) = 0;
    }

    /* noncacheable bss section */
    size = __noncacheable_bss_end__ - __noncacheable_bss_start__;
    for (i = 0; i < size; i++) {
        *(__noncacheable_bss_start__ + i) = 0;
    }

    /* data section LMA: etext */
    size = __data_end__ - __data_start__;
    for (i = 0; i < size; i++) {
        *(__data_start__ + i) = *(__etext + i);
    }

    /* ramfunc section LMA: etext + data length */
    size = __ramfunc_end__ - __ramfunc_start__;
    for (i = 0; i < size; i++) {
        *(__ramfunc_start__ + i) = *(__etext + (__data_end__ - __data_start__) + i);
    }

    /* noncacheable init section LMA: etext + data length + ramfunc legnth */
    size = __noncacheable_init_end__ - __noncacheable_init_start__;
    for (i = 0; i < size; i++) {
        *(__noncacheable_init_start__ + i) = *(__etext + (__data_end__ - __data_start__) + (__ramfunc_end__ - __ramfunc_start__) + i);
    }



}
