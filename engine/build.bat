@echo OFF & setlocal & pushd %~dp0
call ..\scripts\utils.bat :clang_build_library %~dp0 engine
popd & endlocal