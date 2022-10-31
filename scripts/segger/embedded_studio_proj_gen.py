# Copyright 2021-2022 hpmicro
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

PROJECT_TEMPLATE = "embedded_studio_proj_tmpl.xml"
HPM_SDK_BASE = "$(HPM_SDK_BASE)"

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

def get_relpath(f, base):
    return re.sub(r'\\', r'/', os.path.relpath(f, base))

def get_sdk_fullpath(f, sdk_base):
    # replace real path with sdk base variable to be defined in SES
    return "/".join([HPM_SDK_BASE, get_relpath(f, sdk_base)])

def is_sdk_file(f, sdk_base):
    r = re.sub(re.escape(sdk_base), r'', f)
    return not (r == f)

def get_include_path(config, sdk_base, out_dir):
    l = ["$(StudioDir)/include"]
    for i in config["target"]["includes"].split(","):
        i = i.strip()
        if "gnu" in i or len(i) == 0:
            continue
        i = os.path.realpath(i)
        if is_sdk_file(i, sdk_base):
            inc_path = get_sdk_fullpath(i, sdk_base)
        else:
            inc_path = get_relpath(i, out_dir)
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

def populate_file_nodes(root, sdk_base, project_dir, out_dir, level = 1):
    node = ""
    for n in root.keys():
        if n == "files":
            for f in root[n]:
                if is_sdk_file(f, sdk_base):
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), get_sdk_fullpath(f, sdk_base))
                    obj = re.sub(re.escape(sdk_base + os.sep), r'', f)
                else:
                    node += "%s<file file_name=\"%s\">\n" % (" " * (level * 2), get_relpath(f, out_dir))
                    obj = re.sub(re.escape(project_dir), r'app', f)
                obj = re.sub(r'\\', r'/', obj)
                node += "%s<configuration Name=\"Common\" build_object_file_name=\"%s$(OBJ)\"/>\n" % (" " * ((level + 1) * 2), "/".join(["$(IntDir)", obj]))
                node += "%s</file>\n" % (" " * (level * 2))
        else:
            node += "%s<folder Name=\"%s\">\n" % (" " * (level * 2), n)
            node += populate_file_nodes(root[n], sdk_base, project_dir, out_dir, level + 1)
            node += "%s</folder>\n" % (" " * (level * 2))
    return node

def generate_file_structure(files, sdk_base, out_dir, project_dir):
    f_tree = {}
    # process all files
    for f in files:
        f = f.strip()
        if file_to_be_excluded(f):
            continue
        if len(f) == 0:
            continue
        f = os.path.realpath(f)
        m = re.sub(re.escape(project_dir), '', f)
        if m == f:
            # sdk source
            ses_file = re.sub(r'\\', r'/', get_sdk_fullpath(f, sdk_base))
        else:
            # app source
            m = re.sub(r'\\', r'/', m)
            ses_file = re.sub(r'^', r'application', m)
        update_file_tree(ses_file, f, f_tree)

    # generate ses project xml content
    nodes = populate_file_nodes(f_tree, sdk_base, project_dir, out_dir, level = 1)
    return nodes

def generate_ses_project(config, out_dir=".", project_dir = None):
    files = config["target"]["sources"].split(",")
    sdk_base = os.path.realpath(config["target"]["sdk_base"])
    out_dir = os.path.realpath(out_dir)
    project_dir = os.path.realpath(project_dir)

    config["target"]["includes"] = get_include_path(config, sdk_base, out_dir)
    config["target"]["defines"] = get_definitions(config["target"]["defines"])
    config["target"]["file_structure"] = generate_file_structure(files, sdk_base, out_dir, project_dir)
    config["target"]["link_symbols"] = get_link_symbols(config["target"]["link_symbols"])
    config["target"]["linker"] = get_sdk_fullpath(config["target"]["linker"], sdk_base)
    config["target"]["register_definition"] = get_sdk_fullpath(config["target"]["register_definition"], sdk_base)
    config["target"]["cpu_register_definition"] = get_sdk_fullpath(config["target"]["cpu_register_definition"], sdk_base)
    if config["target"]["ses_link_input"].strip():
        config["target"]["ses_link_input"] = get_sdk_fullpath(config["target"]["ses_link_input"], sdk_base)
    s = ""

    with open(PROJECT_TEMPLATE, "r") as f:
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
