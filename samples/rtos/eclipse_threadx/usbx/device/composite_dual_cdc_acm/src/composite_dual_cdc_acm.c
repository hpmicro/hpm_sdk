/* This is a small demo of the high-performance FileX FAT file system. It
 * includes setup for a small 34KB RAM disk and a loop that writes and reads a
 * small file.  */
#include "board.h"
/* This example illustrates USBX Device HID Mouse */

#include "ux_api.h"
#include "ux_device_class_cdc_acm.h"
#include "ux_system.h"
#include "ux_utility.h"

#include "hpm_usb_device.h"
#include "ux_device_descriptor.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PRINTF printf
#define UX_DEMO_STACK_SIZE (1024 * 2)

#ifndef USBX_MEMORY_SIZE
#define USBX_MEMORY_SIZE (1024 * 64)
#endif

UX_SLAVE_CLASS_CDC_ACM_PARAMETER parameter1;
UX_SLAVE_CLASS_CDC_ACM_PARAMETER parameter2;
UX_SLAVE_CLASS_CDC_ACM *cdc1;
UX_SLAVE_CLASS_CDC_ACM *cdc2;
TX_THREAD demo_thread1;
TX_THREAD demo_thread2;
static ULONG demo_thread_stack1[UX_DEMO_STACK_SIZE / sizeof(ULONG)];
static ULONG demo_thread_stack2[UX_DEMO_STACK_SIZE / sizeof(ULONG)];

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(64) static ULONG usb_memory[USBX_MEMORY_SIZE / sizeof(ULONG)];

extern void usb_device_setup(void);
static VOID demo_cdc_instance1_activate(VOID *cdc_instance)
{
    /* Save the CDC instance.  */
    PRINTF("CDC ACM1 device activate\r\n");
    cdc1 = (UX_SLAVE_CLASS_CDC_ACM *)cdc_instance;
}

static VOID demo_cdc_instance1_deactivate(VOID *cdc_instance)
{
    (void)cdc_instance;
    cdc1 = UX_NULL;
    PRINTF("CDC ACM1 device deactivate\r\n");
}

static VOID demo_cdc_instance2_activate(VOID *cdc_instance)
{
    /* Save the CDC instance.  */
    PRINTF("CDC ACM2 device activate\r\n");
    cdc2 = (UX_SLAVE_CLASS_CDC_ACM *)cdc_instance;
}

static VOID demo_cdc_instance2_deactivate(VOID *cdc_instance)
{
    (void)cdc_instance;
    cdc2 = UX_NULL;
    PRINTF("CDC ACM2 device deactivate\r\n");
}

static void demo_thread1_entry(ULONG arg)
{
    ULONG status;
    ULONG actual_length;
    ULONG requested_length;
    UCHAR buffer[256] = {
        0,
    };
    (void)arg;
    while (1) {
        /* Ensure the CDC class is mounted.  */
        if (cdc1 == UX_NULL) {
            tx_thread_sleep(100);
            continue;
        }

        /* Read from the CDC class.  */
        status = ux_device_class_cdc_acm_read(cdc1, buffer, sizeof(buffer), &actual_length);
        if (status != UX_SUCCESS)
            continue;

        PRINTF("SEND: ");

        /* The actual length becomes the requested length.  */
        requested_length = actual_length;

        /* Check the status.  If OK, we will write to the CDC instance.  */
        status = ux_device_class_cdc_acm_write(cdc1, buffer, requested_length, &actual_length);
        if (status != UX_SUCCESS)
            continue;

        buffer[actual_length] = 0;
        PRINTF("%s", buffer);

        /* Check for CR/LF.  */
        if (buffer[requested_length - 1] == '\r') {
            /* Copy LF value into user buffer.  */
            ux_utility_memory_copy(buffer, "\n", 1);

            /* And send it again.  */
            status = ux_device_class_cdc_acm_write(cdc1, buffer, 1, &actual_length);

            if (status != UX_SUCCESS)
                continue;

            buffer[actual_length] = 0;
            PRINTF("%s", buffer);
        }

        /* Fill buffer.  */
        buffer[0] = 'a';
        buffer[1] = 'b';
        buffer[2] = 'c';
        buffer[3] = 'd';
        buffer[4] = 'e';
        buffer[5] = 'f';
        buffer[6] = '\r';
        buffer[7] = '\n';

        /* And send 8 bytes.  */
        status = ux_device_class_cdc_acm_write(cdc1, buffer, 8, &actual_length);

        if (status != UX_SUCCESS)
            continue;

        buffer[actual_length] = 0;
        PRINTF("%s", buffer);

        /* And send 0 byte packet. Forced ZLP.  */
        status = ux_device_class_cdc_acm_write(cdc1, buffer, 0, &actual_length);

        if (status != UX_SUCCESS)
            continue;

        PRINTF("\r\n");
    }
}

