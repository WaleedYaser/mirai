@echo OFF & setlocal & pushd %~dp0
call ../scripts/utils.bat :clang_build_exec %~dp0 playground
popd