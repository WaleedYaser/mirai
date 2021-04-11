# output current clang version
clang_version() {
  clang -v
}

# build library dll in debug mode for now
# parameter1: library src directory
# parameter2: library name
function clang_build_library() {
  timer_begin

  local folder=$1
  local name=lib$2
  local build_dir=../bin/
  mkdir -p $build_dir

  pushd $folder &> /dev/null
  echo $folder

  local c_filenames=$(find . -type f -name "*.c")
  for c_file in ${c_filenames[@]}; do
    echo "  $c_file"
  done

  local compiler_flags="-shared -g -Wvarargs -Wall -Werror"
  local include_flags="-Isrc"
  local linker_flags=""
  local defines="-DMIRAI_DEBUG -DMIRAI_IMPORT"

  clang $c_filenames $compiler_flags -o $build_dir$name.so $defines $include_flags $linker_flags
  local ERRORLEVEL=$?
  if [ $ERRORLEVEL -ne 0 ]
  then
    echo_error "Failed to build $name.so with exit code $ERRORLEVEL"
    exit $ERRORLEVEL
  fi

  local total_seconds=$(timer_end)
  echo_success "Build $name.so Succeeded (build time: ${total_seconds}s)"
  echo # empty line

  popd &> /dev/null
}

# build executable file in debug mode and link it to engine.dll for now
# parameter1: executalble src directory
# parameter2: executalble name
function clang_build_exec() {
  timer_begin

  local folder=$1
  local name=$2
  local build_dir=../bin/
  mkdir -p $build_dir

  pushd $folder &> /dev/null
  echo $folder

  local c_filenames=$(find . -type f -name "*.c")
  for c_file in ${c_filenames[@]}; do
    echo "  $c_file"
  done

  local compiler_flags="-g -Wvarargs -Wall -Werror"
  local include_flags="-Isrc -I../engine/src"
  local linker_flags="-L$build_dir -lengine -Wl,-rpath,."
  local defines="-DMIRAI_DEBUG -DMIRAI_IMPORT"

  clang $c_filenames $compiler_flags -o $build_dir$name $defines $include_flags $linker_flags
  local ERRORLEVEL=$?
  if [ $ERRORLEVEL -ne 0 ]
  then
    echo_error "Failed to build $name with exit code $ERRORLEVEL"
    exit $ERRORLEVEL
  fi

  local total_seconds=$(timer_end)
  echo_success "Build $name Succeeded (build time: ${total_seconds}s)"
  echo # empty line

  popd &> /dev/null
}

# echo error message in red
# parameter1: message
echo_error() {
  local RED='\033[0;31m'
  local NOCOLOR='\033[0m'
  echo -e "${RED}$1${NOCOLOR}"
}

# echo message in yellow
# parameter1: message
echo_header() {
  local YELLOW='\033[0;33m'
  local NOCOLOR='\033[0m'
  echo -e "${YELLOW}$1${NOCOLOR}"
}

# echo message in green
# parameter1: message
echo_success() {
  local GREEN='\033[0;32m'
  local NOCOLOR='\033[0m'
  echo -e "${GREEN}$1${NOCOLOR}"
}

timer_begin() {
  start=$(date +%s)
}

timer_end() {
  local end=$(date +%s)
  echo $((end-start))
}