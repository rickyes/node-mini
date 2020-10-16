#!/bin/bash -xe

BUILD_ARCH_TYPE=$1
V8_BUILD_OPTIONS=$2

cd deps/v8

if [ ! -d _depot_tools ];then
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git _depot_tools
fi

TOOL_PATH=$PWD/_depot_tools

cd _depot_tools && $TOOL_PATH/gclient

cd ..

$TOOL_PATH/gclient config https://chromium.googlesource.com/v8/v8

$TOOL_PATH/gclient sync

tools/dev/v8gen.py x64.release.sample 
# PATH=~/_depot_tools:$PATH gn gen -v out.gn/$BUILD_ARCH_TYPE --args="is_component_build=false is_debug=false use_goma=false goma_dir=\"None\" use_custom_libcxx=false v8_target_cpu=\"$TARGET_ARCH\" target_cpu=\"$TARGET_ARCH\" v8_enable_backtrace=true"
$TOOL_PATH/ninja -C out.gn/$BUILD_ARCH_TYPE v8_monolith