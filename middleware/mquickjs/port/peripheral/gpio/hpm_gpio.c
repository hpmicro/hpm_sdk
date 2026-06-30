/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include "hpm_common.h"
#include "hpm_gpio_drv.h"
#include "hpm_gpiom_drv.h"
#include "hpm_gpio.h"

/* Helper: compare two JSValue for equality (simple implementation) */
static bool js_value_equals(JSValue a, JSValue b)
{
    /* For this mquickjs version, JSValue is essentially a tagged pointer/int.
     * Two string objects from JS_PROP_STRING_DEF are constants, so we can
     * compare them by value (address equality for pointers).
     */
    return a == b;
}

#define INVALID_PIN_NUMBER    0xFFFFFFFFUL
#define GPIO_MODE_NAME_SIZE   25
/* GPIO pin data structure */
typedef struct {
    uint32_t pin_number;
    char mode_name[GPIO_MODE_NAME_SIZE];
} HPM_GPIO_Data;

/**
 * @brief Get Pin index from name string
 *
 * This function converts a pin name string to a pin index number.
 * The pin naming convention follows the pattern: P<PORT><INDEX>
 *
 * Supported formats:
 * - PA00 to PF99: GPIO ports A-F with pin indices 00-99
 * - PV00 to PZ99: GPIO ports V-Z with pin indices 00-99
 *
 * Examples:
 * - "PA00" -> pin index 0 (GPIO A, pin 0)
 * - "PZ03" -> pin index 355 (GPIO Z, pin 3)
 * - "PB15" -> pin index 47 (GPIO B, pin 15)
 *
 * @param [in] name Pin name string (must be exactly 4 characters)
 * @return Pin index number, or -RT_EINVAL if name is invalid
 */
static uint32_t hpm_pin_get(const char *name)
{
    /* Validate pin name format: P<PORT><INDEX> (4 characters total) */
    if (!(  (strlen(name) == 4) &&                    /* Exactly 4 characters */
            (name[0] == 'P') &&                          /* First character is 'P' */
            ((('A' <= name[1]) && (name[1] <= 'F')) ||   /* Port A-F or V-Z */
             (('V' <= name[1]) && (name[1] <= 'Z'))) &&
            (('0' <= name[2]) && (name[2] <= '9')) &&    /* Third character is digit */
            (('0' <= name[3]) && (name[3] <= '9'))       /* Fourth character is digit */
        ))
    {
        return INVALID_PIN_NUMBER;
    }
    /* Calculate GPIO port index */
    uint32_t gpio_idx = (name[1] <= 'F') ? (name[1] - 'A') : (11 + name[1] - 'V');
    /* Calculate pin index within the port */
    uint32_t pin_idx = (uint32_t)(name[2] - '0') * 10 + (name[3] - '0');
    /* Return combined pin index (gpio_idx * 32 + pin_idx) */
    return (gpio_idx * 32 + pin_idx);
}

