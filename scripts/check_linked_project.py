# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys
import yaml

APP_LINKED_PROJECT="linked_project"
APP_LINKED_PROJECT_NAME="project_name"
APP_LINKED_PROJECT_PATH="project_path"
APP_LINKED_PROJECT_BUILD_TYPE="build_type"

def parse_app_yml(app_yml):
    """Parse the app.yaml file and return the parsed content"""
    app_info = None
    with open(app_yml, "r", encoding='utf-8', errors='ignore') as stream:
        try:
            app_info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_info

def get_linked_project(app_info):
    """Extract linked project information from app_info"""
    linked_proj_info = None
    if not app_info is None and APP_LINKED_PROJECT in app_info.keys():
        linked_proj_info = app_info[APP_LINKED_PROJECT]
    return linked_proj_info

def check_linked_project(app_yml):
    """Check if app.yaml contains linked project configuration"""
    app_info = parse_app_yml(app_yml)
    linked_proj_info = get_linked_project(app_info)

    if linked_proj_info is None:
        return False

    # Check if linked project has required fields
    has_project_name = APP_LINKED_PROJECT_NAME in linked_proj_info.keys()
    has_build_type = APP_LINKED_PROJECT_BUILD_TYPE in linked_proj_info.keys()

    # Return True if both project_name and build_type are present
    return has_project_name and has_build_type

def get_linked_project_info(app_yml):
    """Get detailed linked project information"""
    app_info = parse_app_yml(app_yml)
    linked_proj_info = get_linked_project(app_info)

    if linked_proj_info is None:
        return None

    info = {}
    if APP_LINKED_PROJECT_NAME in linked_proj_info.keys():
        info['project_name'] = linked_proj_info[APP_LINKED_PROJECT_NAME]
    if APP_LINKED_PROJECT_PATH in linked_proj_info.keys():
        info['project_path'] = linked_proj_info[APP_LINKED_PROJECT_PATH]
    if APP_LINKED_PROJECT_BUILD_TYPE in linked_proj_info.keys():
        info['build_type'] = linked_proj_info[APP_LINKED_PROJECT_BUILD_TYPE]

    return info

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python check_linked_project.py <app_yml> [--info]")
        print("  --info: Show detailed linked project information")
        sys.exit(1)

    app_yml = sys.argv[1]
    show_info = len(sys.argv) > 2 and sys.argv[2] == "--info"

    # If app.yaml doesn't exist, return no linked project
    if not os.path.exists(app_yml):
        if show_info:
            print("No linked project configuration found")
        sys.exit(1)  # No linked project

    has_linked_project = check_linked_project(app_yml)

    if show_info:
        if has_linked_project:
            info = get_linked_project_info(app_yml)
            print("Linked project found:")
            for key, value in info.items():
                print(f"  {key}: {value}")
        else:
            print("No linked project configuration found")
        sys.exit(0)
    else:
        # Simple check - use exit code to indicate linked project existence
        if has_linked_project:
            sys.exit(0)  # Has linked project
        else:
            sys.exit(1)  # No linked project

