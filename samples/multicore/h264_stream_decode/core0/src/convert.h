#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "mp4.h"
#include <stdio.h>

void yuv420p_to_yuyv(unsigned char** pFrame, unsigned char *yuv422, int width, int height);

#endif