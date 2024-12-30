/* This is a small demo of the high-performance FileX FAT file system. It includes setup for
 * a small 34KB RAM disk and a loop that writes and reads a small file.  */
#include "board.h"
/* This example illustrates USBX Device HID Mouse */

#include "fx_api.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_device_class_storage.h"

#include "hpm_usb_device.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PRINTF printf
#ifndef RAM_DISK_SIZE
#define RAM_DISK_SIZE (16 * 1024)
#endif

#ifndef USBX_MEMORY_SIZE
#define USBX_MEMORY_SIZE (32 * 1024)
#endif

#define THREAD_NAME "USB thread"
#define THREAD_STACK_SIZE (1024 * 2)
#define THREAD_PRIORITY (20)
#define THREAD_PREEMPT_PRIORITY (20)
#define THREAD_TIME_SLICE (1)

#define MEDIA_VOLUME_NAME "RAM DISK"
#define MEDIA_FAT_NUMBER (2)
#define MEDIA_DIR_ENTRY_NUMBER (128)
#define MEDIA_HIDDEN_SECTOR_NUMBER (0)

#define MEDIA_TOTAL_SECTORS (RAM_DISK_SIZE / MEDIA_BYTES_PER_SECTOR)
#define MEDIA_BYTES_PER_SECTOR (512)
#define MEDIA_SECTORS_PER_CLUSTER (4)
#define MEDIA_HEAD_NUMBER (1)
#define MEDIA_SECTORS_PER_TRACK (1)

#define DEVICE_CONFIGURATION_NUMBER (1)
#define DEVICE_INTERFACE_NUMBER (0)

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(64) static ULONG usb_memory[USBX_MEMORY_SIZE / sizeof(ULONG)];

static ULONG ram_disk_memory[RAM_DISK_SIZE / sizeof(ULONG)];

UX_SLAVE_CLASS_STORAGE_PARAMETER storage_parameter;
FX_MEDIA ram_disk;

/* Define the ThreadX object control blocks...  */
static TX_THREAD thread_usb;
static ULONG thread_usb_stack[THREAD_STACK_SIZE / sizeof(ULONG)];

/* Buffer for FileX RAM media initialization.  */
static ULONG buffer[512 / sizeof(ULONG)];

#define DEVICE_FRAMEWORK_LENGTH_FULL_SPEED (sizeof(device_framework_full_speed))
static UCHAR device_framework_full_speed[] = {

    /* Device descriptor */
    0x12,
    0x01,
    0x10,
    0x01,
    0x00,
    0x00,
    0x00,
    0x40,
    0xB7,
    0x34,
    0xb8,
    0x00,
    0x00,
    0x00,
    0x01,
    0x02,
    0x03,
    0x01,

    /* Configuration descriptor */
    0x09,
    0x02,
    0x20,
    0x00,
    0x01,
    0x01,
    0x00,
    0xc0,
    0x32,

    /* Interface descriptor */
    0x09,
    0x04,
    0x00,
    0x00,
    0x02,
    0x08,
    0x06,
    0x50,
    0x00,

    /* Endpoint descriptor (Bulk In) */
    0x07,
    0x05,
    0x81,
    0x02,
    0x40,
    0x00,
    0x00,

    /* Endpoint descriptor (Bulk Out) */
    0x07,
    0x05,
    0x02,
    0x02,
    0x40,
    0x00,
    0x00
};

#define DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED (sizeof(device_framework_high_speed))
static UCHAR device_framework_high_speed[] = {

    /* Device descriptor */
    0x12,
    0x01,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0xB7,
    0x34,
    0xb8,
    0x00,
    0x01,
    0x00,
    0x01,
    0x02,
    0x03,
    0x01,

    /* Device qualifier descriptor */
    0x0a,
    0x06,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,

    /* Configuration descriptor */
    0x09,
    0x02,
    0x20,
    0x00,
    0x01,
    0x01,
    0x00,
    0xc0,
    0x32,

    /* Interface descriptor */
    0x09,
    0x04,
    0x00,
    0x00,
    0x02,
    0x08,
    0x06,
    0x50,
    0x00,

    /* Endpoint descriptor (Bulk In) */
    0x07,
    0x05,
    0x81,
    0x02,
    0x00,
    0x02,
    0x00,

    /* Endpoint descriptor (Bulk Out) */
    0x07,
    0x05,
    0x02,
    0x02,
    0x00,
    0x02,
    0x00
};

