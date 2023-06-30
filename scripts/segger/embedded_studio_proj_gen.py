# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

#!/usr/bin/env python3

# Usage: python embedded_studio_proj_gen.py <target_configuration>.json <output_directory>
#
# <target_configuration>.json is a json file generated from CMake on the form:
# {
#     "target": {
#         "name": "hello_world",
#         "sources": "hello_world.c;..",
#         "includes": "include1;include2;..",
#         "definitions":"NDEBUG;FLASH_XIP=1;..",
#     },
#     "platform": {
#         "name": "yitian",
#         "arch": "rv32",
#         "flash_size": 524288,
#         "ram_size": 65536,
#     }
# }

import jinja2
import sys
import argparse
import json
import os
import re
from collections import namedtuple
from shutil import copyfile

HPM_SDK_BASE = "$(HPM_SDK_BASE)"
PROJECT_TEMPLATE = "embedded_studio_proj_tmpl.xml"

def load_config(input_file):
    with open(input_file, "r") as f:
        config = json.load(f)
    return config

def load_platform(platform_config):
    with open(platform_config["definition_file"], "r") as f:
        config = json.load(f)
    return [platform for platform in config["platforms"] if platform["name"] == platform_config["name"]][0]

def file_to_be_excluded(f):
    excluded_files = ["sbrk"]
    for e in excluded_files:
        if e in f:
            return True
    return False

def fix_path(p):
    return re.sub(r'\\', r'/', os.path.realpath(os.path.expanduser(p)))

def get_relpath(f, base):
    try:
        return os.path.relpath(f, base)
    except ValueError:
        return f

def get_sdk_fullpath(f, sdk_base):
    # replace real path with sdk base variable to be defined in SES
    return "/".join([HPM_SDK_BASE, get_relpath(f, sdk_base)])

def file_in_directory(f, directory):
    tmp_f = re.sub(r'-', r'_HpM_', f)
    tmp_dir = re.sub(r'-', r'_HpM_', directory)
    r = re.sub(re.escape(tmp_dir), r'', tmp_f)
    return not (r == tmp_f)

def is_sdk_file(f, sdk_base):
    return file_in_directory(f, sdk_base)

def is_sdk_sample_file(f):
    return f.find("samples") > 0

def get_include_path(config, sdk_base, out_dir):
    l = ["$(StudioDir)/include"]
    for d in config["target"]["includes"].split(","):
        d = d.strip()
        if len(d) == 0:
            continue
        d = fix_path(d)
        if is_sdk_file(d, sdk_base):
            inc_path = get_sdk_fullpath(d, sdk_base)
        else:
            inc_path = get_relpath(d, out_dir)
        inc_path = re.sub(r'\\', '/', inc_path)
        l.append(inc_path)
    return l

def get_definitions(definitions):
    l = []
    for i in definitions.split(","):
        l.append(re.sub("\"", "&quot;", i))
    return l

def get_link_symbols(symbols):
    l = []
    for i in symbols.split(","):
        l.append(re.sub("\"", "&quot;", i))
    return l

def tree_insert_dirs(ses_dir, ses_file, f, tree):
    dirs = ses_dir.split('/')
    if len(dirs) == 1:
        d = dirs[0]
        if not d in tree.keys():
            tree[d] = {}
        if not "files" in tree[d].keys():
            tree[d]["files"] = []
        tree[d]["files"].append(f)
        return
    d = dirs[0]
    if not d in tree.keys():
        tree[d] = {}
    ses_dir = re.sub(re.escape(d + r'/'), '', ses_dir)
    tree_insert_dirs(ses_dir, ses_file, f, tree[d])

def update_file_tree(ses_file, f, tree):
    ses_dir, file = os.path.split(ses_file)
    ses_dir = re.sub(re.escape("%s" % HPM_SDK_BASE) + r'/', '', ses_dir)
    tree_insert_dirs(ses_dir, ses_file, f, tree)

