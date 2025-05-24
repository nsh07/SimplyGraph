#
# **************************************************************
# *         C++ Mathematical Expression Toolkit Library        *
# *                                                            *
# * Author: Arash Partow (1999-2024)                           *
# * URL: https://www.partow.net/programming/exprtk/index.html  *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of the Mathematical Expression Toolkit Library is *
# * permitted under the guidelines and in accordance with the  *
# * most current version of the MIT License.                   *
# * https://www.opensource.org/licenses/MIT                    *
# * SPDX-License-Identifier: MIT                               *
# *                                                            *
# **************************************************************
#


#!/usr/bin/env bash

set -e
trap "echo 'Error: build.sh terminated' >&2" ERR

compiler="c++"
build_mode="Debug"
num_jobs="4"
cmake_generator="Unix Makefiles"
sanitizer_build=""
build_targets=""

help()
{
    echo
    echo "Syntax: build.sh [h|c|m|j|s|g|t]"
    echo "options:"
    echo "h  Help"
    echo "c  Compiler (eg: g++, clang++ etc)"
    echo "m  Build mode (eg: Debug / Release)"
    echo "j  Number of jobs/threads"
    echo "s  Enable sanitizer build"
    echo "g  CMake generator (eg: Unix Makefiles, Ninja)"
    echo "t  Build target (eg: exprtkapp1, exprtkapp2)"
    echo
}

while getopts "hc:m:j:sg:t:" option; do
    case $option in
        h) help
           exit;;
        c) compiler=$OPTARG;;
        m) build_mode=$OPTARG;;
        j) num_jobs=$OPTARG;;
        s) sanitizer_build="ON";;
        g) cmake_generator=$OPTARG;;
        t) build_targets+="$OPTARG ";;
        \?) echo "Error: Invalid option: $OPTARG"
            help
            exit;;
    esac
done

if [ ! -z "$build_targets" ]; then
   build_targets="--target $build_targets"
   echo "build_targets: $build_targets"
fi

if [ ! -z "$sanitizer_build" ]; then
   sanitizer_build="-DENABLE_SANITIZERS=ON"
fi

build_dir="$PWD/build/$build_mode"

echo "[*] Build mode:      $build_mode                                    "
echo "[*] Compiler:        $compiler (version: $($compiler -dumpversion)) "
echo "[*] CMake Generator: $cmake_generator                               "
echo "[*] Jobs:            $num_jobs                                      "
echo "[*] Sanitizer build: $sanitizer_build                               "
echo "[*] Build Directory: $build_dir                                     "
echo "[*] Target(s):       $build_targets                                 "
echo

mkdir -p "$build_dir"

cmake   \
    -S . \
    -B "$build_dir" \
    -G "$cmake_generator" \
    -DCMAKE_BUILD_TYPE=$build_mode \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    -DCMAKE_CXX_COMPILER:FILEPATH="$compiler" \
    $sanitizer_build

cp -rf "$build_dir/compile_commands.json" "$build_dir/.."

cmake --build $build_dir $build_targets -j $num_jobs