/*
 * String Device Framework :
 * Byte 0 and 1 : Word containing the language ID : 0x0904 for US
 * Byte 2       : Byte containing the index of the descriptor
 * Byte 3       : Byte containing the length of the descriptor string
 */

#define STRING_FRAMEWORK_LENGTH (sizeof(string_framework))
static UCHAR string_framework[] = {
    /* Manufacturer string descriptor : Index 1 */
    0x09,
    0x04,
    0x01,
    21U,
    'H',
    'P',
    'M',
    'I',
    'C',
    'R',
    'O',
    ' ',
    'S',
    'E',
    'M',
    'I',
    'C',
    'O',
    'N',
    'D',
    'U',
    'C',
    'T',
    'O',
    'R',

    /* Product string descriptor : Index 2 */
    0x09,
    0x04,
    0x02,
    16U,
    'U',
    'S',
    'B',
    ' ',
    'S',
    'T',
    'O',
    'R',
    'A',
    'G',
    'E',
    ' ',
    'D',
    'E',
    'M',
    'O',

    /* Serial Number string descriptor : Index 3 */
    0x09,
    0x04,
    0x03,
    0x04,
    0x30,
    0x30,
    0x30,
    0x31
};

/*
 * Multiple languages are supported on the device, to add
 * a language besides english, the unicode language code must
 * be appended to the language_id_framework array and the length
 * adjusted accordingly.
 */
