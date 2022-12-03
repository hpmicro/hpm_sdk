#include "frame.h"
#include "convert.h"

static void cvt_420p_to_rgb565(int width, int height, const unsigned char *src, unsigned short *dst)
{
    int line, col, linewidth;
    int y, u, v, yy, vr, ug, vg, ub;
    int r, g, b;
    const unsigned char *py, *pu, *pv;

    linewidth = width >> 1;
    py = src;
    pu = py + (width * height);
    pv = pu + (width * height) / 4;

    y = *py++;
    yy = y << 8;
    u = *pu - 128;
    ug = 88 * u;
    ub = 454 * u;
    v = *pv - 128;
    vg = 183 * v;
    vr = 359 * v;

    for (line = 0; line < height; line++) {
        for (col = 0; col < width; col++) {
            r = (yy + vr) >> 8;
            g = (yy - ug - vg) >> 8;
            b = (yy + ub) >> 8;

            if (r < 0)
                r = 0;
            if (r > 255)
                r = 255;
            if (g < 0)
                g = 0;
            if (g > 255)
                g = 255;
            if (b < 0)
                b = 0;
            if (b > 255)
                b = 255;
            *dst++ = (((unsigned short)r>>3)<<11) | (((unsigned short)g>>2)<<5) | (((unsigned short)b>>3)<<0);

            y = *py++;
            yy = y << 8;
            if (col & 1) {
                pu++;
                pv++;

                u = *pu - 128;
                ug = 88 * u;
                ub = 454 * u;
                v = *pv - 128;
                vg = 183 * v;
                vr = 359 * v;
            }
        }
        if ((line & 1) == 0) {
            pu -= linewidth;
            pv -= linewidth;
        }
    }
}

void AVFrame2Img(AVFrame *pFrame, void *buf)
{
    int frameHeight = pFrame->height;
    int frameWidth = pFrame->width;
    int channels = 3;

    /* 创建保存yuv数据的buffer */
    unsigned char *pDecodedBuffer = (unsigned char *)malloc(frameHeight * frameWidth * sizeof(unsigned char) * channels);

    /* 从AVFrame中获取yuv420p数据，并保存到buffer */
    int i, j, k;
    /* 拷贝y分量 */
    for (i = 0; i < frameHeight; i++) {
        memcpy(pDecodedBuffer + frameWidth * i,
               pFrame->data[0] + pFrame->linesize[0] * i,
               frameWidth);
    }
    /* 拷贝u分量 */
    for (j = 0; j < frameHeight / 2; j++) {
        memcpy(pDecodedBuffer + frameWidth * i + frameWidth / 2 * j,
               pFrame->data[1] + pFrame->linesize[1] * j,
               frameWidth / 2);
    }
    /* 拷贝v分量 */
    for (k = 0; k < frameHeight / 2; k++) {
        memcpy(pDecodedBuffer + frameWidth*i + frameWidth / 2 * j + frameWidth / 2 * k,
               pFrame->data[2] + pFrame->linesize[2] * k,
               frameWidth / 2);
    }

    /* 将buffer中的yuv420p数据转换为RGB; */
    cvt_420p_to_rgb565(frameWidth, frameHeight, pDecodedBuffer, buf);
    free(pDecodedBuffer);
}