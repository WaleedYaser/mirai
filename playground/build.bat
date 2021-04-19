@echo OFF & setlocal & pushd %~dp0
rem build mirai playground.exe

call ../scripts/utils.bat :clang_build_exec %~dp0 playground
popd & endlocal