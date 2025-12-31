# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import os
import re
import sys
import yaml
import subprocess
import shutil

sys.path.append(os.path.dirname(sys.argv[0]))
import get_board_info

APP_LINKED_PROJECT="linked_project"
APP_LINKED_PROJECT_NAME="project_name"
APP_LINKED_PROJECT_PATH="project_path"
APP_LINKED_PROJECT_BUILD_TYPE="build_type"

def parse_app_yml(app_yml):
    app_info = None
    with open(app_yml, "r", encoding='utf-8', errors='ignore') as stream:
        try:
            app_info = yaml.safe_load(stream)
        except yaml.YAMLError as e:
            pass
    stream.close()
    return app_info

def get_linked_project(app_info):
    linked_proj_info = None
    if not app_info is None and APP_LINKED_PROJECT in app_info.keys():
        linked_proj_info = app_info[APP_LINKED_PROJECT]
    return linked_proj_info

def is_sdk_board(sdk_base, board_dir):
    return re.match(r'^' + re.escape(sdk_base) + r'/boards/', re.sub(r'\\', '/', board_dir))

def get_compile_cmd_by_cmake_generator(cmake_generator):
    cmd = "ninja"
    if cmake_generator == "Ninja":
        cmd = "ninja"
    elif cmake_generator == "NMake Makefiles":
        cmd = "nmake"
    elif cmake_generator == "Unix Makefiles":
        cmd = "make"
    return cmd

def extract_linked_project_info(app_info, app_yml_base, sdk_base):
    """
    Extract linked project information from app_info

    Args:
        app_info: Parsed app.yml content
        app_yml_base: Base directory of the app.yml file
        sdk_base: SDK base directory

    Returns:
        tuple: (project_name, project_path, build_type, linked_proj_root_dir) or (None, None, None, None) if no linked project
    """
    linked_proj_info = get_linked_project(app_info)
    if linked_proj_info is None:
        return None, None, None, None

    project_name = ""
    project_path = ""
    build_type = ""
    linked_proj_root_dir = ""

    if APP_LINKED_PROJECT_NAME in linked_proj_info.keys():
        project_name = linked_proj_info[APP_LINKED_PROJECT_NAME]
    if APP_LINKED_PROJECT_PATH in linked_proj_info.keys():
        project_path = linked_proj_info[APP_LINKED_PROJECT_PATH]
    if APP_LINKED_PROJECT_BUILD_TYPE in linked_proj_info.keys():
        build_type = linked_proj_info[APP_LINKED_PROJECT_BUILD_TYPE]
    if APP_LINKED_PROJECT_PATH in linked_proj_info.keys():
        linked_proj_root_dir = linked_proj_info[APP_LINKED_PROJECT_PATH]
    else:
        linked_proj_root_dir = os.path.join(sdk_base, "samples")

    if project_path != "":
        if not os.path.isabs(project_path):
            p = os.path.realpath(os.path.join(app_yml_base, project_path))
        else:
            p = os.path.realpath(project_path)
        if os.path.exists(p):
            linked_proj_root_dir = p

    if (project_name != "" and build_type != ""):
        if linked_proj_root_dir == "":
            linked_proj_root_dir = os.path.join(sdk_base, "samples", project_name)
        return project_name, project_path, build_type, linked_proj_root_dir

    return None, None, None, None

def extract_sec_core_output_path(cmakelists_path):
    """
    Extract SEC_CORE_IMG_C_ARRAY_OUTPUT path from CMakeLists.txt

    Args:
        cmakelists_path: Path to the CMakeLists.txt file

    Returns:
        str: Output file path if found, None otherwise
    """
    if not os.path.exists(cmakelists_path):
        return None

    try:
        with open(cmakelists_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        # Look for SEC_CORE_IMG_C_ARRAY_OUTPUT pattern
        import re
        # Try both patterns: set(SEC_CORE_IMG_C_ARRAY_OUTPUT ...) and SEC_CORE_IMG_C_ARRAY_OUTPUT(...)
        pattern1 = r'set\s*\(\s*SEC_CORE_IMG_C_ARRAY_OUTPUT\s+([^)]+)\s*\)'
        pattern2 = r'SEC_CORE_IMG_C_ARRAY_OUTPUT\s*\(\s*["\']([^"\']+)["\']\s*\)'

        match = re.search(pattern1, content)
        if not match:
            match = re.search(pattern2, content)

        if match:
            output_path = match.group(1).strip()

            # Handle CMake variable expansion
            if '${CMAKE_CURRENT_SOURCE_DIR}' in output_path:
                # Replace ${CMAKE_CURRENT_SOURCE_DIR} with the actual directory path
                cmake_current_source_dir = os.path.dirname(cmakelists_path)
                output_path = output_path.replace('${CMAKE_CURRENT_SOURCE_DIR}', cmake_current_source_dir)

            return output_path
    except Exception as e:
        print(f"-- Warning: Failed to read CMakeLists.txt: {e}")

    return None

def generate_empty_file(file_path):
    """
    Generate a placeholder file with C error macro at the specified path

    Args:
        file_path: Path where to create the file
    """
    try:
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(file_path), exist_ok=True)

        # Create file with C error macro and warning message
        with open(file_path, 'w') as f:
            f.write("/*\n")
            f.write(" * WARNING: This file is automatically generated by build_linked_project.py\n")
            f.write(" * This is NOT the actual SEC_CORE_IMG file!\n")
            f.write(" * \n")
            f.write(" * To get the actual SEC_CORE_IMG, you need to complete the compilation\n")
            f.write(" * of the linked project by setting DISABLE_LINKED_PROJECT_BUILD=OFF\n")
            f.write(" */\n\n")
            f.write("#error \"SEC_CORE_IMG not available. Please compile the linked project to generate the actual SEC_CORE_IMG file.\"\n")

        print(f"-- Generated placeholder file with error macro: {file_path}")
        # Print warning message when placeholder file is generated
        print("-- WARNING: Linked project found but building is disabled. Placeholder file generated to complete project construction of current project.")
        return True
    except Exception as e:
        print(f"-- Failed to generate file {file_path}: {e}")
        return False