def populate_file_nodes(root, sdk_base, project_dir, custom_board_dir, out_dir, level = 1):
    node = ""
    for n in root.keys():
        if n == "files":
            for f in root[n]:
                if is_sdk_file(f, sdk_base):
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), get_sdk_fullpath(f, sdk_base))
                    obj = re.sub(re.escape(sdk_base + '/'), r'', f)
                elif custom_board_dir is not None and is_custom_board_file(f, custom_board_dir):
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), get_relpath(f, out_dir))
                    f = re.sub(r'^\w:', '', f) # remove windows drive letter
                    t = f.split(custom_board_dir)[1] # get any characters after custom board name
                    t = re.sub(r'^\W', '', t) # remove leading path separator
                    obj = os.path.join(custom_board_dir, t)
                else:
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), get_relpath(f, out_dir))
                    obj = re.sub(re.escape(project_dir), r'app', f)
                    if obj == f:
                        # deal with drive letter of Windows path
                        obj = re.sub(r'^\w:', '', f)
                obj = re.sub(r'\\', r'/', obj)
                node += "%s<configuration Name=\"Common\" build_object_file_name=\"%s$(OBJ)\"/>\n" % (" " * ((level + 1) * 2), "/".join(["$(IntDir)", obj]))
                node += "%s</file>\n" % (" " * (level * 2))
        else:
            node += "%s<folder Name=\"%s\">\n" % (" " * (level * 2), n)
            node += populate_file_nodes(root[n], sdk_base, project_dir, custom_board_dir, out_dir, level + 1)
            node += "%s</folder>\n" % (" " * (level * 2))
    return node

def get_app_common_path(file_full_path, project_dir):
    app_common_path = os.path.commonprefix([project_dir, file_full_path])
    if len(app_common_path) == 0:
        app_common_path = project_dir
    # remove tailing "/"
    return re.sub(r'/$', '', app_common_path)

def is_custom_board_file(f, board_dir):
    return file_in_directory(f, board_dir)

def generate_file_structure(files, sdk_base, out_dir, project_dir, board_dir, board_name):
    f_tree = {}
    app_common_dir = None
    custom_board_dir = None
    # process all files
    for f in files:
        f = f.strip()
        if len(f) == 0:
            continue
        if file_to_be_excluded(f):
            continue
        f = fix_path(f)
        if is_sdk_file(f, sdk_base):
            # sdk source
            ses_file = re.sub(r'\\', r'/', get_sdk_fullpath(f, sdk_base))
            if is_sdk_sample_file(f):
                t = os.path.join(HPM_SDK_BASE, "samples")
                t = re.sub(r'\\', r'/', t)  # convert windows path separator
                ses_file = re.sub(re.escape(t), 'sdk_sample', ses_file)
        elif is_custom_board_file(f, board_dir):
            if custom_board_dir is None:
                custom_board_dir = board_name
            ses_file = re.sub(board_dir, "boards/" + board_name, f)
        else:
            # app source
            if app_common_dir is None:
                d = get_app_common_path(f, project_dir)
                if d != project_dir:
                    app_common_dir = d
                else:
                    app_common_dir = project_dir
            m = re.sub(re.escape(app_common_dir), '', f)
            # deal with drive letter of Windows path
            m = re.sub(r'^\w:', '', m)
            ses_file = re.sub(r'^', r'application', m)
        update_file_tree(ses_file, f, f_tree)

    # generate ses project xml content
    nodes = populate_file_nodes(f_tree, sdk_base, app_common_dir, custom_board_dir, out_dir, level = 1)
    return nodes

def get_gcc_opt_level(gcc_level):
    if gcc_level == "-O2":
        opt_level_string = "Level 2 balanced"
    elif gcc_level == "-O3":
        opt_level_string = "Level 3 for more speed"
    elif gcc_level == "-O1":
        opt_level_string = "Level 1"
    elif gcc_level == "-O0":
        opt_level_string = "Level 0"
    else:
        opt_level_string = "None"
    return opt_level_string

