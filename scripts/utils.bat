@echo OFF & call %*

exit /B %ERRORLEVEL%

rem replace all %ESC% variables with the correct represenation, call this in any file you want to use colors in
:set_esc
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)

rem output current clang version
:clang_version
  clang -v
exit /B 0

rem build library dll in debug mode for now
rem parameter1: library src directory
rem parameter2: library name
:clang_build_library
  @echo OFF & setlocal
  setlocal EnableDelayedExpansion

  call :set_esc
  call :timer_begin

  set folder=%~1
  set name=%~2
  set build_dir=..\bin\
  if not exist %build_dir% mkdir %build_dir%

  pushd %folder%
  echo %folder%

  set c_filenames=
  for /R %%f in (*.c) do (
    set c_filenames=!c_filenames! %%f
    echo   %%f
  )

  set compiler_flags=-g -shared -Wvarargs -Wall -Werror
  set include_flags=-Isrc
  set linker_flags=-luser32
  set defines=-DMIRAI_DEBUG -DMIRAI_EXPORT

  clang %c_filenames% %compiler_flags% -o %build_dir%%name%.dll %defines% %include_flags% %linker_flags%

  call :timer_end total_seconds
  call :echo_success "Build %name%.dll Succeeded (build time: %total_seconds%s)"
  echo.

  popd & endlocal
exit /B 0

rem build executable file in debug mode and link it to engine.dll for now
rem parameter1: executalble src directory
rem parameter2: executalble name
:clang_build_exec
  @echo OFF & setlocal
  setlocal EnableDelayedExpansion

  call :set_esc
  call :timer_begin

  set folder=%~1
  set name=%~2
  set build_dir=..\bin\
  if not exist %build_dir% mkdir %build_dir%

  pushd %folder%
  echo %folder%

  set c_filenames=
  for /R %%f in (*.c) do (
    set c_filenames=!c_filenames! %%f
    echo   %%f
  )

  set compiler_flags=-g -Wvarargs -Wall -Werror
  set include_flags=-Isrc -I../engine/src
  set linker_flags=-L%build_dir% -lengine.lib
  set defines=-DMIRAI_DEBUG -DMIRAI_IMPORT

  clang %c_filenames% %compiler_flags% -o %build_dir%%name%.exe %defines% %include_flags% %linker_flags%

  call :timer_end total_seconds
  call :echo_success "Build %name%.exe Succeeded (build time: %total_seconds%s)"
  echo.

  popd & endlocal
exit /B 0

rem fill input variables with colors to echo if you want to change output color
rem parameter1: out default color
rem parameter2: out red color
rem parameter3: out green color
rem parameter4: out yellow color
:colors
  rem default
  set "%~1=%ESC%[0m"
  rem red
  set "%~2=%ESC%[91m"
  rem green
  set "%~3=%ESC%[92m"
  rem yellow
  set "%~4=%ESC%[93m"
exit /B 0

rem echo message as error and color it in red
rem parameter1: message
:echo_error
  call :colors default, red, green, yellow
  rem %~1 parameter1
  echo %red%Error: %~1%default%
exit /B 0

rem echo message in yellow
rem parameter1: message
:echo_header
  call :colors default, red, green, yellow
  rem %~1 parameter1
  echo %yellow%%~1%default%
exit /B 0

rem echo message in green
rem parameter1: message
:echo_success
  call :colors default, red, green, yellow
  rem %~1 parameter1
  echo %green%%~1%default%
exit /B 0

rem begin time calculation
:timer_begin
  SET begin_time=%TIME%
exit /B 0

rem end time calculatio and return time in seconds since calling timer_begin
rem parameter1: out total_seconds
:timer_end
  SET end_time=%TIME%

  set options="tokens=1-4 delims=:.,"
  for /f %options% %%a in ("%begin_time%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
  for /f %options% %%a in ("%end_time%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

  set /a hours=%end_h%-%start_h%
  set /a mins=%end_m%-%start_m%
  set /a secs=%end_s%-%start_s%
  set /a ms=%end_ms%-%start_ms%
  if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
  if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
  if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
  if %hours% lss 0 set /a hours = 24%hours%
  if 1%ms% lss 100 set ms=0%ms%

  set /a totalsecs = %hours%*3600 + %mins%*60 + %secs%
  set "%~1=%totalsecs%.%ms%"
exit /B 0