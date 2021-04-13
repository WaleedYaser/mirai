#!/bin/bash
set echo on
pushd -q "$( cd -q "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source ./../scripts/utils.sh

clang_build_library $(pwd) engine

popd &> /dev/null