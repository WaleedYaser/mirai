@echo OFF & setlocal & pushd %~dp0

call scripts\utils.bat :timer_begin
call scripts\utils.bat :set_esc
call scripts\utils.bat :echo_header "==== Compiler ===================================="
call scripts\utils.bat :clang_version
call scripts\utils.bat :echo_header "==== Build ======================================="

call engine\build.bat
if %ERRORLEVEL% neq 0 (
  call scripts\utils.bat :echo_error "Failed to build engine with exit code %ERRORLEVEL%"
  exit
)

call playground\build.bat
if %ERRORLEVEL% neq 0 (
  call scripts/utils.bat :echo_error "Failed to build playgrund with exit code %ERRORLEVEL%"
  exit
)

call scripts\utils.bat :timer_end total_seconds
call scripts\utils.bat :echo_header "==== Status ======================================"
call scripts\utils.bat :echo_success "Build Mirai Binaries Succeeded! (build time: %total_seconds%s)"
call scripts\utils.bat :echo_header "=================================================="

popd & endlocal