def build_linked_project(app_info, board_yaml, app_bin_dir, cmake_generator, app_yml, should_build_linked_project=True):
    # Get sdk_base from environment variable
    sdk_base = os.path.realpath(os.environ["HPM_SDK_BASE"])

    # Extract board_name and board_dir from board_yaml
    board_name = os.path.splitext(os.path.basename(board_yaml))[0]
    board_dir = os.path.dirname(board_yaml)

    # Derive app_yml_base from app_yml
    app_yml_base = os.path.dirname(app_yml)

    # Extract linked project information
    project_name, project_path, build_type, linked_proj_root_dir = extract_linked_project_info(app_info, app_yml_base, sdk_base)

    if project_name is None:
        return 0

    # Check if we should build the linked project
    if not should_build_linked_project:
        print('-- Skipping linked project build (DISABLE_LINKED_PROJECT_BUILD=ON)')

        # Try to extract output path from CMakeLists.txt and generate empty file
        cmakelists_path = os.path.join(linked_proj_root_dir, "CMakeLists.txt")
        output_path = extract_sec_core_output_path(cmakelists_path)

        if output_path:
            if not os.path.isabs(output_path):
                # Make path relative to linked project root
                output_path = os.path.join(linked_proj_root_dir, output_path)
            # Only generate empty file if it doesn't already exist
            if not os.path.exists(output_path):
                generate_empty_file(output_path)
        else:
            print("-- Warning: Could not find SEC_CORE_IMG_C_ARRAY_OUTPUT in CMakeLists.txt")

        return 0

    linked_proj_build_dir = os.path.join(linked_proj_root_dir, os.path.basename(app_bin_dir))
    if os.path.exists(linked_proj_build_dir):
        if sys.platform == 'win32':
            # add suffix '\\?\' before the long path to avoid removing failed in windows.
            shutil.rmtree(r"\\?\\" + linked_proj_build_dir)
        else:
            shutil.rmtree(linked_proj_build_dir)
    os.makedirs(linked_proj_build_dir, exist_ok=True)
    os.chdir(linked_proj_build_dir)
    extra_option = ""
    if not is_sdk_board(sdk_base, board_dir):
        extra_option = "-DBOARD_SEARCH_PATH=" + os.path.dirname(board_dir)

    print('-- Started to build linked project...')
    build_linked_proj_cmd = "cmake -G" + cmake_generator + " -DBOARD=" + board_name + " -DHPM_BUILD_TYPE=" + build_type + " " + extra_option + " -B " + linked_proj_build_dir + " -S " + linked_proj_root_dir
    build_linked_proj_cmd += " && " + get_compile_cmd_by_cmake_generator(cmake_generator)
    p = subprocess.Popen(build_linked_proj_cmd, shell=True,  stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    retval = p.wait()
    if retval != 0:
        print("-- Failed to build linked project!")
    else:
        print('-- Finished building linked project successfully!')
    return retval

if __name__ == "__main__":
    # Example usage of the build_linked_project function
    if len(sys.argv) < 5:
        print("Usage: python build_linked_project.py <app_yml> <board_yaml> <app_bin_dir> <cmake_generator> [should_build_linked_project]")
        print("  should_build_linked_project: Optional. Set to 'false' or '0' to skip linked project compilation (default: true)")
        sys.exit(1)

    app_yml = sys.argv[1]
    board_yaml = sys.argv[2]
    app_bin_dir = sys.argv[3]
    cmake_generator = sys.argv[4]

    # Parse optional should_build_linked_project parameter
    should_build_linked_project = True
    if len(sys.argv) > 5:
        should_build_linked_project_str = sys.argv[5].lower()
        if should_build_linked_project_str in ['false', '0', 'no']:
            should_build_linked_project = False

    app_info = parse_app_yml(app_yml)
    retval = build_linked_project(app_info, board_yaml, app_bin_dir, cmake_generator, app_yml, should_build_linked_project)
    sys.exit(retval)
