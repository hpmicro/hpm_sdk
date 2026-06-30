/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "hpm_debug_console.h"

#ifdef CONFIG_MQUICKJS
#include "cutils.h"
#include "mquickjs.h"
#include "mquickjs_port.h"
#include "peripheral/gpio/hpm_gpio.h"
/* Include the generated stdlib header - must be after mquickjs.h and mquickjs_port.h */
#include "hpm_stdlib.h"

/* JS heap memory - adjust size based on available RAM */
#define JS_HEAP_SIZE (32 * 1024)
static uint8_t js_heap[JS_HEAP_SIZE];

/* REPL buffer */
#define REPL_BUFFER_SIZE 256
static char repl_buffer[REPL_BUFFER_SIZE];
static int repl_index = 0;

/* Log function for mquickjs */
static void js_log_func(void *opaque, const void *buf, size_t buf_len)
{
    (void)opaque;
    for (size_t i = 0; i < buf_len; i++) {
        if (((const char *)buf)[i] == '\n') {
            console_send_byte('\r');
        }
        console_send_byte(((const char *)buf)[i]);
    }
}

/* Run JavaScript code and print result */
static void run_js(JSContext *ctx, const char *code, size_t code_len, const char *filename)
{
    JSValue result;

    if (code_len == 0) {
        code_len = strlen(code);
    }

    result = JS_Eval(ctx, code, code_len, filename, 0);

    if (JS_IsException(result)) {
        JSValue obj;
        obj = JS_GetException(ctx);
        printf("\x1b[31mError: ");
        JS_PrintValueF(ctx, obj, JS_DUMP_LONG);
        printf("\x1b[0m\n");
    }

    /* GC after each evaluation to free memory */
    JS_GC(ctx);
}

/* Run REPL command - prints result for expressions */
static void run_repl(JSContext *ctx, const char *code, size_t code_len)
{
    JSValue result;

    if (code_len == 0) {
        code_len = strlen(code);
    }

    result = JS_Eval(ctx, code, code_len, "<repl>", 0);

    if (JS_IsException(result)) {
        JSValue obj;
        obj = JS_GetException(ctx);
        printf("\x1b[31mError: ");
        JS_PrintValueF(ctx, obj, JS_DUMP_LONG);
        printf("\x1b[0m\n");
    } else if (!JS_IsUndefined(result)) {
        /* Print result for non-undefined values (expressions) */
        JS_PrintValueF(ctx, result, 0);
        printf("\n");
    }

    /* GC after each evaluation to free memory */
    JS_GC(ctx);
}

/* Simple REPL loop */
static void repl_loop(JSContext *ctx)
{
    int c;

    printf("\nmquickjs REPL (type 'exit()' to quit)\n");
    printf("js> ");

    while (1) {
        c = getchar();

        if (c == '\r' || c == '\n') {
            putchar('\n');

            repl_buffer[repl_index] = '\0';

            /* Check for exit command */
            if (strcmp(repl_buffer, "exit()") == 0 || strcmp(repl_buffer, "quit") == 0) {
                printf("Goodbye!\n");
                break;
            }

            /* Execute the code */
            if (repl_index > 0) {
                run_repl(ctx, repl_buffer, 0);
            }

            /* Reset buffer */
            repl_index = 0;
            printf("js> ");
        } else if (c == 0x08 || c == 0x7F) {
            /* Backspace */
            if (repl_index > 0) {
                repl_index--;
                printf("\b \b");
            }
        } else if (c >= ' ' && repl_index < REPL_BUFFER_SIZE - 1) {
            /* Regular character */
            repl_buffer[repl_index++] = (char)c;
            putchar(c);
        }
    }
}

#else
/* Stub functions when mquickjs is disabled */
static void run_js(void *ctx, const char *code, size_t len, const char *filename) {}
static void repl_loop(void *ctx) {}
#endif /* CONFIG_MQUICKJS */

/*
 * Embedded JavaScript example code
 * Note: mquickjs uses stricter ES5 mode
 */
