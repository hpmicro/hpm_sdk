# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

import pywinusb.hid as hid
import os
import time
import sys

# VID and PID customization changes here...

VID = 0xcafe
PID = 0x4004

def search_dev():
    filter = hid.HidDeviceFilter(vendor_id = VID, product_id = PID)
    hid_device = filter.get_devices()
    return hid_device

def recv_data(data):
    print("<=================== USB HID Read ========================>")
    for i in range(0, len(data)):
        print("0x{0:02x}" .format(data[i]), end=" ")
    print("\n")
    return None

def send_data(report):
    print("<=================== USB HID Write ========================>")
    buffer = [0xff]*1024
    buffer[0] = report[0].report_id
    print("0x{0:02x}" .format(buffer[0]), end=" ")

    for i in range(1,1024):
        buffer[i] = i % 256
        print("0x{0:02x}" .format(buffer[i]), end=" ")
    print("\n")

    report[0].set_raw_data(buffer)
    report[0].send()
    return None

if __name__ == '__main__':
    device = search_dev()[0]
    device.open()
    device.set_raw_data_handler(recv_data)
    send_data(device.find_output_reports())
    time.sleep(1)

