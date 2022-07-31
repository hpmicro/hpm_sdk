# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import re
import sys
import yaml
import subprocess

sys.path.append(os.path.dirname(sys.argv[0]))
import get_board_info

APP_DEPENDENCY="dependency"
APP_LINKED_PROJECT="linked_project"
APP_LINKED_PROJECT_NAME="project_name"
APP_LINKED_PROJECT_BUILD_TYPE="build_type"

def parse_app_yml(app_yml):
    app_info = None
    with open(app_yml, "r") as stream:
        try:
            app_info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_info

def get_app_dep(app_info):
    ip_list = ""
    app_dep = None
    if not app_info is None and APP_DEPENDENCY in app_info.keys():
        app_dep = app_info[APP_DEPENDENCY]
    return app_dep

def get_linked_project(app_info):
    linked_proj_info = None
    if not app_info is None and APP_LINKED_PROJECT in app_info.keys():
        linked_proj_info = app_info[APP_LINKED_PROJECT]
    return linked_proj_info


def build_linked_project(app_info,board_name):
    project_name=""
    build_type=""
    linked_proj_info = get_linked_project(app_info)
    if (linked_proj_info == None):
        return 0
    else:
        if APP_LINKED_PROJECT_NAME in linked_proj_info.keys():
            project_name = linked_proj_info[APP_LINKED_PROJECT_NAME]
        if APP_LINKED_PROJECT_BUILD_TYPE in linked_proj_info.keys():
            build_type = linked_proj_info[APP_LINKED_PROJECT_BUILD_TYPE]

        if (project_name != "" and build_type != ""):
            linked_proj_root_dir = os.path.join(os.getenv('HPM_SDK_BASE'), "samples", project_name)
            linked_proj_build_dir = os.path.join(linked_proj_root_dir, "build")
            if os.path.exists(linked_proj_build_dir):
                pass
            else:
               os.mkdir(linked_proj_build_dir)
            build_linked_proj_cmd = "cd " + linked_proj_build_dir
            build_linked_proj_cmd += " && " + "cmake -GNinja -DBOARD=" + board_name + " -DCMAKE_BUILD_TYPE="+build_type + " .."
            build_linked_proj_cmd += " && ninja"
            p = subprocess.Popen(build_linked_proj_cmd, shell=True,  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            retval = p.wait()
            return retval

if __name__ == "__main__":
    board_cap = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_FEATURE_KEY)
    board_name = get_board_info.get_info(sys.argv[1], get_board_info.BOARD_INFO_NAME_KEY)
    app_yml = sys.argv[2]

    if not os.path.exists(app_yml) or board_cap is None:
        sys.exit(0)

    app_info = parse_app_yml(app_yml)

    retval = build_linked_project(app_info, board_name)
    if (retval != 0):
        sys.exit(retval)

    app_dep = get_app_dep(app_info)
    if app_dep is None:
        sys.exit(0)

    for d in app_dep:
        # process OR logic
        m = re.match(r'||', d)
        if not m is None:
            or_list = d.split('||')
            found = False
            for d in or_list:
                if d.strip() in board_cap.split(get_board_info.BOARD_FEATURE_DELIM):
                    found = True
                    break
            if not found:
                sys.exit(1)
        else:
            if not d in board_cap.split(get_board_info.BOARD_FEATURE_DELIM):
                sys.exit(1)
    sys.exit(0)
