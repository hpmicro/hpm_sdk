
#include <stdint.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"
#include "hpm_interrupt.h"


extern void system_init(void);


void c_startup(void)
{
    uint32_t i, size;

    extern volatile uint8_t __etext[];
    extern volatile uint8_t __bss_start__[], __bss_end__[];
    extern volatile uint8_t __data_start__[], __data_end__[];
    extern volatile uint8_t __noncacheable_bss_start__[], __noncacheable_bss_end__[];

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

}
