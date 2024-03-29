#include "usbd_core.h"
#include "usb_midi.h"
#include "board.h"
#include "hpm_gpio_drv.h"

#if defined(BOARD_BUTTON_PRESSED_VALUE)
#define APP_BUTTON_PRESSED_VALUE BOARD_BUTTON_PRESSED_VALUE
#else
#define APP_BUTTON_PRESSED_VALUE 0
#endif

#define MIDI_OUT_EP 0x02
#define MIDI_IN_EP  0x81

#define USB_CONFIG_SIZE (9 + 9 + 9 + 9 + 7 + MIDI_SIZEOF_JACK_DESC + 9 + 5 + 9 + 5)

#ifdef CONFIG_USB_HS
#define MIDI_EP_MPS 512
#else
#define MIDI_EP_MPS 64
#endif

const uint8_t midi_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /* Standard AC Interface Descriptor */
    0x09,
    0x04,
    0x00,
    0x00,
    0x00,
    0x01,
    0x01,
    0x00,
    0x00,
    /* Class-specific AC Interface Descriptor */
    0x09,
    0x24,
    0x01,
    0x00,
    0x01,
    0x09,
    0x00,
    0x01,
    0x01,
    /* MIDIStreaming Interface Descriptors */
    0x09,
    0x04,
    0x01,
    0x00,
    0x02,
    0x01,
    0x03,
    0x00,
    0x00,
    /* Class-Specific MS Interface Header Descriptor */
    0x07,
    0x24,
    0x01,
    0x00,
    0x01,
    WBVAL(65),

    MIDI_JACK_DESCRIPTOR_INIT(0x01),
    /* OUT endpoint descriptor */
    0x09, 0x05, MIDI_OUT_EP, 0x02, WBVAL(MIDI_EP_MPS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x01,

    /* IN endpoint descriptor */
    0x09, 0x05, MIDI_IN_EP, 0x02, WBVAL(MIDI_EP_MPS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x03,

    /*
     * string0 descriptor
     */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /*
     * string1 descriptor
     */
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    /*
     * string2 descriptor
     */
    0x28,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'M', 0x00,                  /* wcChar10 */
    'I', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'I', 0x00,                  /* wcChar13 */
    ' ', 0x00,                  /* wcChar14 */
    'D', 0x00,                  /* wcChar15 */
    'E', 0x00,                  /* wcChar16 */
    'M', 0x00,                  /* wcChar17 */
    'O', 0x00,                  /* wcChar18 */
    /*
     * string3 descriptor
     */
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '0', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    /*
     * device qualifier descriptor
     */
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static USB_NOCACHE_RAM_SECTION uint8_t s_sendbuffer[4];
static volatile bool s_midi_usb_busy;
static bool s_auto_play;
static uint32_t s_note_pos;
static uint32_t s_note_pos_prev;
/* Store example melody as an array of note values */
static const uint8_t s_note_sequence[] = {
    74, 78, 81, 86, 90, 93, 98, 102, 57, 61, 66, 69, 73, 78, 81, 85, 88, 92, 97, 100, 97, 92, 88, 85, 81, 78,
    74, 69, 66, 62, 57, 62, 66, 69, 74, 78, 81, 86, 90, 93, 97, 102, 97, 93, 90, 85, 81, 78, 73, 68, 64, 61,
    56, 61, 64, 68, 74, 78, 81, 86, 90, 93, 98, 102
};

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid;

    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_midi_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;
}

void usbd_midi_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    s_midi_usb_busy = false;
}

struct usbd_endpoint midi_out_ep = {
    .ep_addr = MIDI_OUT_EP,
    .ep_cb = usbd_midi_bulk_out
};

struct usbd_endpoint midi_in_ep = {
    .ep_addr = MIDI_IN_EP,
    .ep_cb = usbd_midi_bulk_in
};

void midi_set_auto_play(bool set)
{
    s_auto_play = set;
}

void midi_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, midi_descriptor);
    usbd_add_interface(busid, &intf0);
    usbd_add_interface(busid, &intf1);
    usbd_add_endpoint(busid, &midi_out_ep);
    usbd_add_endpoint(busid, &midi_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void midi_task_286ms(uint8_t busid)
{
    const uint8_t cable_num = 0; /* MIDI jack associated with USB endpoint */
    const uint8_t channel = 0;   /* 0 for channel 1 */
    int ret;

    if (s_auto_play) {
        s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_ON;
        s_sendbuffer[1] = NoteOn | channel;
        s_sendbuffer[2] = s_note_sequence[s_note_pos];
        s_sendbuffer[3] = 127;  /* velocity */
        s_midi_usb_busy = true;
        ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
        if (ret < 0) {
            printf("error\n");
            return;
        }
        while (s_midi_usb_busy) {
        }

        if (s_note_pos > 0) {
            s_note_pos_prev = s_note_pos - 1;
        } else {
            s_note_pos_prev = sizeof(s_note_sequence) - 1;
        }
        s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_OFF;
        s_sendbuffer[1] = NoteOff | channel;
        s_sendbuffer[2] = s_note_sequence[s_note_pos_prev];
        s_sendbuffer[3] = 0;  /* velocity */
        s_midi_usb_busy = true;
        ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
        if (ret < 0) {
            printf("error\n");
            return;
        }
        while (s_midi_usb_busy) {
        }

        s_note_pos++;
        if (s_note_pos >= sizeof(s_note_sequence)) {
            s_note_pos = 0;
        }
    }
}

void midi_task_main(uint8_t busid)
{
    const uint8_t cable_num = 0; /* MIDI jack associated with USB endpoint */
    const uint8_t channel = 0;   /* 0 for channel 1 */
    bool key_pushed = false;
    int ret;

    if (!s_auto_play) {
        if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == APP_BUTTON_PRESSED_VALUE) {
            key_pushed = true;
            s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_ON;
            s_sendbuffer[1] = NoteOn | channel;
            s_sendbuffer[2] = s_note_sequence[s_note_pos];
            s_sendbuffer[3] = 100;  /* velocity */
            s_midi_usb_busy = true;
            ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
            if (ret < 0) {
                printf("error\n");
                return;
            }
            while (s_midi_usb_busy) {
            }
        }

        if (key_pushed) {
            key_pushed = false;
            while (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == APP_BUTTON_PRESSED_VALUE) {
            }
            s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_OFF;
            s_sendbuffer[1] = NoteOff | channel;
            s_sendbuffer[2] = s_note_sequence[s_note_pos];
            s_sendbuffer[3] = 0;  /* velocity */
            s_midi_usb_busy = true;
            ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
            if (ret < 0) {
                printf("error\n");
                return;
            }
            while (s_midi_usb_busy) {
            }
        }

        s_note_pos++;
        if (s_note_pos >= sizeof(s_note_sequence)) {
            s_note_pos = 0;
        }
    }
}