static void demo_thread2_entry(ULONG arg)
{
    ULONG status;
    ULONG actual_length;
    ULONG requested_length;
    UCHAR buffer[256] = {
        0,
    };
    (void)arg;
    while (1) {
        /* Ensure the CDC class is mounted.  */
        if (cdc2 == UX_NULL) {
            tx_thread_sleep(100);
            continue;
        }

        /* Read from the CDC class.  */
        status = ux_device_class_cdc_acm_read(cdc2, buffer, sizeof(buffer), &actual_length);
        if (status != UX_SUCCESS)
            continue;

        PRINTF("SEND: ");

        /* The actual length becomes the requested length.  */
        requested_length = actual_length;

        /* Check the status.  If OK, we will write to the CDC instance.  */
        status = ux_device_class_cdc_acm_write(cdc2, buffer, requested_length, &actual_length);
        if (status != UX_SUCCESS)
            continue;

        buffer[actual_length] = 0;
        PRINTF("%s", buffer);

        /* Check for CR/LF.  */
        if (buffer[requested_length - 1] == '\r') {
            /* Copy LF value into user buffer.  */
            ux_utility_memory_copy(buffer, "\n", 1);

            /* And send it again.  */
            status = ux_device_class_cdc_acm_write(cdc2, buffer, 1, &actual_length);

            if (status != UX_SUCCESS)
                continue;

            buffer[actual_length] = 0;
            PRINTF("%s", buffer);
        }

        /* Fill buffer.  */
        buffer[0] = '1';
        buffer[1] = '2';
        buffer[2] = '3';
        buffer[3] = '4';
        buffer[4] = '5';
        buffer[5] = '6';
        buffer[6] = '\r';
        buffer[7] = '\n';

        /* And send 8 bytes.  */
        status = ux_device_class_cdc_acm_write(cdc2, buffer, 8, &actual_length);

        if (status != UX_SUCCESS)
            continue;

        buffer[actual_length] = 0;
        PRINTF("%s", buffer);

        /* And send 0 byte packet. Forced ZLP.  */
        status = ux_device_class_cdc_acm_write(cdc2, buffer, 0, &actual_length);

        if (status != UX_SUCCESS)
            continue;

        PRINTF("\r\n");
    }
}

usb_device_handle_t usb_handle;
static ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(USB_SOC_DCD_DATA_RAM_ADDRESS_ALIGNMENT) dcd_data_t _dcd_data;
static void usb_device_hw_setup(void)
{
    usb_handle.regs = (USB_Type *)HPM_USB0_BASE;
    usb_handle.dcd_data = &_dcd_data;
    uint32_t int_mask;
    int_mask = (USB_USBINTR_UE_MASK | USB_USBINTR_UEE_MASK | USB_USBINTR_PCE_MASK | USB_USBINTR_URE_MASK);

    usb_device_init(&usb_handle, int_mask);

    return;
}

int main(void)
{
    /* Initialize the board. */
    board_init();
    board_init_usb(HPM_USB0);
    board_init_gpio_pins();

    usb_device_hw_setup();

    PRINTF("Start USBX device composite example...\r\n");

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();

    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    UINT status;

    UX_PARAMETER_NOT_USED(first_unused_memory);

    /* Initialize USBX Memory */
    status = ux_system_initialize((VOID *)usb_memory, USBX_MEMORY_SIZE, UX_NULL, 0);
    if (status != UX_SUCCESS)
        goto err;

    usb_device_setup();

    /* The code below is required for installing the device portion of USBX. No
     * call back for device status change in this example. */
    status = ux_device_stack_initialize(ux_get_hs_framework(),
        ux_get_hs_framework_length(),
        ux_get_fs_framework(),
        ux_get_fs_framework_length(),
        ux_get_string_framework(),
        ux_get_string_framework_length(),
        ux_get_language_framework(),
        ux_get_language_framework_length(),
        UX_NULL);
    if (status != UX_SUCCESS)
        goto err;

    /* Set the parameters for callback when insertion/extraction of a CDC
     * device.  */
    parameter1.ux_slave_class_cdc_acm_instance_activate = demo_cdc_instance1_activate;
    parameter1.ux_slave_class_cdc_acm_instance_deactivate = demo_cdc_instance1_deactivate;

    /* Initialize the device CDC class. This class owns both interfaces
     * starting with 0. */
    status = ux_device_stack_class_register((UCHAR *)"ACM1", ux_device_class_cdc_acm_entry, 1, 0, &parameter1);

    if (status != UX_SUCCESS)
        goto err;

    /* Set the parameters for callback when insertion/extraction of a CDC
     * device.  */
    parameter2.ux_slave_class_cdc_acm_instance_activate = demo_cdc_instance2_activate;
    parameter2.ux_slave_class_cdc_acm_instance_deactivate = demo_cdc_instance2_deactivate;

    /* Initialize the device CDC class. This class owns both interfaces
     * starting with 2. */
    status = ux_device_stack_class_register((UCHAR *)"ACM2", ux_device_class_cdc_acm_entry, 1, 2, &parameter2);

    if (status != UX_SUCCESS)
        goto err;

    /* Create the acm1 thread.  */
    tx_thread_create(&demo_thread1, "USBX demo1", demo_thread1_entry, 0, demo_thread_stack1, UX_DEMO_STACK_SIZE, 20, 20, 1, TX_AUTO_START);

    /* Create the acm2 thread.  */
    tx_thread_create(&demo_thread2, "USBX demo2", demo_thread2_entry, 0, demo_thread_stack2, UX_DEMO_STACK_SIZE, 20, 20, 1, TX_AUTO_START);

    return;

err:
    PRINTF("tx_application_define: ERROR 0x%x\r\n", status);
    while (1) {
    }
}