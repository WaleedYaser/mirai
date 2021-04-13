#!/bin/bash
set echo on
pushd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )" &> /dev/null
source ./scripts/utils.sh

build_all_timer=$(timer_begin)
echo_header "==== Compiler ===================================="
clang_version
echo_header "==== Build ======================================="

source ./engine/build.sh
source ./playground/build.sh

total_seconds=$(timer_end "$build_all_timer")
echo_header "==== Status ======================================"
echo_success "Build Mirai Binaries Succeeded! (build time: ${total_seconds}s)"
echo_header "=================================================="

popd &> /dev/null