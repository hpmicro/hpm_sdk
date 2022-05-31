# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
import yaml
import subprocess

sys.path.append(os.path.dirname(sys.argv[0]))
import get_board_info

APP_DEPENDENCY="dependency"
APP_LINKED_PROJECT="linked_project"
APP_LINKED_PROJECT_NAME="project_name"
APP_LINKED_PROJECT_BUILD_TYPE="build_type"

def get_app_dep(app_yml):
    ip_list = ""
    app_dep = None
    with open(app_yml, "r") as stream:
        try:
            app_info = yaml.safe_load(stream)
            if APP_DEPENDENCY in app_info.keys():
                app_dep = app_info[APP_DEPENDENCY]
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_dep

def get_linked_project(app_yml):
    linked_proj_info = None
    with open(app_yml, "r") as stream:
        try:
            app_info = yaml.safe_load(stream)
            if APP_LINKED_PROJECT in app_info.keys():
                linked_proj_info = app_info[APP_LINKED_PROJECT]
        except yaml.YAMLError as e:
            pass
    stream.close()
    return linked_proj_info


def build_linked_project(app_yml,board_name):
    project_name=""
    build_type=""
    linked_proj_info = get_linked_project(app_yml)
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
    retval = build_linked_project(sys.argv[2], board_name)
    if (retval != 0):
        sys.exit(retval)
    if not os.path.exists(sys.argv[2]) or board_cap is None:
        sys.exit(0)
    app_dep = get_app_dep(sys.argv[2])
    if app_dep is None:
        sys.exit(0)
    for d in app_dep:
        if not d in board_cap.split(get_board_info.BOARD_FEATURE_DELIM):
            sys.exit(1)
    sys.exit(0)
