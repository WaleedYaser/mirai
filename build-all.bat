@echo OFF & setlocal & pushd %~dp0
rem Build all the binaries of Mirai
rem make sure to install clang first before calling this file. you can install it by heading to
rem     https://llvm.org/builds/
rem     and under "Windows snapshot builds" you will find "Windows installer (64-bit)"

rem @echo OFF: prevent all commands in the batch file (including echo off command) from displaying
rem     on the screen.
rem setlocal: make all variables (until endlocal) local to this file.
rem pushd: store the current directory for use by popd, and change current directory to the
rem     specified one.
rem %dp0: this macro expands to the full path of the current batch file directory.
rem pushd %dp0: change the current directory to the batch file directory to make sure that all paths
rem     are relative to the current batch file.

call scripts\utils.bat :timer_begin
call scripts\utils.bat :echo_header "==== Compiler ===================================="
call scripts\utils.bat :clang_version
call scripts\utils.bat :echo_header "==== Vulkan SDK ===================================="
call scripts\utils.bat :vulkan_sdk_path
call scripts\utils.bat :echo_header "==== Build ======================================="

call engine\build.bat
call playground\build.bat

call scripts\utils.bat :timer_end total_seconds
call scripts\utils.bat :echo_header "==== Status ======================================"
call scripts\utils.bat :echo_success "Build Mirai Binaries Succeeded! (build time: %total_seconds%s)"
call scripts\utils.bat :echo_header "=================================================="

rem popd: change current directory to the directory that was most recently stored by pushd command
rem endlocal: end the scope of setlocal
popd & endlocal