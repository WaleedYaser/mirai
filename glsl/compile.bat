@echo OFF & setlocal & pushd %~dp0
rem compile glsl shaders int SPIR-V

call ..\scripts\utils.bat :glsl_compile %~dp0

popd & endlocal