# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import re
import sys
import yaml
import subprocess
import shutil
import hashlib
import ctypes

sys.path.append(os.path.dirname(sys.argv[0]))
import get_board_info

APP_DEPENDENCY="dependency"
SOC_IP_FEATURE_FILE_NAME="hpm_soc_ip_feature.h"
SOC_IP_FEATURE_PREFIX="HPM_IP_FEATURE_"

def parse_app_yml(app_yml):
    app_info = None
    with open(app_yml, "r", encoding='utf-8', errors='ignore') as stream:
        try:
            app_info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_info

def get_app_dep(app_info):
    app_dep = None
    if not app_info is None and APP_DEPENDENCY in app_info.keys():
        app_dep = app_info[APP_DEPENDENCY]
    return app_dep

def is_sdk_sample(sdk_base, app_path):
    return re.match(r'^' + re.escape(sdk_base) + r'/samples/', re.sub(r'\\', '/', app_path))

def get_soc_ip_feature_list(sdk_base, soc_series, soc_name):
    soc_ip_feature_file = os.path.join(sdk_base, "soc", soc_series, soc_name, SOC_IP_FEATURE_FILE_NAME)
    feature_list = []
    if os.path.exists(soc_ip_feature_file):
        with open(soc_ip_feature_file, 'r', encoding='utf-8', errors='ignore') as stream:
            for line in stream.readlines():
                regex_result = re.match(r'^#define\s+(%s.*)\s+1' % SOC_IP_FEATURE_PREFIX, line)
                if regex_result:
                    feature_list.append(regex_result.group(1))
        stream.close()
    return feature_list


def check_ip_dependency(sdk_base, soc_name, app_dependency):
    soc_series = get_board_info.get_soc_series_name(soc_name)
    # check ip dependency
    ip_list = get_board_info.get_soc_ip_list(soc_series, soc_name)

    feature_list = get_soc_ip_feature_list(sdk_base, soc_series, soc_name)
    for d in app_dep:
        if not re.match('ip_feature_', d):
            # process OR logic
            m = re.match(r'||', d)
            if not m is None:
                or_list = d.split('||')
                found = False
                for d in or_list:
                    if d.strip() in board_cap.split(get_board_info.BOARD_FEATURE_DELIM):
                        found = True
                        break
                    if len(ip_list) > 0:
                        if d.strip() in ip_list:
                            found = True
                            break
                if not found:
                    return False
            else:
                if not d.strip() in board_cap.split(get_board_info.BOARD_FEATURE_DELIM) and not d.strip() in ip_list:
                    return False
        else:
            if not ("HPM_%s" % d).upper() in feature_list:
                return False
    return True


if __name__ == "__main__":
    board_cap = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_FEATURE_KEY)
    board_excluded_samples = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_EXCLUDED_SAMPLES_KEY)
    soc_name = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_SOC_KEY)
    app_yml = os.path.realpath(sys.argv[2])

    if not os.path.exists(app_yml) or board_cap is None:
        sys.exit(0)

    sdk_base = os.path.realpath(os.environ["HPM_SDK_BASE"])
    app_path = os.path.realpath(app_yml)

    if is_sdk_sample(sdk_base, os.path.dirname(app_path)):
        try:
            app_name = os.path.dirname(os.path.relpath(app_path, sdk_base))
            if app_name in board_excluded_samples:
                sys.exit(1)
        except:
            # if application is not on the same drive with sdk_base, skip checking
            pass

    app_info = parse_app_yml(app_yml)

    # Check app dependency and IP dependency first
    app_dep = get_app_dep(app_info)
    if app_dep is None:
        sys.exit(0)

    if not check_ip_dependency(sdk_base, soc_name, app_dep):
        sys.exit(1)

    sys.exit(0)