#define LANGUAGE_ID_FRAMEWORK_LENGTH (sizeof(language_id_framework))
static UCHAR language_id_framework[] = {

    /* English. */
    0x09,
    0x04
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Define external function prototypes. */
extern VOID _fx_ram_driver(FX_MEDIA *media_ptr);
extern void usb_device_setup(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
static UINT demo_thread_media_status(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status)
{
    (void)storage;
    (void)lun;
    (void)media_id;
    (void)media_status;
    /* The RAM disk drive never fails. This is just for demo only !!!! */
    return UX_SUCCESS;
}

static UINT demo_thread_media_read(VOID *storage, ULONG lun, UCHAR *data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{
    (void)storage;
    (void)lun;
    (void)media_status;
    ram_disk.fx_media_driver_logical_sector = lba;
    ram_disk.fx_media_driver_sectors = number_blocks;
    ram_disk.fx_media_driver_request = FX_DRIVER_READ;
    ram_disk.fx_media_driver_buffer = data_pointer;
    _fx_ram_driver(&ram_disk);

    return ram_disk.fx_media_driver_status;
}

static UINT demo_thread_media_write(VOID *storage, ULONG lun, UCHAR *data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{
    (void)storage;
    (void)lun;
    (void)media_status;
    ram_disk.fx_media_driver_logical_sector = lba;
    ram_disk.fx_media_driver_sectors = number_blocks;
    ram_disk.fx_media_driver_request = FX_DRIVER_WRITE;
    ram_disk.fx_media_driver_buffer = data_pointer;
    _fx_ram_driver(&ram_disk);

    return ram_disk.fx_media_driver_status;
}

static VOID demo_usb_msd_instance_activate(VOID *msd_instance)
{
    UX_PARAMETER_NOT_USED(msd_instance);

    PRINTF("USB MSD device activate\r\n");
}

static VOID thread_usb_entry(ULONG thread_input)
{
    UX_SLAVE_CLASS_STORAGE_LUN *lun;
    ULONG status;
    (void)thread_input;
    /* Format the ram drive. */
    status = fx_media_format(&ram_disk,
        _fx_ram_driver,
        (VOID *)ram_disk_memory,
        (UCHAR *)buffer,
        sizeof(buffer),
        MEDIA_VOLUME_NAME,
        MEDIA_FAT_NUMBER,
        MEDIA_DIR_ENTRY_NUMBER,
        MEDIA_HIDDEN_SECTOR_NUMBER,
        MEDIA_TOTAL_SECTORS,
        MEDIA_BYTES_PER_SECTOR,
        MEDIA_SECTORS_PER_CLUSTER,
        MEDIA_HEAD_NUMBER,
        MEDIA_SECTORS_PER_TRACK);

    /* Check the media format status.  */
    if (status != FX_SUCCESS)
        goto err;

    /*
     * The code below is required for installing the device portion of USBX.
     */
    status = ux_device_stack_initialize(device_framework_high_speed,
        DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED,
        device_framework_full_speed,
        DEVICE_FRAMEWORK_LENGTH_FULL_SPEED,
        string_framework,
        STRING_FRAMEWORK_LENGTH,
        language_id_framework,
        LANGUAGE_ID_FRAMEWORK_LENGTH,
        UX_NULL);
    if (status != UX_SUCCESS)
        goto err;

    /* Store the number of LUN in this device storage instance. */
    storage_parameter.ux_slave_class_storage_parameter_number_lun = 1;

    lun = &storage_parameter.ux_slave_class_storage_parameter_lun[0];

    /* Initialize the storage class parameters for reading/writing to the Flash Disk. */
    lun->ux_slave_class_storage_media_last_lba = MEDIA_TOTAL_SECTORS;
    lun->ux_slave_class_storage_media_block_length = MEDIA_BYTES_PER_SECTOR;
    lun->ux_slave_class_storage_media_type = 0;
    lun->ux_slave_class_storage_media_removable_flag = 0x80;
    lun->ux_slave_class_storage_media_read = demo_thread_media_read;
    lun->ux_slave_class_storage_media_write = demo_thread_media_write;
    lun->ux_slave_class_storage_media_status = demo_thread_media_status;

    storage_parameter.ux_slave_class_storage_instance_activate = demo_usb_msd_instance_activate;

    /* Initialize the device storage class. The class is connected with interface 0 on configuration 1. */
    status = ux_device_stack_class_register(
        _ux_system_slave_class_storage_name, _ux_device_class_storage_entry, DEVICE_CONFIGURATION_NUMBER, DEVICE_INTERFACE_NUMBER, (VOID *)&storage_parameter);
    if (status != UX_SUCCESS)
        goto err;

    usb_device_setup();

    return;

err:
    PRINTF("thread_usb_entry: ERROR 0x%x\r\n", status);
    while (1) {
    }
}

static VOID usbx_error_callback(UINT system_level, UINT system_context, UINT error_code)
{
    /*
     * Refer to ux_api.h. For example,
     * UX_SYSTEM_LEVEL_INTERRUPT, UX_SYSTEM_CONTEXT_DCD, UX_DEVICE_HANDLE_UNKNOWN
     */
    PRINTF("USBX ERR: system level(%d), context(%d), error code(0x%x)\r\n", system_level, system_context, error_code);
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
}

int main(void)
{
    /* Initialize the board. */
    board_init();
    board_init_usb(HPM_USB0);
    board_init_gpio_pins();

    usb_device_hw_setup();

    PRINTF("USBX device mass storage example\r\n");

    /* Enter the ThreadX kernel. */
    tx_kernel_enter();

    return 0;
}

void tx_application_define(void *first_unused_memory)
{
    UINT status;

    UX_PARAMETER_NOT_USED(first_unused_memory);

    /* Initialize FileX. */
    fx_system_initialize();

    /* Initialize USBX Memory */
    status = ux_system_initialize((VOID *)usb_memory, USBX_MEMORY_SIZE, UX_NULL, 0);
    if (status != UX_SUCCESS)
        goto err;

    /* error callback */
    ux_utility_error_callback_register(usbx_error_callback);

    /* Create the main demo thread.  */
    tx_thread_create(
        &thread_usb, THREAD_NAME, thread_usb_entry, 0, thread_usb_stack, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_PREEMPT_PRIORITY, THREAD_TIME_SLICE, TX_AUTO_START);

    return;

err:
    PRINTF("tx_application_define: ERROR 0x%x\r\n", status);
    while (1) {
    }
}