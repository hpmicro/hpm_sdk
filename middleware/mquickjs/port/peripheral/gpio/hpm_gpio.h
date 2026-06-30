/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_GPIO_JS_H
#define HPM_GPIO_JS_H

#include "mquickjs.h"

/* HPM GPIO class ID - must match hpm_stdlib.h */
#define JS_CLASS_HPM_GPIO       (JS_CLASS_USER + 0)
#define JS_CLASS_COUNT          (JS_CLASS_USER + 1)

/* GPIO class functions - implemented in hpm_gpio.c */

/* Constructor: new HPM_Pin(port, pin) */
JSValue js_hpm_gpio_pin_constructor(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/* Finalizer */
void js_hpm_gpio_pin_finalizer(JSContext *ctx, void *opaque);

/* Getter: pin.level */
JSValue js_hpm_gpio_pin_get_level(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/* Setter: pin.level = value */
JSValue js_hpm_gpio_pin_set_level(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/* Method: pin.toggle() */
JSValue js_hpm_gpio_pin_toggle(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/* Getter: pin.mode */
JSValue js_hpm_gpio_pin_get_mode(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);

/* Setter: pin.mode = value */
JSValue js_hpm_gpio_pin_set_mode(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv);


#endif /* HPM_GPIO_JS_H */
