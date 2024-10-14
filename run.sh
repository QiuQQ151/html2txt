#!/bin/bash

# 获取当前脚本所在的目录
SCRIPT_DIR=$(dirname "$0")

# 切换到脚本所在的目录（确保相对路径正确）
cd "$SCRIPT_DIR"

# 执行当前目录下的可执行文件
./out

# 打印成功日志
echo "可执行文件已运行: $(date)" >> /var/log/executable_log.log
