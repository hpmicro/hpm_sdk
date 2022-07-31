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
File = namedtuple("File", ["path", "obj_path"])
Group = namedtuple("Group", ["name", "files", "match_string"])

GROUP_TEMPLATES = [
    Group(name="drivers", files=[], match_string='\$\(HPM_SDK_BASE\)\/drivers'),
    Group(name="components", files=[], match_string="\$\(HPM_SDK_BASE\)\/components/"),
    Group(name="boards", files=[], match_string="\$\(HPM_SDK_BASE\)\/boards/"),
    Group(name="utils", files=[], match_string="\$\(HPM_SDK_BASE\)\/utils/"),
    Group(name="startup", files=[], match_string="\$\(HPM_SDK_BASE\)\/startup/"),
    Group(name="linker", files=[], match_string=".icf"),
    Group(name="toolchains", files=[], match_string="\$\(HPM_SDK_BASE\)\/toolchains/"),
    Group(name="soc", files=[], match_string="\$\(HPM_SDK_BASE\)\/soc/"), # icf files are located in soc, so make sure soc group will be created after linker
    Group(name="middleware", files=[], match_string="\$\(HPM_SDK_BASE\)\/middleware/")]

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

def get_relpath(f, sdk_base):
    s = os.path.relpath(os.path.realpath(f), os.path.realpath(sdk_base))
    return re.sub(r'\\', '/', s)

def get_sdk_fullpath(f, sdk_base):
    return re.sub(r'^', '$(HPM_SDK_BASE)/', get_relpath(f, sdk_base))

def add_sdk_file_to_group(group, f, sdk_base):
    s = re.sub(r'^', '$(IntDir)/', get_relpath(f, sdk_base))
    group.files.append(File(get_sdk_fullpath(f, sdk_base), s))


def add_app_file_to_group(group, f, base_path):
    # if app source file is not located under project dir, there'll be couple of
    # leading '../'. the object file can use the name without these leading '../'
    s = re.sub(r'^(../)+', '', get_relpath(f, base_path))
    s = re.sub(r'^', r'$(IntDir)/app/', s)
    group.files.append(File(f, s))

def create_file_groups(files, sdk_base, project_dir):
    app = Group(name="app", files=[], match_string=None)
    groups = GROUP_TEMPLATES[:]
    for f in files:
        found_group = False
        f = f.strip()
        if file_to_be_excluded(f):
            continue
        if len(f) == 0:
            continue

        for g in groups:
            new_path = get_sdk_fullpath(f, sdk_base)
            m = re.search(g.match_string, new_path)
            if not m is None:
                add_sdk_file_to_group(g, f, sdk_base)
                found_group = True
                break
        if not found_group:
            add_app_file_to_group(app, f, project_dir)

    groups.append(app)
    # Remove empty groups
    for g in groups[:]:
        if len(g.files) == 0:
            groups.remove(g)

    return groups

def get_include_path(config, sdk_base):
    l = ["$(StudioDir)/include"]
    for i in config["target"]["includes"].split(","):
        i = i.strip()
        if "gnu" in i or len(i) == 0:
            continue
        file_sdk_path = get_sdk_fullpath(i, sdk_base)
        l.append(file_sdk_path)
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

def generate_ses_project(config, out_dir=".", project_dir = None):
    files = config["target"]["sources"].split(",")
    sdk_base = config["target"]["sdk_base"]
    config["target"]["includes"] = get_include_path(config, sdk_base)
    config["target"]["defines"] = get_definitions(config["target"]["defines"])
    config["target"]["groups"] = create_file_groups(files, sdk_base, project_dir)
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
