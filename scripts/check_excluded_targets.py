# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

import sys
import yaml

EXCLUDED_TARGETS="excluded_targets"

def get_excluded_targets(input_yml):
    excluded = []
    with open(input_yml, "r") as stream:
        try:
            info = yaml.safe_load(stream)
            if not info is None and EXCLUDED_TARGETS in info.keys():
                for t in info[EXCLUDED_TARGETS]:
                    excluded.append(t.strip().lower())
        except yaml.YAMLError as e:
            pass
    stream.close()
    if len(excluded):
        sys.stdout.write("%s\n" % (";".join(excluded)))

    return excluded

def check_excluded_targets(input_yml, build_type):
    exc_tar = get_excluded_targets(input_yml)
    if exc_tar is not None:
        if build_type in exc_tar:
            sys.exit(1)
        else:
            sys.exit(0)
    else:
        sys.exit(0)

if __name__ == "__main__":
    get_excluded_targets(sys.argv[1])