def get_segger_rtl_linker_symbols(printf, scanf):
    symbols = ""
    prefix = ""
    print_float = False
    scan_float = False

    if printf["fp"]["value"] in ("Float", "Double"):
        if printf["fp"]["value"] == "Float":
            prefix = "__SEGGER_RTL_vfprintf=__SEGGER_RTL_vfprintf_short_float"
        elif printf["fp"]["value"] == "Double":
            prefix = "__SEGGER_RTL_vfprintf=__SEGGER_RTL_vfprintf_float"
        print_float = True
    else:
        prefix = "__SEGGER_RTL_vfprintf=__SEGGER_RTL_vfprintf"

    printf["fmt_level"]["value"] = re.sub(r" ", r"_", printf["fmt_level"]["value"].strip())
    if printf["fmt_level"]["value"] in ("long", "long_long"):
        prefix = "%s_%s" % (prefix, printf["fmt_level"]["value"])
    elif print_float:
        prefix = "%s_long" % (prefix)
    elif not print_float:
        prefix = "%s_int" % (prefix)

    if printf["width_precision"]["value"] in ("No") and not print_float:
        prefix = "%s_nwp" % (prefix)

    if printf["wchar"]["value"] in ("Yes"):
        prefix = "%s_wchar" % (prefix)

    symbols += "%s;" % (prefix)

    if scanf["fp"]["value"] in ("Yes"):
        prefix = "__SEGGER_RTL_vfscanf=__SEGGER_RTL_vfscanf_float"
        scan_float = True
    else:
        prefix = "__SEGGER_RTL_vfscanf=__SEGGER_RTL_vfscanf"

    scanf["fmt_level"]["value"] = re.sub(r" ", r"_", scanf["fmt_level"]["value"].strip())
    if scanf["fmt_level"]["value"] in ("long", "long_long"):
        prefix = "%s_%s" % (prefix, scanf["fmt_level"]["value"])
    elif scan_float:
        prefix = "%s_long" % (prefix)
    elif not scan_float:
        prefix = "%s_int" % (prefix)

    if scanf["cc"]["value"] in ("Yes"):
        symbols += "%s_cc;" % (prefix)
    else:
        symbols += "%s;" % (prefix)

    return symbols

def init_printf_scanf_properties():
    printf = {}
    scanf = {}
    printf["fp"] = {}
    printf["fp"]["name"] = "linker_printf_fp_enabled"
    printf["fp"]["value"] = "Float"
    printf["fmt_level"] = {}
    printf["fmt_level"]["name"] = "linker_printf_fmt_level"
    printf["fmt_level"]["value"] = "int"
    printf["wchar"] = {}
    printf["wchar"]["name"] = "linker_printf_wchar_enabled"
    printf["wchar"]["value"] = "No"
    printf["width_precision"] = {}
    printf["width_precision"]["name"] = "linker_printf_width_precision_supported"
    printf["width_precision"]["value"] = "Yes"

    scanf["fp"] = {}
    scanf["fp"]["name"] = "linker_scanf_fp_enabled"
    scanf["fp"]["value"] = "No"
    scanf["fmt_level"] = {}
    scanf["fmt_level"]["name"] = "linker_scanf_fmt_level"
    scanf["fmt_level"]["value"] = "int"
    scanf["cc"] = {}
    scanf["cc"]["name"] = "linker_scanf_character_group_matching_enabled"
    scanf["cc"]["value"] = "No"
    return printf, scanf

def update_printf_scanf_properties(printf, scanf, name, value):
    if name == printf["fp"]["name"]:
        printf["fp"]["value"] = value
    elif name == printf["fmt_level"]["name"]:
        printf["fmt_level"]["value"] = value
    elif name == printf["wchar"]["name"]:
        printf["wchar"]["value"] = value
    elif name == printf["width_precision"]["name"]:
        printf["width_precision"]["value"] = value
    elif name == scanf["fp"]["name"]:
        scanf["fp"]["value"] = value
    elif name == scanf["fmt_level"]["name"]:
        scanf["fmt_level"]["value"] = value
    elif name == scanf["cc"]["name"]:
        scanf["cc"]["value"] = value

def get_all_printf_scanf_opt_names(printf, scanf):
    opt_names = []
    for i in printf.keys():
        opt_names.append(printf[i]["name"])
    for i in scanf.keys():
        opt_names.append(scanf[i]["name"])
    return opt_names

