#!/bin/bash

INFO_TTY_COLOR=$(tput setaf 5 && tput bold)
SUCCESS_TTY_COLOR=$(tput setaf 2)
ERROR_TTY_COLOR=$(tput setaf 1 && tput bold)
TTY_COLOR_RESET=$(tput sgr0)

build_dir="build"

cd $build_dir

tests_exe_files=("FSEntryFinderTests" "TranslatorTests" "GeneratorTests")
for test in ${tests_exe_files[*]}; do
    valgrind --leak-check=full "./$test"
    if [ $? -ne 0 ]; then
        echo "${ERROR_TTY_COLOR}Memory errors in file $test.cpp occur${TTY_COLOR_RESET}"
        exit 1
      else
        echo "${SUCCESS_TTY_COLOR}Memory checks of $test.cpp was success${TTY_COLOR_RESET}"
      fi
done

cd ..
exit 0