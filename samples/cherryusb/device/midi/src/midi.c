#include "usbd_core.h"
#include "usb_audio.h"
#include "usb_midi.h"
#include "board.h"
#include "hpm_gpio_drv.h"

#define MIDI_OUT_EP 0x02
#define MIDI_IN_EP  0x81

#define AUDIO_MS_SIZ (7 + MIDI_SIZEOF_JACK_DESC + 9 + 5 + 9 + 5)

#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +                                       \
                                                  AUDIO_AC_DESCRIPTOR_INIT_LEN(1) +         \
                                                  AUDIO_MS_STANDARD_DESCRIPTOR_INIT_LEN +   \
                                                  AUDIO_MS_SIZ)

#define AUDIO_AC_SIZ (AUDIO_SIZEOF_AC_HEADER_DESC(1))

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x02, AUDIO_AC_SIZ, 0x00, 0x01),
    AUDIO_MS_STANDARD_DESCRIPTOR_INIT(0x01, 0x02),
    MIDI_CS_HEADER_DESCRIPTOR_INIT(AUDIO_MS_SIZ),
    MIDI_JACK_DESCRIPTOR_INIT(0x01),
    /* OUT endpoint descriptor */
    0x09, 0x05, MIDI_OUT_EP, 0x02, WBVAL(USB_BULK_EP_MPS_HS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x01,

    /* IN endpoint descriptor */
    0x09, 0x05, MIDI_IN_EP, 0x02, WBVAL(USB_BULK_EP_MPS_HS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x03,
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x01, AUDIO_AC_SIZ, 0x00, 0x01),
    AUDIO_MS_STANDARD_DESCRIPTOR_INIT(0x01, 0x02),
    MIDI_CS_HEADER_DESCRIPTOR_INIT(AUDIO_MS_SIZ),
    MIDI_JACK_DESCRIPTOR_INIT(0x01),
    /* OUT endpoint descriptor */
    0x09, 0x05, MIDI_OUT_EP, 0x02, WBVAL(USB_BULK_EP_MPS_FS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x01,

    /* IN endpoint descriptor */
    0x09, 0x05, MIDI_IN_EP, 0x02, WBVAL(USB_BULK_EP_MPS_FS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x03,
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, 0x01),
};

static const uint8_t other_speed_config_descriptor_hs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x01, AUDIO_AC_SIZ, 0x00, 0x01),
    AUDIO_MS_STANDARD_DESCRIPTOR_INIT(0x01, 0x02),
    MIDI_CS_HEADER_DESCRIPTOR_INIT(AUDIO_MS_SIZ),
    MIDI_JACK_DESCRIPTOR_INIT(0x01),
    /* OUT endpoint descriptor */
    0x09, 0x05, MIDI_OUT_EP, 0x02, WBVAL(USB_BULK_EP_MPS_FS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x01,

    /* IN endpoint descriptor */
    0x09, 0x05, MIDI_IN_EP, 0x02, WBVAL(USB_BULK_EP_MPS_FS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x03,
};

static const uint8_t other_speed_config_descriptor_fs[] = {
    USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    AUDIO_AC_DESCRIPTOR_INIT(0x00, 0x01, AUDIO_AC_SIZ, 0x00, 0x01),
    AUDIO_MS_STANDARD_DESCRIPTOR_INIT(0x01, 0x02),
    MIDI_CS_HEADER_DESCRIPTOR_INIT(AUDIO_MS_SIZ),
    MIDI_JACK_DESCRIPTOR_INIT(0x01),
    /* OUT endpoint descriptor */
    0x09, 0x05, MIDI_OUT_EP, 0x02, WBVAL(USB_BULK_EP_MPS_HS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x01,

    /* IN endpoint descriptor */
    0x09, 0x05, MIDI_IN_EP, 0x02, WBVAL(USB_BULK_EP_MPS_HS), 0x00, 0x00, 0x00,
    0x05, 0x25, 0x01, 0x01, 0x03,
};

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro MIDI DEMO",           /* Product */
    "2024051701",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return other_speed_config_descriptor_hs;
    } else if (speed == USB_SPEED_FULL) {
        return other_speed_config_descriptor_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor midi_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static USB_NOCACHE_RAM_SECTION uint8_t s_sendbuffer[4];
static volatile bool s_midi_usb_busy;
static volatile bool s_auto_play;
static uint32_t s_auto_note_pos;
static uint32_t s_auto_note_pos_prev;
static uint32_t s_manual_note_pos;
static uint32_t s_manual_note_pos_prev;
static int8_t manual_state;

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
    usbd_desc_register(busid, &midi_descriptor);
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

    if (usb_device_is_configured(busid) == false) {
        return;
    }

    if (s_auto_play) {
        s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_ON;
        s_sendbuffer[1] = NoteOn | channel;
        s_sendbuffer[2] = s_note_sequence[s_auto_note_pos];
        s_sendbuffer[3] = 127;  /* velocity */
        s_midi_usb_busy = true;
        ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
        if (ret < 0) {
            printf("error\n");
            return;
        }
        while (s_midi_usb_busy) {
        }

        if (s_auto_note_pos > 0) {
            s_auto_note_pos_prev = s_auto_note_pos - 1;
        } else {
            s_auto_note_pos_prev = sizeof(s_note_sequence) - 1;
        }

        s_auto_note_pos++;
        if (s_auto_note_pos >= sizeof(s_note_sequence)) {
            s_auto_note_pos = 0;
        }

        s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_OFF;
        s_sendbuffer[1] = NoteOff | channel;
        s_sendbuffer[2] = s_note_sequence[s_auto_note_pos_prev];
        s_sendbuffer[3] = 0;  /* velocity */
        s_midi_usb_busy = true;
        ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
        if (ret < 0) {
            printf("error\n");
            return;
        }
        while (s_midi_usb_busy) {
        }

        manual_state = 0;
        s_manual_note_pos = 0;
        s_manual_note_pos_prev = 0;
    } else {
        switch (manual_state) {
        case 1:
            if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) != BOARD_BUTTON_PRESSED_VALUE) {
                s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_OFF;
                s_sendbuffer[1] = NoteOff | channel;
                s_sendbuffer[2] = s_note_sequence[s_manual_note_pos_prev];
                s_sendbuffer[3] = 0;  /* velocity */
                s_midi_usb_busy = true;
                ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
                if (ret < 0) {
                    printf("error\n");
                    return;
                }
                while (s_midi_usb_busy) {
                }
                manual_state = 0;
            }
            break;

        case 0:
        default:
            if (gpio_read_pin(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN) == BOARD_BUTTON_PRESSED_VALUE) {
                s_sendbuffer[0] = (cable_num << 4) | MIDI_CIN_NOTE_ON;
                s_sendbuffer[1] = NoteOn | channel;
                s_sendbuffer[2] = s_note_sequence[s_manual_note_pos];
                s_sendbuffer[3] = 100;  /* velocity */
                s_midi_usb_busy = true;
                ret = usbd_ep_start_write(busid, MIDI_IN_EP, s_sendbuffer, 4);
                if (ret < 0) {
                    printf("error\n");
                    return;
                }
                while (s_midi_usb_busy) {
                }

                if (s_manual_note_pos > 0) {
                    s_manual_note_pos_prev = s_manual_note_pos - 1;
                } else {
                    s_manual_note_pos_prev = sizeof(s_note_sequence) - 1;
                }

                s_manual_note_pos++;
                if (s_manual_note_pos >= sizeof(s_note_sequence)) {
                    s_manual_note_pos = 0;
                }

                manual_state = 1;
            }
            break;
        }

        s_auto_note_pos = 0;
        s_auto_note_pos_prev = 0;
    }
}
