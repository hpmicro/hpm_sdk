## Script for hpmicro ##
sdk_inc(common)
sdk_inc(osal)
sdk_inc(core)
sdk_inc(class/cdc)
sdk_inc(class/hid)
sdk_inc(class/midi)
sdk_inc(class/msc)
sdk_inc(class/audio)
sdk_inc(class/video)
sdk_inc(class/hub)
sdk_inc(class/wireless)
sdk_inc(port/ehci)
sdk_inc(port/hpmicro)

sdk_src(port/hpmicro/usb_glue_hpm.c)

if(CONFIG_USB_DEVICE_CDC OR CONFIG_USB_DEVICE_CDC_ACM OR CONFIG_USB_DEVICE_CDC_ECM
    OR CONFIG_USB_DEVICE_HID OR CONFIG_USB_DEVICE_MSC
    OR CONFIG_USB_DEVICE_AUDIO OR CONFIG_USB_DEVICE_VIDEO
    OR CONFIG_USB_DEVICE_RNDIS OR CONFIG_USB_DEVICE_MIDI)
  set(CONFIG_CHERRYUSB_DEVICE 1)
endif()

if(CONFIG_CHERRYUSB_DEVICE)
  sdk_src(core/usbd_core.c)
  sdk_src(port/hpmicro/usb_dc_hpm.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_CDC class/cdc/usbd_cdc_acm.c)  ## legacy for old version
  sdk_src_ifdef(CONFIG_USB_DEVICE_CDC_ACM class/cdc/usbd_cdc_acm.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_CDC_ECM class/cdc/usbd_cdc_ecm.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_HID class/hid/usbd_hid.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_MSC class/msc/usbd_msc.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_AUDIO class/audio/usbd_audio.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_VIDEO class/video/usbd_video.c)
  sdk_src_ifdef(CONFIG_USB_DEVICE_RNDIS class/wireless/usbd_rndis.c)
endif()

if(CONFIG_USB_HOST_CDC_ACM OR CONFIG_USB_HOST_CDC_ECM OR CONFIG_USB_HOST_HID
    OR CONFIG_USB_HOST_MSC OR CONFIG_USB_HOST_AUDIO OR CONFIG_USB_HOST_VIDEO
    OR CONFIG_USB_HOST_RNDIS)
  set(CONFIG_CHERRYUSB_HOST 1)
endif()

if(CONFIG_CHERRYUSB_HOST)
  sdk_src(core/usbh_core.c)
  sdk_src(class/hub/usbh_hub.c)
  sdk_src(port/ehci/usb_hc_ehci.c)
  sdk_src(port/hpmicro/usb_hc_hpm.c)
  sdk_src_ifdef(CONFIG_USB_HOST_CDC_ACM class/cdc/usbh_cdc_acm.c)
  sdk_src_ifdef(CONFIG_USB_HOST_CDC_ECM class/cdc/usbh_cdc_ecm.c)
  sdk_src_ifdef(CONFIG_USB_HOST_HID class/hid/usbh_hid.c)
  sdk_src_ifdef(CONFIG_USB_HOST_MSC class/msc/usbh_msc.c)
  sdk_src_ifdef(CONFIG_USB_HOST_AUDIO class/audio/usbh_audio.c)
  sdk_src_ifdef(CONFIG_USB_HOST_VIDEO class/video/usbh_video.c)
  sdk_src_ifdef(CONFIG_USB_HOST_RNDIS class/wireless/usbh_rndis.c)
  if(CONFIG_USB_HOST_CDC_ECM OR CONFIG_USB_HOST_RNDIS)
    sdk_src(platform/lwip/usbh_lwip.c)
  endif()
  if(CONFIG_USB_HOST_HID_LVGL)
    sdk_src_glob(platform/lvgl/*.c)
    sdk_inc(platform/lvgl)
  endif()
endif()

if(CONFIG_FREERTOS)
  sdk_src(osal/usb_osal_freertos.c)
endif()

if(CONFIG_RTTHREAD_NANO)
  sdk_src(osal/usb_osal_rtthread.c)
endif()
