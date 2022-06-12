#!/bin/bash

CMAKE_BINARY_DIR="build"
TMP_DIR_NAME="tmp"
TMP_FILE="$TMP_DIR_NAME/format-diff"

./scripts/build.sh -t
mkdir -p ${TMP_DIR_NAME}
touch $TMP_FILE

ret_code=0
check-ret-code() {
  if [ $? -ne 0 ]; then
    ret_code=1
    echo "${ERROR_TTY_COLOR}Errors in file $1 occur. Linters, that found error $2${TTY_COLOR_RESET}"
  else
    echo "${SUCCESS_TTY_COLOR}PROCESSING $1 with $2 was success${TTY_COLOR_RESET}"
  fi
}

CPPCHECK_CHECKS="warning,style,performance,portability"

INFO_TTY_COLOR=$(tput setaf 5 && tput bold)
SUCCESS_TTY_COLOR=$(tput setaf 2)
ERROR_TTY_COLOR=$(tput setaf 1 && tput bold)
TTY_COLOR_RESET=$(tput sgr0)

DIRS=("project" "tests")
for dir in ${DIRS[*]}; do
  for file in $(find -P $dir -type f); do
    echo "${INFO_TTY_COLOR}PROCESSING $file${TTY_COLOR_RESET}"

    cpplint --filter=-legal/copyright,-readability/casting,-build/include_subdir $file
    check-ret-code $file "cpplint" ret_code
    cppcheck --enable=$CPPCHECK_CHECKS --error-exitcode=1 -I project/include --suppress=missingIncludeSystem $file
    check-ret-code $file "cppcheck" ret_code

  done
done

exit $ret_code