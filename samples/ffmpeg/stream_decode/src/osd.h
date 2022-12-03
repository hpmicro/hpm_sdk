#ifndef _OSD_H_
#define _OSD_H_

#include "mp4.h"
#include "frame.h"
#include <stdio.h>

#include "board.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_pdma_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"

extern const unsigned char nAsciiDot24x48[];
bool lcd_config_layer2(void);
void test_blit(void);
void test_scale(void);
#endif