static const char *js_example_code =
    "// HPM SDK mquickjs Example\n"
    "\n"
    "print('=== HPM SDK + mquickjs Demo ===');\n"
    "print('');\n"
    "\n"
    "// Fibonacci example\n"
    "function fibonacci(n) {\n"
    "    if (n <= 1) return n;\n"
    "    return fibonacci(n - 1) + fibonacci(n - 2);\n"
    "}\n"
    "\n"
    "print('Fibonacci sequence:');\n"
    "var fib = [];\n"
    "var i;\n"
    "for (i = 0; i < 10; i++) {\n"
    "    fib[i] = fibonacci(i);\n"
    "}\n"
    "print('  ' + fib.join(', '));\n"
    "print('');\n"
    "\n"
    "// Array operations\n"
    "var arr = [1, 2, 3, 4, 5];\n"
    "print('Array operations:');\n"
    "print('  Original: ' + arr.join(', '));\n"
    "print('  Length: ' + arr.length);\n"
    "print('');\n"
    "\n"
    "// Object example\n"
    "var sensor = {\n"
    "    name: 'Temperature',\n"
    "    value: 25.5,\n"
    "    unit: 'C',\n"
    "    read: function() {\n"
    "        return this.value;\n"
    "    }\n"
    "};\n"
    "\n"
    "print('Object example:');\n"
    "print('  Sensor: ' + sensor.name);\n"
    "print('  Value: ' + sensor.read() + ' ' + sensor.unit);\n"
    "print('');\n"
    "\n"
    "// Constructor function (ES5 style, no class keyword)\n"
    "function Counter(start) {\n"
    "    this.count = start;\n"
    "    this.increment = function() {\n"
    "        this.count++;\n"
    "        return this.count;\n"
    "    };\n"
    "    this.decrement = function() {\n"
    "        this.count--;\n"
    "        return this.count;\n"
    "    };\n"
    "}\n"
    "\n"
    "var counter = new Counter(10);\n"
    "print('Counter example:');\n"
    "print('  Initial: ' + counter.count);\n"
    "print('  Increment: ' + counter.increment());\n"
    "print('  Increment: ' + counter.increment());\n"
    "print('  Decrement: ' + counter.decrement());\n"
    "print('');\n"
    "\n"
    "// Math example\n"
    "print('Math functions:');\n"
    "print('  Math.PI = ' + Math.PI);\n"
    "print('  Math.sqrt(2) = ' + Math.sqrt(2));\n"
    "print('  Math.pow(2, 10) = ' + Math.pow(2, 10));\n"
    "print('  Math.random() = ' + Math.random());\n"
    "print('');\n"
    "\n"
    "// String operations\n"
    "var str = 'Hello, MicroQuickJS!';\n"
    "print('String operations:');\n"
    "print('  Original: \"' + str + '\"');\n"
    "print('  Length: ' + str.length);\n"
    "print('  Uppercase: \"' + str.toUpperCase() + '\"');\n"
    "print('  Substring(0,5): \"' + str.substring(0, 5) + '\"');\n"
    "print('');\n"
    "\n"
    "print('=== Demo Complete ===');\n";

/* LED blink example using HPM_Pin GPIO class */
#if BOARD_LED_ON_LEVEL == 1
#define JS_LED_ON_VALUE  "PinValue.High"
#define JS_LED_OFF_VALUE "PinValue.Low"
#else
#define JS_LED_ON_VALUE  "PinValue.Low"
#define JS_LED_OFF_VALUE "PinValue.High"
#endif

static const char *js_led_example_code =
    "// HPM LED Blink Example\n"
    "\n"
    "var led1 = new HPM_Pin('" BOARD_LED_GPIO_NAME "');\n"
    "\n"
    "led1.mode = PinMode.Output;\n"
    "\n"
    "console.log('LED1 mode: ' + led1.mode);\n"
    "console.log('Start blinking 10 times...');\n"
    "\n"
    "var i;\n"
    "for (i = 0; i < 5; i++) {\n"
    "    led1.level = " JS_LED_ON_VALUE ";\n"
    "    console.log('LED ON');\n"
    "    delayMs(500);  // Wait 500ms\n"
    "\n"
    "    led1.level = " JS_LED_OFF_VALUE ";\n"
    "    console.log('LED OFF');\n"
    "    delayMs(500);  // Wait 500ms\n"
    "}\n"
    "console.log('Blinking complete!');\n";

int main(void)
{
#ifdef CONFIG_MQUICKJS
    JSContext *ctx;
#endif

    /* Initialize board */
    board_init();
    board_init_led_pins();

    printf("\n");
    printf("========================================\n");
    printf("    HPM SDK + mquickjs Example\n");
    printf("    MicroQuickJS for Embedded Systems\n");
    printf("========================================\n");
    printf("\n");

#ifdef CONFIG_MQUICKJS
    /* Create JavaScript context */
    printf("Initializing JavaScript engine...\n");
    printf("Memory size: %d bytes\n", (int)sizeof(js_heap));

    ctx = JS_NewContext(js_heap, sizeof(js_heap), &js_stdlib);
    if (ctx == NULL) {
        printf("Failed to create JS context!\n");
        while (1) {
        }
    }

    JS_SetLogFunc(ctx, js_log_func);

    printf("JavaScript engine ready!\n");
    printf("\n");

    /* Run embedded example code */
    printf("--- Running embedded JavaScript example ---\n");
    run_js(ctx, js_example_code, 0, "<example>");
    printf("\n");
    run_js(ctx, js_led_example_code, 0, "<led_example>");
    /* Start REPL */
    repl_loop(ctx);

    /* Cleanup */
    JS_FreeContext(ctx);
#else
    printf("mquickjs is not enabled. Define CONFIG_MQUICKJS=1 to enable.\n");
    printf("Running embedded example code directly:\n\n");
    printf("%s\n", js_example_code);
#endif

    while (1) {
        /* Main loop */
    }

    return 0;
}
