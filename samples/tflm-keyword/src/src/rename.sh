#!/bin/bash

rename_files() {
    local dir="$1"

    # 遍历当前目录下的所有文件和目录
    for entry in "$dir"/*
    do
        if [ -d "$entry" ]; then
            # 如果是目录,则递归调用函数
            rename_files "$entry"
        elif [ -f "$entry" ]; then
            # 如果是文件,且文件名以 "riscv_nn_" 开头
            if [[ "$entry" == *"riscv_nn_"* ]]; then
                new_name="${entry/riscv_nn_/tpt_nn_}"
                mv "$entry" "$new_name"
                echo "Renamed $entry to $new_name"
            fi
        fi
    done
}

# 指定要处理的起始目录
start_dir="."

# 调用递归函数
rename_files "$start_dir"