static int hpm_pin_mode(uint32_t pin, const char *value, char *name)
{
    /* TODO: Check the validity of the pin value */
    uint32_t gpio_idx = pin >> 5;  /* Extract GPIO port index */
    uint32_t pin_idx = pin & 0x1FU; /* Extract pin index within port */
    /* Configure pin controller to use GPIO0 */
    gpiom_set_pin_controller(HPM_GPIOM, gpio_idx, pin_idx, gpiom_soc_gpio0);
    /* Clear function control register */
    HPM_IOC->PAD[pin].FUNC_CTL = 0;
    /* Handle special cases for specific GPIO ports */
    switch (gpio_idx)
    {
    case GPIO_DI_GPIOY :
        /* GPIOY requires special PIOC configuration */
        HPM_PIOC->PAD[pin].FUNC_CTL = 3;
        break;
#ifdef GPIO_DI_GPIOZ
    case GPIO_DI_GPIOZ :
#ifdef HPM_BIOC
        /* GPIOZ requires special BIOC configuration if available */
        HPM_BIOC->PAD[pin].FUNC_CTL = 3;
#endif
        break;
#endif
    default :
        break;
    }
    /* Configure pin mode and pad settings */
    if (strcmp(value, "INPUT") == 0) {
        /* Configure as input with no pull resistors */
        gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[pin].PAD_CTL &= ~(IOC_PAD_PAD_CTL_PS_MASK | IOC_PAD_PAD_CTL_PE_MASK);
        snprintf(name, GPIO_MODE_NAME_SIZE, "INPUT");
    } else if (strcmp(value, "OUTPUT") == 0) {
        /* Configure as output with push-pull driver */
        gpio_set_pin_output(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[pin].PAD_CTL &=  ~(IOC_PAD_PAD_CTL_PS_MASK | IOC_PAD_PAD_CTL_PE_MASK | IOC_PAD_PAD_CTL_OD_MASK);
        snprintf(name, GPIO_MODE_NAME_SIZE, "OUTPUT");
    } else if (strcmp(value, "INPUT_PULLDOWN") == 0) {
        /* Configure as input with pull-down resistor */
        gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[pin].PAD_CTL = (HPM_IOC->PAD[pin].PAD_CTL & ~IOC_PAD_PAD_CTL_PS_MASK) | IOC_PAD_PAD_CTL_PE_SET(1);
        snprintf(name, GPIO_MODE_NAME_SIZE, "INPUT_PULLDOWN");
    } else if (strcmp(value, "INPUT_PULLUP") == 0) {
        /* Configure as input with pull-up resistor */
        gpio_set_pin_input(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[pin].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        snprintf(name, GPIO_MODE_NAME_SIZE, "INPUT_PULLUP");
    } else if (strcmp(value, "OUTPUT_OD") == 0) {
        /* Configure as output with open-drain driver */
        gpio_set_pin_output(HPM_GPIO0, gpio_idx, pin_idx);
        HPM_IOC->PAD[pin].PAD_CTL = (HPM_IOC->PAD[pin].PAD_CTL & ~(IOC_PAD_PAD_CTL_PS_MASK | IOC_PAD_PAD_CTL_PE_MASK)) | IOC_PAD_PAD_CTL_OD_SET(1);
        snprintf(name, GPIO_MODE_NAME_SIZE, "OUTPUT_OD");
    } else {
        /* Invalid mode - no action taken */
        return -1;
    }
    /* Enable pad loopback feature */
    HPM_IOC->PAD[pin].FUNC_CTL = IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
    return 0;
}

/* Constructor: new HPM_Pin(port, pin) */
JSValue js_hpm_gpio_pin_constructor(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)this_val;
    uint32_t pin;
    HPM_GPIO_Data *d;
    JSValue obj;

    if (!(argc & FRAME_CF_CTOR)) {
        return JS_ThrowTypeError(ctx, "must be called with new");
    }
    argc &= ~FRAME_CF_CTOR;

    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "Pin-name must be not empty");
    }

    JSCStringBuf buf;
    const char *str;
    size_t len;
    str = JS_ToCStringLen(ctx, &len, argv[0], &buf);

    pin = hpm_pin_get(str);
    if (pin == INVALID_PIN_NUMBER) {
        return JS_ThrowTypeError(ctx, "Invalid Pin-name");
    }

    obj = JS_NewObjectClassUser(ctx, JS_CLASS_HPM_GPIO);

    d = malloc(sizeof(*d));
    if (d == NULL) {
        return JS_ThrowOutOfMemory(ctx);
    }
    d->pin_number = pin;

    JS_SetOpaque(ctx, obj, d);

    return obj;
}

/* Finalizer */
void js_hpm_gpio_pin_finalizer(JSContext *ctx, void *opaque)
{
    (void)ctx;
    HPM_GPIO_Data *d = opaque;
    if (d) {
        free(d);
    }
}

/* Getter: pin.level */
JSValue js_hpm_gpio_pin_get_level(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)argc;
    (void)argv;
    HPM_GPIO_Data *d;
    int class_id = JS_GetClassID(ctx, *this_val);
    if (class_id != JS_CLASS_HPM_GPIO) {
        return JS_ThrowTypeError(ctx, "expecting HPM_GPIO class");
    }

    d = JS_GetOpaque(ctx, *this_val);
    uint32_t gpio_idx = d->pin_number >> 5;  /* Extract GPIO port index */
    uint32_t pin_idx = d->pin_number & 0x1FU; /* Extract pin index within port */
    uint32_t level = gpio_read_pin(HPM_GPIO0, gpio_idx, pin_idx);

    return JS_NewInt32(ctx, (int)level);
}

