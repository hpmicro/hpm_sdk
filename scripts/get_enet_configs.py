#!/usr/bin/env python3
# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
#
# Helper for build_all.sh: read ENET/LWIP matrix from .gitlab_ci_enet_configs.yaml
# and resolve per-board ENET ids, LWIP ids, and cmake args for samples.
# Usage:
#   get_enet_configs.py --board <board>           # print ENET config ids, one per line
#   get_enet_configs.py --sample-has-enet <sample> # exit 0 if sample has enet deps, else 1
#   get_enet_configs.py --sample-has-lwip-dhcp <sample> # exit 0 if sample has LWIP_DHCP in CMakeLists
#   get_enet_configs.py --lwip-configs            # print LWIP config ids (static, dhcp)
#   get_enet_configs.py --config-cmake BOARD CONFIG_ID  # print cmake -D args for ENET config
#   get_enet_configs.py --config-cmake-lwip <id>  # print cmake args for LWIP config
#   get_enet_configs.py --preprocess-sample-filter p1 [p2 ...]
#       Preprocess SAMPLE_FILTER patterns before passing to build_all.sh.
#       - "dep:<keyword>": expand to all samples depending on <keyword> (full coverage)
#       - other patterns: pass through unchanged

import argparse
import os
import sys
import yaml

ENET_DEPS = ("enet", "board_enet_phy")
DUAL_PHY_DEP = "board_enet_dual_phy"  # dual port: skip matrix, use default
CONFIG_YAML = ".gitlab_ci_enet_configs.yaml"
LWIP_CONFIGS_KEY = "lwip_configs"


