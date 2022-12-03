#include "osd.h"
#include "frame.h"

#ifndef BLIT_RESULT_VERIFICATION
#define BLIT_RESULT_VERIFICATION 0
#endif

#define LCD BOARD_LCD_BASE
#define LCD_IRQ BOARD_LCD_IRQ
#define LCD_WIDTH BOARD_LCD_WIDTH
#define LCD_HEIGHT BOARD_LCD_HEIGHT
#define PDMA BOARD_PDMA_BASE
#define RUNNING_CORE HPM_CORE0

#define PIXEL_FORMAT display_pixel_format_argb8888
#define COLOR_SIZE 32

#define TEST_LAYER_COUNT 3
#define TEST_IMAGE_COUNT 2
#define LAYER_WIDTH 800
#define LAYER_HEIGHT 62
extern const unsigned char aaa[];
typedef uint32_t color_t;

color_t __attribute__((section(".framebuffer"))) buffer[TEST_LAYER_COUNT][LAYER_WIDTH * LAYER_HEIGHT];
typedef struct {
    uint32_t u;
    display_pixel_format_t format;
} color;

color c[TEST_LAYER_COUNT];

struct {
    lcdc_layer_config_t layer;
    bool left_to_right;
    bool up_to_down;
} layer_info[TEST_LAYER_COUNT];

void writefount2screen(uint16_t or_x, uint16_t or_y, uint16_t x_end, uint16_t y_end, uint8_t assic_id, uint16_t colour,
    uint8_t clearflag, uint8_t *str_fount, uint32_t screen_addr, uint16_t fount_size)
{
    uint8_t *strdisp;
    uint16_t x, y;
    uint8_t bit;
    uint8_t temp1;
    strdisp = (uint8_t *)screen_addr;
    str_fount += fount_size * (assic_id - 0x20); /*get end encode*/
    bit = 0;
    for (y = or_y; y <= y_end; y++) {
        for (x = or_x; x <= x_end; x++) {
            if (clearflag == true) {
                *(strdisp + y * (LCD_WIDTH * 2) + 2 * x) = colour & 0x00ff;
                *(strdisp + y * (LCD_WIDTH * 2) + 2 * x + 1) = colour >> 8;
            } else {
                temp1 = (*str_fount) >> bit;
                if ((temp1 & 0x01) == 0x01) {
                    *(strdisp + y * (LCD_WIDTH * 2) + 2 * x) = colour & 0x00ff;
                    *(strdisp + y * (LCD_WIDTH * 2) + 2 * x + 1) = colour >> 8;
                }
                bit++;
                if (bit == 8) {
                    bit = 0;
                    str_fount += 1;
                }
            }
        }
    }
}

char string2fount(uint16_t line, uint16_t column, uint8_t *string, uint8_t string_num, uint16_t colour,
    uint8_t *str_fount, uint32_t screen_addr, uint8_t fount_width, uint8_t fount_height)
{
    uint8_t i = 0, j = 0, numtemp = 0;
    uint16_t or_x, or_y, x_end, y_end;
    uint16_t fount_stroage_size;
    or_x = column * fount_width;
    or_y = line * fount_height;
    x_end = or_x + fount_width - 1;
    y_end = or_y + fount_height - 1;
    fount_stroage_size = fount_width * fount_height / 8;
    for (numtemp = 0; numtemp < string_num; numtemp++) {
        if ((*(string + numtemp) != 10) && (*(string + numtemp) != 0)) { /*enter or end*/
            if (*(string + numtemp) != 8) {                              /*delete*/
                writefount2screen(or_x + fount_width * i, or_y + fount_height * j, x_end + fount_width * i, y_end + fount_height * j,
                    *(string + numtemp), colour, false, str_fount, screen_addr, fount_stroage_size);
            } else {
                writefount2screen(or_x + fount_width * i, or_y + fount_height * j, x_end + fount_width * i, y_end + fount_height * j,
                    *(string + numtemp), colour, true, str_fount, screen_addr, fount_stroage_size);
            }
        } else if (*(string + numtemp) == 10) {
            i = 19; /* jump next line */
        } else if (*(string + numtemp) == 0) {
            return true;
        }
        i++;
        if (i * fount_width == LCD_WIDTH) {
            j++;
            i = 0;
        }
    }
    return true;
}

