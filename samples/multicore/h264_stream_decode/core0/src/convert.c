#include "convert.h"

void yuv420p_to_yuyv(unsigned char **pFrame, unsigned char *yuv422, int width, int height)

{
    int x, y;
    unsigned char *p420y = pFrame[0];
    unsigned char *p420u = pFrame[1];
    unsigned char *p420v = pFrame[2];
    int tmp = 0;
    int index = 0;
    int index_y = 0, index_u = 0, index_v = 0;
    for (y = 0; y < height; y++) {
        index_u = 0;
        index_v = 0;
        int hang_tmp = y / 2;
        for (x = 0; x < width; x++) {
            yuv422[index] = p420y[index_y];
            index_y++;
            index++;
            if (tmp == 1) {
                tmp = 0;
                int tmp1 = hang_tmp * width / 2 + index_u;
                yuv422[index] = p420v[tmp1];
                index_u++;

            } else {
                tmp = 1;
                int tmp1 = hang_tmp * width / 2 + index_v;
                yuv422[index] = p420u[tmp1];
                index_v++;
            }
            index++;
        }
    }
}