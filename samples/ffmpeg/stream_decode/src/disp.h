#ifndef _DISP_H_
#define _DISP_H_

#include "hpm_lcdc_drv.h"
#define PIX_422     0
#define PIX_420p    1
#define DISPLAY_MODE_422         1
#define DISPLAY_MODE_RGB         0
#define DISPLAY_MODE_YUV
//#define FILE_PRE                 "640-360-"

//#define FILE_PRE                 "800-480-"
#define FILE_PRE

void init_lcd(void);

#endif