/* Setter: pin.level = PinValue.High */
JSValue js_hpm_gpio_pin_set_level(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    HPM_GPIO_Data *d;
    int val = 0;
    int class_id = JS_GetClassID(ctx, *this_val);

    if (class_id != JS_CLASS_HPM_GPIO) {
        return JS_ThrowTypeError(ctx, "expecting HPM_GPIO class");
    }

    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "level value required");
    }

    d = JS_GetOpaque(ctx, *this_val);

    // Check if value is PinValue enum
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue pinvalue_obj = JS_GetPropertyStr(ctx, global_obj, "PinValue");

    if (!JS_IsException(pinvalue_obj) && !JS_IsUndefined(pinvalue_obj) && !JS_IsNull(pinvalue_obj)) {
        // Value might be PinValue.High or PinValue.Low
        JSValue high_val = JS_GetPropertyStr(ctx, pinvalue_obj, "High");
        JSValue low_val = JS_GetPropertyStr(ctx, pinvalue_obj, "Low");
        if (js_value_equals(argv[0], high_val)) {
            val = 1;
        } else if (js_value_equals(argv[0], low_val)) {
            val = 0;
        } else {
            // Not a PinValue, try to convert as number
            if (JS_ToInt32(ctx, &val, argv[0])) {
                return JS_ThrowTypeError(ctx, "Invalid level: must be PinValue enum or 0/1");
            }
        }
    } else {
        // PinValue not defined, try number directly
        if (JS_ToInt32(ctx, &val, argv[0])) {
            return JS_ThrowTypeError(ctx, "Invalid level");
        }
    }

    uint32_t gpio_idx = d->pin_number >> 5;  /* Extract GPIO port index */
    uint32_t pin_idx = d->pin_number & 0x1FU; /* Extract pin index within port */

    gpio_write_pin(HPM_GPIO0, gpio_idx, pin_idx, val ? 1 : 0);

    return JS_NewInt32(ctx, 0);
}

/* Method: pin.toggle() */
JSValue js_hpm_gpio_pin_toggle(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)argc;
    HPM_GPIO_Data *d;
    int val = 0;
    int class_id = JS_GetClassID(ctx, *this_val);

    if (class_id != JS_CLASS_HPM_GPIO)
        return JS_ThrowTypeError(ctx, "expecting HPM_GPIO class");

    d = JS_GetOpaque(ctx, *this_val);

    if (JS_ToInt32(ctx, &val, argv[0])) {
        return JS_ThrowTypeError(ctx, "Invalid pin level");
    }

    uint32_t gpio_idx = d->pin_number >> 5;  /* Extract GPIO port index */
    uint32_t pin_idx = d->pin_number & 0x1FU; /* Extract pin index within port */

    gpio_toggle_pin(HPM_GPIO0, gpio_idx, pin_idx);

    return JS_NewInt32(ctx, 0);
}

JSValue js_hpm_gpio_pin_get_mode(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    (void)argc;
    (void)argv;
    HPM_GPIO_Data *d;
    int class_id = JS_GetClassID(ctx, *this_val);
    if (class_id != JS_CLASS_HPM_GPIO) {
        return JS_ThrowTypeError(ctx, "expecting HPM_GPIO class");
    }

    d = JS_GetOpaque(ctx, *this_val);
    const char *m = d->mode_name;

    return JS_NewString(ctx, m);
}

/* Helper: check if value equals PinMode.Output (or other PinMode enum values) */
static bool is_valid_pinmode_value(JSContext *ctx, JSValue val)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue pinmode_obj = JS_GetPropertyStr(ctx, global_obj, "PinMode");

    if (JS_IsException(pinmode_obj) || JS_IsUndefined(pinmode_obj) || JS_IsNull(pinmode_obj)) {
        // PinMode not defined, accept any string value (backward compatibility)
        return true;
    }

    // Check if val matches any PinMode property
    const char *pinmode_names[] = {"Input", "Output", "Input_Pullup", "Input_Pulldown", "Output_OD"};
    int pinmode_count = 5;

    for (int i = 0; i < pinmode_count; i++) {
        JSValue pinmode_val = JS_GetPropertyStr(ctx, pinmode_obj, pinmode_names[i]);
        if (!JS_IsException(pinmode_val) && js_value_equals(val, pinmode_val)) {
            return true;
        }
    }

    return false;
}

JSValue js_hpm_gpio_pin_set_mode(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
    HPM_GPIO_Data *d;
    int class_id = JS_GetClassID(ctx, *this_val);
    if (class_id != JS_CLASS_HPM_GPIO) {
        return JS_ThrowTypeError(ctx, "expecting HPM_GPIO class");
    }

    if (argc < 1) {
        return JS_ThrowTypeError(ctx, "mode value required");
    }

    d = JS_GetOpaque(ctx, *this_val);

    JSCStringBuf buf;
    const char *str;
    size_t len;
    str = JS_ToCStringLen(ctx, &len, argv[0], &buf);

    if (!is_valid_pinmode_value(ctx, argv[0])) {
        return JS_ThrowTypeError(ctx, "Invalid mode: must use PinMode enum (e.g., PinMode.Output)");
    }

    if (hpm_pin_mode(d->pin_number, str, d->mode_name)) {
        return JS_ThrowTypeError(ctx, "Invalid mode");
    }

    return JS_NewInt32(ctx, 0);
}