uint32_t get_rgb_color(color *c)
{
    switch (c->format) {
    case display_pixel_format_rgb565:
        return ((((c->u & 0xFF0000) >> 16) * 0x1F / 0xFF) << 11) | (((((c->u & 0xFF00)) >> 8) * 0x3F / 0xFF) << 6) | (((c->u & 0xFF) * 0x1F / 0xFF) << 0);
    case display_pixel_format_argb8888:
        return c->u;
    default:
        return 0;
    }
}

void flip_rotate(color_t *dst, uint32_t dst_width,
    const color_t *src, uint32_t src_width, uint32_t x, uint32_t y,
    uint32_t width, uint32_t height,
    pdma_flip_t flip, pdma_rotate_t rotate, uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_flip_rotate(PDMA,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)dst),
        dst_width,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)src),
        src_width, x, y,
        width, height, flip, rotate, alpha, PIXEL_FORMAT, true, &status);

    if (stat != status_success) {
        printf("pdma flip rotate failed: 0x%x\n", status);
    }
}

void blit(color_t *dst, uint32_t dst_width,
    const color_t *src, uint32_t src_width,
    uint32_t x, uint32_t y,
    uint32_t width, uint32_t height, uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_blit(PDMA,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)dst),
        dst_width,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)src),
        src_width,
        x, y, width, height, alpha, PIXEL_FORMAT, true, &status);

    if (stat != status_success) {
        printf("pdma blit failed: 0x%x\n", status);
    }

#if BLIT_RESULT_VERIFICATION == 1
    uint32_t i, j, source, *p_dest;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            p_dest = (uint32_t *)((uint32_t)dst + ((i + y) * dst_width + x + j) * display_get_pixel_size_in_byte(PIXEL_FORMAT));
            source = *(uint32_t *)((uint32_t)src + (i * width + j) * display_get_pixel_size_in_byte(PIXEL_FORMAT));
            if (((*p_dest) & 0xFFFFFF) != (source & 0xFFFFFF)) {
                printf("!color mismatch: src: 0x%x, 0x%x; dst: 0x%x, 0x%x\n",
                    (uint32_t)src + (i * width + j) * display_get_pixel_size_in_byte(PIXEL_FORMAT), source,
                    p_dest, *p_dest);
            }
        }
    }
#endif
}

void fill_area(color_t *buf, uint32_t width, uint32_t height, color *c, uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)buf);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)buf + width * height * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_fill_color(PDMA,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)buf), width,
        width, height,
        c->u, alpha, c->format, true, &status);

    if (stat != status_success) {
        printf("pdma fill color failed: 0x%x\n", status);
    }
}

void scale(color_t *dst, uint32_t dst_width,
    const color_t *src, uint32_t src_width,
    uint32_t x, uint32_t y,
    uint32_t width, uint32_t height,
    uint32_t target_width, uint32_t target_height,
    uint32_t alpha)
{
    hpm_stat_t stat;
    uint32_t status;

    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)src);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)src + src_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);

        aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)dst);
        aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)dst + dst_width * (y + height) * display_get_pixel_size_in_byte(c->format));
        aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    stat = pdma_scale(PDMA,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)dst),
        dst_width,
        (uint32_t)core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)src),
        src_width,
        x, y, width, height,
        target_width, target_height,
        alpha, PIXEL_FORMAT, false, &status);

    if (stat != status_success) {
        printf("pdma scale failed: 0x%x\n", status);
    }
}

void generate_color_table(void)
{
    uint8_t i;
    for (i = 1; i < TEST_LAYER_COUNT; i++) {
        c[i].format = PIXEL_FORMAT;
        c[i].u = 0xFF000000 | ((rand() % 0xFF + i) << 16) | ((rand() % 0xFF + i) << 8) | ((rand() % 0xFF + i));
    }
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)&c);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)&c + sizeof(c));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
}

void move_layer(uint8_t index)
{
    lcdc_layer_config_t *layer = &layer_info[index].layer;

    if (layer_info[index].left_to_right) {
        layer->position_x++;
    } else {
        layer->position_x--;
    }

    if (layer_info[index].up_to_down) {
        layer->position_y++;
    } else {
        layer->position_y--;
    }
    if ((layer->position_x >= (LCD_WIDTH - layer->width)) || (layer->position_x == 0)) {
        layer_info[index].left_to_right ^= true;
        layer->position_x = layer->position_x ? LCD_WIDTH - layer->width : 0;
    }
    if ((layer->position_y == 0) || ((layer->position_y >= LCD_HEIGHT - layer->height))) {
        layer_info[index].up_to_down ^= true;
        layer->position_y = layer->position_y ? LCD_HEIGHT - layer->height : 0;
    }

    lcdc_config_layer(LCD, index, layer, true);
}

