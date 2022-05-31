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
File = namedtuple("File", ["path"])
Group = namedtuple("Group", ["name", "files", "match_string"])

GROUP_TEMPLATES = [
    Group(name="app", files=[], match_string="/samples/"),
    Group(name="drivers", files=[], match_string="/drivers/"),
    Group(name="components", files=[], match_string="/components/"),
    Group(name="boards", files=[], match_string="/boards/"),
    Group(name="utils", files=[], match_string="/utils/"),
    Group(name="startup", files=[], match_string="/startup/"),
    Group(name="linker", files=[], match_string=".icf"),
    Group(name="toolchains", files=[], match_string="/toolchains/"),
    Group(name="soc", files=[], match_string="/soc/"), # icf files are located in soc, so make sure soc group will be created after linker
    Group(name="middleware", files=[], match_string="/middleware/")]

def unix_relative_path_get(path1, path2):
    rel_path = None
    # relpath only works with directory names
    if os.path.exists(path1) and os.path.exists(path2):
        try:
            rel_path = os.path.join(os.path.relpath(os.path.dirname(path1), path2), os.path.basename(path1))
        except ValueError:
            return path1.replace("\\", "/")
        return rel_path.replace("\\", "/")
    return rel_path

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

def create_file_groups(files, out_dir):
    other = Group(name="Other", files=[], match_string=None)
    groups = GROUP_TEMPLATES[:]
    for f in files:
        found_group = False
        f = f.strip()
        if file_to_be_excluded(f):
            continue
        if len(f) == 0:
            continue

        for g in groups:
            if g.match_string in f:
                f = unix_relative_path_get(f, out_dir)
                g.files.append(File(f))
                found_group = True
                break
        if not found_group:
            f = unix_relative_path_get(f, out_dir)
            other.files.append(File(f))

    groups.append(other)
    # Remove empty groups
    for g in groups[:]:
        if len(g.files) == 0:
            groups.remove(g)

    return groups

def get_includes(config, out_dir):
    l = ["$(StudioDir)/include"]
    for i in config["target"]["includes"].split(","):
        i = i.strip()
        if "gnu" in i or len(i) == 0:
            continue
        l.append(unix_relative_path_get(i, out_dir))
    return l

def get_defines(config):
    l = []
    for i in config["target"]["defines"].split(","):
        l.append(re.sub("\"", "&quot;", i))
    return l

def get_link_symbols(config):
    l = []
    for i in config["target"]["link_symbols"].split(","):
        l.append(re.sub("\"", "&quot;", i))
    return l

def generate_ses_project(config, out_dir="."):
    files = config["target"]["sources"].split(",")
    config["target"]["includes"] = get_includes(config, out_dir)
    config["target"]["defines"] = get_defines(config)
    config["target"]["groups"] = create_file_groups(files, out_dir)
    config["target"]["link_symbols"] = get_link_symbols(config)
    config["target"]["linker"] = unix_relative_path_get(config["target"]["linker"], out_dir)
    config["target"]["register_definition"] = unix_relative_path_get(config["target"]["register_definition"], out_dir)
    config["target"]["cpu_register_definition"] = unix_relative_path_get(config["target"]["cpu_register_definition"], out_dir)
    config["target"]["ses_link_input"] = unix_relative_path_get(config["target"]["ses_link_input"], out_dir)
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
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    config = load_config(input_file)
    ses_project = generate_ses_project(config, out_dir)

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
