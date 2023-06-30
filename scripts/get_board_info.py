# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

BOARD_INFO_NAME_KEY = 'name'
BOARD_INFO_SOC_KEY="soc"
BOARD_INFO_DEVICE_NAME_KEY="device"
BOARD_INFO_OPENOCD_SOC_KEY="openocd-soc"
BOARD_INFO_OPENOCD_PROBE_KEY="openocd-probe"
BOARD_INFO_FEATURE_KEY="feature"
BOARD_INFO_FLASH_KEY="on-board-flash"
BOARD_INFO_EXTRAM_KEY="on-board-ram"
BOARD_INFO_SIZE_KEY="size"
BOARD_FEATURE_DELIM=":"

def get_info(board_yml, info_type = "soc"):
    is_error = False
    ip_list = ""
    result = "not found"
    with open(board_yml, "r") as stream:
        try:
            board_info = yaml.safe_load(stream)
            if(info_type == "soc"):
                if BOARD_INFO_SOC_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_SOC_KEY])
            elif(info_type == "openocd-soc"):
                if BOARD_INFO_OPENOCD_SOC_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_OPENOCD_SOC_KEY])
            elif(info_type == "openocd-probe"):
                if BOARD_INFO_OPENOCD_PROBE_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_OPENOCD_PROBE_KEY])
            elif(info_type == "feature"):
                if BOARD_INFO_FEATURE_KEY in board_info["board"].keys():
                    result = BOARD_FEATURE_DELIM.join(board_info["board"][BOARD_INFO_FEATURE_KEY])
            elif(info_type == "flash_size"):
                if BOARD_INFO_FLASH_KEY in board_info["board"].keys():
                    result = board_info["board"][BOARD_INFO_FLASH_KEY][BOARD_INFO_SIZE_KEY]
            elif(info_type == "extram_size"):
                if BOARD_INFO_EXTRAM_KEY in board_info["board"].keys():
                    result = board_info["board"][BOARD_INFO_EXTRAM_KEY][BOARD_INFO_SIZE_KEY]
            elif (info_type == "name"):
                if BOARD_INFO_NAME_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_NAME_KEY])
            elif(info_type == "device"):
                if BOARD_INFO_DEVICE_NAME_KEY in board_info["board"].keys():
                    result = ("%s" % board_info["board"][BOARD_INFO_DEVICE_NAME_KEY])
        except yaml.YAMLError as e:
            pass
    stream.close()
    return result

if __name__ == "__main__":
    sys.stdout.write(get_info(sys.argv[1], sys.argv[2].lower()))
