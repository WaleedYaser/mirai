#!/bin/bash
set echo on
pushd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )" &> /dev/null
source ./scripts/utils.sh

timer_begin
echo_header "==== Compiler ===================================="
clang_version
echo_header "==== Build ======================================="

source ./engine/build.sh
source ./playground/build.sh

total_seconds=`timer_end`
echo_header "==== Status ======================================"
echo_success "Build Mirai Binaries Succeeded! (build time: ${total_seconds}s)"
echo_header "=================================================="
popd &> /dev/null