#!/bin/bash

INFO_TTY_COLOR=$(tput setaf 5 && tput bold)
SUCCESS_TTY_COLOR=$(tput setaf 2)
ERROR_TTY_COLOR=$(tput setaf 1 && tput bold)
TTY_COLOR_RESET=$(tput sgr0)

project_dir="project"
src_dir="$project_dir/src"
include_dir="$project_dir/include"
tests_dir="tests"

format_file_path="./tools/.clang-format"

format-command() {
  echo "${INFO_TTY_COLOR}Formatting $1${TTY_COLOR_RESET}"
  extensions=("cpp" "hpp")
  for extension in ${extensions[*]}; do
      clang-format -i --style=file:$format_file_path $1/*.$extension 2> /dev/null
  done
}

format-command ${project_dir}
format-command ${src_dir}
format-command ${include_dir}
format-command ${tests_dir}

echo "${SUCCESS_TTY_COLOR}All directories successfully formatted$1${TTY_COLOR_RESET}"
