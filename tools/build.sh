#!/bin/bash

INFO_TTY_COLOR=$(tput setaf 5 && tput bold)
SUCCESS_TTY_COLOR=$(tput setaf 2)
ERROR_TTY_COLOR=$(tput setaf 1 && tput bold)
TTY_COLOR_RESET=$(tput sgr0)

BINARY_DIR="build"
CMAKE_OPTIONS=""
while getopts "tc" opt; do
  case $opt in
  t)
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DTEST=ON "
    ;;
  c)
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "
    ;;
  a*)
    echo "Unexpected flag $opt"
    exit 1
    ;;
  esac
done

echo "${INFO_TTY_COLOR}Enabled cmake options: $CMAKE_OPTIONS${TTY_COLOR_RESET}"
rm -rf $BINARY_DIR

cmake $CMAKE_OPTIONS -S . -B $BINARY_DIR
if [ $? -ne 0 ]; then
  echo "${ERROR_TTY_COLOR}Build ends with errors. Binary directory was not created.${TTY_COLOR_RESET}"
  exit 1
fi

echo "${SUCCESS_TTY_COLOR}Binary directory was created with name $BINARY_DIR. Build finished success${TTY_COLOR_RESET}"