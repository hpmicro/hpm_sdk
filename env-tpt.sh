#
# Copyright (c) 2021 hpmicro
#
# SPDX-License-Identifier: BSD-3-Clause
#

#Terapines
#export GNURISCV_TOOLCHAIN_PATH=/opt/zcc-toolchain/3.2.1
#export HPM_SDK_TOOLCHAIN_VARIANT=zcc
#export HPM_SDK_BASE=$HOME/work/code/git/hpm_sdk

#export GNURISCV_TOOLCHAIN_PATH=/opt/zcc
#export HPM_SDK_TOOLCHAIN_VARIANT=zcc
#export HPM_SDK_BASE=$HOME/work/code/git/hpm_sdk

#export GNURISCV_TOOLCHAIN_PATH=/opt/zcc-pro-u22
#export HPM_SDK_TOOLCHAIN_VARIANT=zcc
#export HPM_SDK_BASE=$HOME/work/code/git/hpm_sdk

export GNURISCV_TOOLCHAIN_PATH=/home/crux/workspace/test_suit/install-zcc
export HPM_SDK_TOOLCHAIN_VARIANT=zcc

#Andes
#export GNURISCV_TOOLCHAIN_PATH=/opt/andes-toolchain/v520/nds32le-elf-newlib-v5d
#export HPM_SDK_TOOLCHAIN_VARIANT=nds-gcc
#export HPM_SDK_BASE=$HOME/work/code/git/hpm_sdk

if [ "X$MSYSTEM" "==" "X" ]; then
    if [ "X$name" "==" "Xenv.sh" ]; then
        echo "Please source this file, rather than executing it."
        exit
    fi
    env_name=$0
else
    env_name=$1
fi

script=$(cd -P -- "$(dirname -- "$env_name")" && printf '%s\n' "$(pwd -P)/$(basename -- "$env_name")")
if [ "X$MSYSTEM" "==" "X" ]; then
    export HPM_SDK_BASE=$(dirname "$script")
else
    export HPM_SDK_BASE=$script
fi

echo $HPM_SDK_BASE

echo "GNURISCV_TOOLCHAIN_PATH=$GNURISCV_TOOLCHAIN_PATH"
echo "HPM_SDK_TOOLCHAIN_VARIANT=$HPM_SDK_TOOLCHAIN_VARIANT"
echo "HPM_SDK_BASE=$HPM_SDK_BASE"

export OPENOCD_SCRIPTS=${HPM_SDK_BASE}/boards/openocd

