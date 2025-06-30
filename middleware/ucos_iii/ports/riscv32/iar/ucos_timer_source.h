#ifndef _UCOS_TIMER_SOURCE_H_
#define _UCOS_TIMER_SOURCE_H_

#ifndef CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR

#include "hpm_mchtmr_drv.h"

#else /* CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR */

#include "hpm_gptmr_drv.h"
#include "hpm_clock_drv.h"

#ifndef UCOS_TIMER_RESOURCE
    #define UCOS_TIMER_RESOURCE      BOARD_UCOS_TIMER
#endif

#ifndef UCOS_TIMER_CH
    #define UCOS_TIMER_CH            BOARD_UCOS_TIMER_CHANNEL
#endif

#ifndef UCOS_TIMER_IRQ
    #define UCOS_TIMER_IRQ           BOARD_UCOS_TIMER_IRQ
#endif

#ifndef UCOS_TIMER_CLOCK
    #define UCOS_TIMER_CLOCK         BOARD_UCOS_TIMER_CLK_NAME
#endif

#endif /* End of CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR */

#endif