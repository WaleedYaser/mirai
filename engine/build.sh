#!/bin/bash
set echo on
pushd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )" &> /dev/null
source ./../scripts/utils.sh

clang_build_library $(pwd) engine

popd &> /dev/null