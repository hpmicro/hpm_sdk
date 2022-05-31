/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/

/*********************
 *      INCLUDES
 *********************/
#include "board.h"
#include "hpm_touch.h"
#include "lv_port_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t *x, lv_coord_t *y);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t * indev_touchpad;
static touch_point_t touch_points[HPM_TOUCH_MAX_POINTS];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /* Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    hpm_stat_t stat;
    stat = touch_init(BOARD_CAP_I2C_BASE);
    if (stat != status_success) {
        while(1);
    }
}

/* Will be called by the library to read the touchpad */
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    data->point.x = last_x;
    data->point.y = last_y;
}

static void touchpad_get_xy(lv_coord_t *x, lv_coord_t *y)
{
#if CONFIG_TOUCH_GT911 == 1
    (*x) = touch_points[0].x;
    (*y) = touch_points[0].y;
#elif CONFIG_TOUCH_FT5406 == 1
    (*x) = touch_points[0].y;
    (*y) = touch_points[0].x;
#else
    #error unsupported touch
#endif
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    hpm_stat_t stat;
    uint8_t num_of_points;
    stat = touch_get_data(touch_points, &num_of_points);
    if (stat != status_success) {
        return false;
    }
    if ((num_of_points < HPM_TOUCH_MAX_POINTS) && num_of_points) {
        return true;
    }
    return false;
}