def popluate_printf_scanf_opts(printf, scanf, opts):
    for i in printf.keys():
        formatted = "%s=\"%s\"" % (printf[i]["name"], printf[i]["value"])
        opts.append(formatted)
    for i in scanf.keys():
        formatted = "%s=\"%s\"" % (scanf[i]["name"], scanf[i]["value"])
        opts.append(formatted)

def process_extra_options(config):
    opts = []
    printf, scanf = init_printf_scanf_properties()
    printf_scanf_opt_names = get_all_printf_scanf_opt_names(printf, scanf)
    for o in config["target"]["extra_ses_options"].split(","):
        o = o.strip()
        if not len(o):
            continue
        opt_name = re.sub(r"(.*)=.*", r'\1', o).strip()
        opt_val = re.sub(r".*=(.*)", r'\1', o).strip()
        if opt_name in printf_scanf_opt_names:
            # printf/scanf related properties will be popluated later with default values
            update_printf_scanf_properties(printf, scanf, opt_name, opt_val)
        else:
            formatted = "%s=\"%s\"" % (opt_name, opt_val)
            opts.append(formatted)

    popluate_printf_scanf_opts(printf, scanf, opts)

    config["target"]["extra_ses_options"] = opts
    config["target"]["segger_rtl_linker_symbols"] = get_segger_rtl_linker_symbols(printf, scanf)

def generate_ses_project(config, out_dir=".", project_dir = None):
    files = config["target"]["sources"].split(",")
    sdk_base = fix_path(config["target"]["sdk_base"])
    out_dir = fix_path(out_dir)
    project_dir = fix_path(project_dir)
    board_dir = fix_path(config["target"]["board_dir"])
    board_name = config["target"]["board"]

    config["target"]["includes"] = get_include_path(config, sdk_base, out_dir)
    config["target"]["defines"] = get_definitions(config["target"]["defines"])
    config["target"]["file_structure"] = generate_file_structure(files, sdk_base, out_dir, project_dir, board_dir, board_name)
    config["target"]["link_symbols"] = get_link_symbols(config["target"]["link_symbols"])
    config["target"]["linker"] = get_sdk_fullpath(config["target"]["linker"], sdk_base)
    config["target"]["register_definition"] = get_sdk_fullpath(config["target"]["register_definition"], sdk_base)
    config["target"]["cpu_register_definition"] = get_sdk_fullpath(config["target"]["cpu_register_definition"], sdk_base)
    config["target"]["gcc_opt_level"] = get_gcc_opt_level(config["target"]["gcc_opt_level"].strip())
    if config["target"]["ses_link_input"].strip():
        config["target"]["ses_link_input"] = get_sdk_fullpath(config["target"]["ses_link_input"], sdk_base)
    s = ""
    process_extra_options(config)

    with open(os.path.join(sdk_base, "scripts", "segger", PROJECT_TEMPLATE), "r") as f:
        s = f.read()

    t = jinja2.Template(s)
    s = t.render(config)

    return s

def generate_ses_session(out_dir):
    session_file_contents = ['<!DOCTYPE CrossStudio_Session_File>',
                             '<session>',
                             '\t<Files>',
                             '\t\t<SessionOpenFile path="{}"/>',
                             '\t</Files>',
                             '</session>']
    return '\n'.join(session_file_contents)

def main():
    input_file = sys.argv[1]
    out_dir = sys.argv[2]
    project_dir = sys.argv[3]
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    config = load_config(input_file)
    ses_project = generate_ses_project(config, out_dir, project_dir)

    out_dir += "/"
    output_filename = out_dir + config["target"]["name"].replace(".", "_")
    project_file = output_filename + ".emProject"
    with open(project_file, "w") as f:
        f.write(ses_project)

    ses_session = generate_ses_session(out_dir)
    session_file = output_filename + ".emSession"
    with open(session_file, "w") as f:
        f.write(ses_session)

    print("-- Segger Embedded Studio Project: " + project_file)

if __name__ == "__main__":
    main()
