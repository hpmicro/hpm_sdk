/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Usage in JavaScript:
 *   var led = new HPM_Pin("PA02");  // GPIO pin PA02
 *   led.mode = PinMode.Output;       // Set as output (using PinMode enum)
 *   led.level = PinValue.High;      // Set high
 *   led.level = PinValue.Low;       // Set low
 *   var val = led.level;            // Read level (returns 0 or 1)
 *   led.toggle();                    // Toggle pin
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "mquickjs_build.h"
#include "mquickjs.h"
#include "hpm_gpio.h"

/*
 * PinMode object - global enum for GPIO modes
 * Usage: PinMode.Input, PinMode.Output, etc.
 */
static const JSPropDef js_pinmode[] = {
    JS_PROP_STRING_DEF("Input", "INPUT", 0),
    JS_PROP_STRING_DEF("Output", "OUTPUT", 0),
    JS_PROP_STRING_DEF("Input_Pullup", "INPUT_PULLUP", 0),
    JS_PROP_STRING_DEF("Input_Pulldown", "INPUT_PULLDOWN", 0),
    JS_PROP_STRING_DEF("Output_OD", "OUTPUT_OD", 0),
    JS_PROP_END,
};

static const JSClassDef js_pinmode_obj = JS_OBJECT_DEF("PinMode", js_pinmode);

/*
 * PinValue object - global enum for GPIO level values
 * Usage: PinValue.High, PinValue.Low
 */
static const JSPropDef js_pinvalue[] = {
    JS_PROP_STRING_DEF("High", "HIGH", 0),
    JS_PROP_STRING_DEF("Low", "LOW", 0),
    JS_PROP_END,
};

static const JSClassDef js_pinvalue_obj = JS_OBJECT_DEF("PinValue", js_pinvalue);

/*
 * GPIO prototype properties - instance methods and getters/setters
 */
static const JSPropDef js_hpm_gpio_proto[] = {
    JS_CGETSET_DEF("level", js_hpm_gpio_pin_get_level, js_hpm_gpio_pin_set_level),
    JS_CFUNC_DEF("toggle", 0, js_hpm_gpio_pin_toggle),
    JS_CGETSET_DEF("mode", js_hpm_gpio_pin_get_mode, js_hpm_gpio_pin_set_mode),
    JS_PROP_END,
};

/*
 * GPIO class properties - static methods/constants
 */
static const JSPropDef js_hpm_gpio[] = {
    JS_PROP_END,
};

/*
 * GPIO class definition
 */
static const JSClassDef js_hpm_gpio_class =
    JS_CLASS_DEF("HPM_Pin", 1, js_hpm_gpio_pin_constructor, JS_CLASS_HPM_GPIO,
                 js_hpm_gpio, js_hpm_gpio_proto, NULL, js_hpm_gpio_pin_finalizer);