void test_blit(void)
{
    static uint8_t index = 0;
    index++;

    fill_area(buffer[1], LAYER_WIDTH, LAYER_HEIGHT, &c[1], 0xFF);
    blit(buffer[1], LAYER_WIDTH,                            /* dest buffer, dest buffer width */
        (color_t *)&aaa[0], LAYER_WIDTH,                    /* source buffer, source buffer width */
        0, 0,                                               /* x, y: coordinates in the dest area */
        LAYER_WIDTH, LAYER_HEIGHT / (index % 2 + 1), 0xFF); /* width, height, alpha */
}

void test_scale(void)
{
    static uint8_t index = 0;
    index++;

    pdma_stop(PDMA);
    scale(buffer[1], LAYER_WIDTH,        /* dest buffer, dest buffer width  */
        (color_t *)&aaa[0], LAYER_WIDTH, /* source buffer, source buffer width */
        0, 0,                            /* x, y: coordinates in the dest area */
        LAYER_WIDTH, 62,                 /* width, height */
        LAYER_WIDTH, 62,                 /* target_width, target_height */
        0xFF);
}

bool lcd_config_layer2(void)
{
    uint32_t i = 0;

    lcdc_config_t config = {0};
    lcdc_layer_config_t *layer;
    lcdc_get_default_config(LCD, &config);
    board_panel_para_to_lcdc(&config);
    i = 1;
    c[1].format = PIXEL_FORMAT;
    c[1].u = 0xffffff00;
    layer = &layer_info[i].layer;
    lcdc_get_default_layer_config(LCD, layer, PIXEL_FORMAT, i);
    fill_area(buffer[i], LAYER_WIDTH, LAYER_HEIGHT, &c[i], 0xFF);

    layer->position_x = 0;
    layer->position_y = 0;
    layer->width = LAYER_WIDTH;
    layer->height = LAYER_HEIGHT;
    layer->buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)buffer[i]);

    layer->alphablend.src_alpha = 0x00; /* src */
    layer->alphablend.dst_alpha = 0xFF; /* dst */
    layer->alphablend.src_alpha_op = display_alpha_op_invalid;
    layer->alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer->background.u = 0;
    layer->alphablend.mode = display_alphablend_mode_src_over;

    if (status_success != lcdc_config_layer(LCD, i, layer, true)) {
        printf("failed to configure layer %d\n", i);
        return false;
    }

    i = 2;
    c[2].format = PIXEL_FORMAT;
    c[2].u = 0xffffff00;
    layer = &layer_info[i].layer;
    lcdc_get_default_layer_config(LCD, layer, PIXEL_FORMAT, i);
    fill_area(buffer[i], LAYER_WIDTH, LAYER_HEIGHT, &c[i], 0xFF);

    layer->position_x = 0;
    layer->position_y = LCD_HEIGHT - LAYER_HEIGHT;
    layer->width = LAYER_WIDTH;
    layer->height = LAYER_HEIGHT;
    layer->buffer = core_local_mem_to_sys_address(RUNNING_CORE, (uint32_t)buffer[i]);

    layer->alphablend.src_alpha = 0xff; /* src */
    layer->alphablend.dst_alpha = 0xFF; /* dst */
    layer->alphablend.src_alpha_op = display_alpha_op_invalid;
    layer->alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer->background.u = 0;
    layer->alphablend.mode = display_alphablend_mode_src_over;

    if (status_success != lcdc_config_layer(LCD, i, layer, true)) {
        printf("failed to configure layer %d\n", i);
        return false;
    }

    int alpha_count = 0;

    string2fount(1, 1, "123ABC", sizeof("123ABC"), 0xffff, (uint8_t *)nAsciiDot24x48, (uint32_t)buffer[2], 24, 48);

    blit(buffer[1], LAYER_WIDTH, /* dest buffer, dest buffer width */
        (color_t *)aaa, 800,     /* source buffer, source buffer width */
        0, 0,                    /* x, y: coordinates in the dest area */
        LAYER_WIDTH, 62, 0xFF);  /* width, height, alpha */

    return true;
}