def get_board_enet_configs(sdk_base, board):
    path = os.path.join(sdk_base, CONFIG_YAML)
    if not os.path.exists(path):
        return [{"id": "def", "cmake": []}]
    with open(path, "r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
    if not data or board not in data:
        return [{"id": "def", "cmake": []}]
    return data[board]


def sample_has_enet_dep(sdk_base, sample):
    """Return True if sample has enet/board_enet_phy (single port)."""
    app_yaml = os.path.join(sdk_base, "samples", sample, "app.yaml")
    if not os.path.exists(app_yaml):
        return False
    with open(app_yaml, "r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
    if not data or "dependency" not in data:
        return False
    deps = data["dependency"]
    if isinstance(deps, list):
        if DUAL_PHY_DEP in deps:
            return False  # dual port: no matrix iteration
        return any(d in ENET_DEPS for d in deps)
    return deps in ENET_DEPS and deps != DUAL_PHY_DEP


def get_config_cmake_args(sdk_base, board, config_id):
    """Return cmake args string for a config, e.g. '-DAPP_USE_ENET_ITF_RGMII=1'."""
    configs = get_board_enet_configs(sdk_base, board)
    for c in configs:
        if c["id"] == config_id:
            return " ".join("-D%s" % v for v in c.get("cmake", []))
    return ""


def get_lwip_configs(sdk_base):
    """Return LWIP config list from YAML (static, dhcp)."""
    path = os.path.join(sdk_base, CONFIG_YAML)
    if not os.path.exists(path):
        return [{"id": "static", "cmake": ["LWIP_DHCP=0"]}]
    with open(path, "r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
    if not data or LWIP_CONFIGS_KEY not in data:
        return [{"id": "static", "cmake": ["LWIP_DHCP=0"]}]
    return data[LWIP_CONFIGS_KEY]


def sample_has_lwip_dhcp_toggle(sdk_base, sample):
    """Return True if sample CMakeLists.txt contains LWIP_DHCP (supports DHCP toggle).
    WiFi samples (path contains 'wifi') are excluded."""
    if "wifi" in sample.lower():
        return False
    cmake_path = os.path.join(sdk_base, "samples", sample, "CMakeLists.txt")
    if not os.path.exists(cmake_path):
        return False
    with open(cmake_path, "r", encoding="utf-8") as f:
        return "LWIP_DHCP" in f.read()


def get_lwip_config_cmake_args(sdk_base, config_id):
    """Return cmake args string for LWIP config, e.g. '-DLWIP_DHCP=1'."""
    configs = get_lwip_configs(sdk_base)
    for c in configs:
        if c["id"] == config_id:
            return " ".join("-D%s" % v for v in c.get("cmake", []))
    return ""


def _get_all_samples(sdk_base):
    """Return sorted list of all sample relative paths (under samples/)."""
    import re
    samples_dir = os.path.join(sdk_base, "samples")
    result = []
    for root, _dirs, files in os.walk(samples_dir):
        if "CMakeLists.txt" in files:
            cmake_path = os.path.join(root, "CMakeLists.txt")
            with open(cmake_path, "r", encoding="utf-8") as f:
                content = f.read()
            if re.search(r"^\s*project\s*\(\s*[a-zA-Z0-9_]", content, re.MULTILINE):
                rel = os.path.relpath(root, samples_dir).replace("\\", "/")
                result.append(rel)
    result.sort()
    return result


def _sample_has_dep_keyword(sdk_base, sample, keyword):
    """Return True if sample's app.yaml dependency list contains keyword."""
    app_yaml = os.path.join(sdk_base, "samples", sample, "app.yaml")
    if not os.path.exists(app_yaml):
        return False
    with open(app_yaml, "r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
    if not data or "dependency" not in data:
        return False
    deps = data["dependency"]
    if isinstance(deps, list):
        return any(keyword in d for d in deps)
    return keyword in str(deps)


def _sample_cmake_references(sdk_base, sample, keyword):
    """Return True if sample's CMakeLists.txt references keyword (case-insensitive).

    Checks for CONFIG_<KEYWORD> or any occurrence of keyword in CMakeLists.txt.
    This catches set(CONFIG_LWIP 1), sdk_compile_definitions(-DLWIP_...), etc.
    """
    cmake_path = os.path.join(sdk_base, "samples", sample, "CMakeLists.txt")
    if not os.path.exists(cmake_path):
        return False
    with open(cmake_path, "r", encoding="utf-8") as f:
        content = f.read().lower()
    return keyword.lower() in content


def find_samples_by_dep(sdk_base, keyword):
    """Find all samples depending on keyword.

    A sample matches if any of the following is true:
      1. its path contains keyword
      2. its app.yaml dependency list contains keyword
      3. its CMakeLists.txt references keyword (case-insensitive)
    No exclusions applied — intended for full coverage testing (e.g. lwip upgrade).
    """
    all_samples = _get_all_samples(sdk_base)
    result = []
    for s in all_samples:
        if (keyword in s
                or _sample_has_dep_keyword(sdk_base, s, keyword)
                or _sample_cmake_references(sdk_base, s, keyword)):
            result.append(s)
    return result


def preprocess_sample_filter(sdk_base, patterns):
    """Preprocess SAMPLE_FILTER patterns.

    - "dep:<keyword>": expand to all samples depending on <keyword> (full coverage)
    - other patterns: pass through unchanged
    """
    result = []
    seen = set()
    for pat in patterns:
        if pat.startswith("dep:"):
            keyword = pat[4:]
            for s in find_samples_by_dep(sdk_base, keyword):
                if s not in seen:
                    seen.add(s)
                    result.append(s)
        else:
            if pat not in seen:
                seen.add(pat)
                result.append(pat)
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--board", help="Board name")
    parser.add_argument("--sample-has-enet", help="Sample path (e.g. lwip/lwip_tcpecho)")
    parser.add_argument("--sample-has-lwip-dhcp", help="Sample path; exit 0 if has LWIP_DHCP in CMakeLists")
    parser.add_argument("--lwip-configs", action="store_true", help="Print LWIP config ids (static, dhcp)")
    parser.add_argument("--config-cmake", nargs=2, metavar=("BOARD", "CONFIG_ID"))
    parser.add_argument("--config-cmake-lwip", metavar="CONFIG_ID", help="Get cmake args for LWIP config")
    parser.add_argument(
        "--preprocess-sample-filter",
        nargs="*",
        default=None,
        metavar="PATTERN",
        help="Expand dep:<keyword> patterns for SAMPLE_FILTER; other patterns pass through",
    )
    parser.add_argument("--sdk-base", default=os.environ.get("HPM_SDK_BASE", "."))
    args = parser.parse_args()

    sdk_base = os.path.abspath(args.sdk_base)

    if args.preprocess_sample_filter is not None:
        filtered = preprocess_sample_filter(sdk_base, args.preprocess_sample_filter)
        for s in filtered:
            print(s)
        sys.exit(0)

    if args.sample_has_enet:
        sys.exit(0 if sample_has_enet_dep(sdk_base, args.sample_has_enet) else 1)

    if args.sample_has_lwip_dhcp:
        sys.exit(0 if sample_has_lwip_dhcp_toggle(sdk_base, args.sample_has_lwip_dhcp) else 1)

    if args.lwip_configs:
        for c in get_lwip_configs(sdk_base):
            print(c["id"])
        sys.exit(0)

    if args.config_cmake:
        print(get_config_cmake_args(sdk_base, args.config_cmake[0], args.config_cmake[1]))
        sys.exit(0)

    if args.config_cmake_lwip:
        print(get_lwip_config_cmake_args(sdk_base, args.config_cmake_lwip))
        sys.exit(0)

    if args.board:
        configs = get_board_enet_configs(sdk_base, args.board)
        for c in configs:
            print(c["id"])
        sys.exit(0)

    parser.print_help()
    sys.exit(1)


if __name__ == "__main__":
    main()
