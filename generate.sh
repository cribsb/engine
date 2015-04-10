#!/usr/bin/env bash

# Copyright (C) 2015 EEnginE project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


###########################
#### BEGIN Config Part ####
###########################

CONFIG_FILE="generate.cfg"

###########################
####  END Config Part  ####
###########################

STARTDIR="$PWD"

shopt -s extglob # Tells bash to do some fancy regex stuff

catch_INT() {
   error "User interrupt \x1b[1;31m[CTRL-C]" 1>&2
   exit
}

trap catch_INT INT

# Make sure we are in the engine root directory
export ENGINE_ROOT="$(readlink -m "$(dirname "$0")")"
[ -d "$ENGINE_ROOT" ] && cd "$ENGINE_ROOT"


EEC=$(awk "BEGIN {printf \"%i\", (${RANDOM} / 32767) * 6 + 31}") # COLORS!!! RANDOM!!!!! :)

EEE_C="\x1b[${EEC}m\x1b[1m"
EEE_R="\x1b[0m\x1b[1m"

#      _____ _____            _       _____
#     |  ___|  ___|          (_)     |  ___|
#     | |__ | |__ _ __   __ _ _ _ __ | |__
#     |  __||  __| '_ \ / _` | | '_ \|  __|
#     | |___| |__| | | | (_| | | | | | |___
#     \____/\____/_| |_|\__, |_|_| |_\____/
#                        __/ |
#                       |___/


eval "PAD_E=\"\$(printf '%0.1s' \" \"{1..$(( $(tput cols) / 2 - 17 ))} )\""

echo -e "\x1b[?25l"
echo -e "${PAD_E}${EEE_C} _____${EEE_R} _____           ${EEE_C} _ ${EEE_R}     ${EEE_C} _____  "
echo -e "${PAD_E}${EEE_C}|  ___${EEE_R}|  ___|          ${EEE_C}(_)${EEE_R}     ${EEE_C}|  ___| "
echo -e "${PAD_E}${EEE_C}| |__ ${EEE_R}| |__ _ __   __ _ _ _ __ ${EEE_C}| |__   "
echo -e "${PAD_E}${EEE_C}|  __|${EEE_R}|  __| '_ \ / _\` | | '_ \\\\${EEE_C}|  __|  "
echo -e "${PAD_E}${EEE_C}| |___${EEE_R}| |__| | | | (_| | | | | ${EEE_C}| |___  "
echo -e "${PAD_E}${EEE_C}\____/${EEE_R}\____/_| |_|\__, |_|_| |_${EEE_C}\____/  "
echo -e "${PAD_E}${EEE_C}      ${EEE_R}             __/ |               "
echo -e "${PAD_E}${EEE_C}      ${EEE_R}            |___/                "
echo -e ""
echo -e ""

help_text() {
    msg1 "Help Message"
cat << EOF

    You will probably be just fine with running $0 without any arguments.

      +  - Turn everything on (but only gltCcf)

      g  - Force recompile GLEW
      l  - Build libs
      t  - Build tests
      C  - count lines of code (needs cloc and bc)
      c  - Clean up
      f  - Reformat code with clang-format (this can only reformat code found with l and t)

     - Special flags
      q  - disable procress bar and some escape sequenzes
      Q  - disable procress bar complete
      b  - build the project
      A  - generate .atom-build.json based on the build variables
      G  - disable pulling git submodules

      - Build environment variables:

      CMAKE_EXECUTABLE: Path to cmake (default $(which cmake))
      BUILD_COMMAND:    Path to the make command (default $(which make))
      BUILD_DIR:        Path to the build directory (default build)
      MAKEFILE_NAME:    Name of the build file generated from CMake (default Makefile)

      CMAKE_FLAGS:      Addistional cmake flags
      CMAKE_GENERATOR:  Cmake generator to use (default: let cmake decide)
      SKIP_CMAKE:       When 1, skip CMake
      CPP_COMPILER:     The CPP compiler (default: let cmake decide)
      C_COMPILER:       The C compiler (default: let cmale decide)
      INST_PREFIX:      Where to install

      BUILD_FLAGS:      arguments for the build system (e.g. make)

     - Debug stuff
      p  - Print parsed Config
      S  - Do not parse config file

      NOTE: if you have already run this script and then disable tests and/or libs, you need to enable clean

    without any options the configuration is: $0 ltf
EOF
    echo -e "\x1b[?25h"
    exit
}


DO_TESTS=0
DO_LIBS=0
DO_CLOC=0
DO_GLEW=0
DO_CLEAN=0
DO_FORMAT=0
DO_BUILD=0
DO_ATOM_BUILD=0
SKIP_PARSING=0
SKIP_SUB_M=0

PRINT_PARSED=0

ARG_STRING=""

# Load all functions in the generate dir
for i in $(find "${PWD}/generate/functions" -name '*.sh' -type f -print); do
   source "$i"
done

msg1 "Parsing comand line..."

for I in "$@"; do
    case $I in
        help|--help|-h)
            help_text
            ;;
    esac

    ARG_STRING="${ARG_STRING}${I}"
done

if [ -z "$ARG_STRING" ]; then
    ARG_STRING="ltf"
fi

CMD_FLAGS="Result:"

for (( i=0; i<${#ARG_STRING}; ++i )); do
    I=${ARG_STRING:$i:1}

    case $I in
        +)
            DO_TESTS=1
            DO_LIBS=1
            DO_CLOC=1
            DO_GLEW=1
            DO_CLEAN=1
            DO_FORMAT=1
            ;;
        g) DO_GLEW=1      ; CMD_FLAGS="$CMD_FLAGS [GLEW]";;
        l) DO_LIBS=1      ; CMD_FLAGS="$CMD_FLAGS [LIBS]";;
        t) DO_TESTS=1     ; CMD_FLAGS="$CMD_FLAGS [TESTS]";;
        C) DO_CLOC=1      ; CMD_FLAGS="$CMD_FLAGS [COUNT]";;
        c) DO_CLEAN=1     ; CMD_FLAGS="$CMD_FLAGS [CLEAN]";;
        f) DO_FORMAT=1    ; CMD_FLAGS="$CMD_FLAGS [FORMAT]";;
        b) DO_BUILD=1     ; CMD_FLAGS="$CMD_FLAGS -BUILD-";;
        A) DO_ATOM_BUILD=1; CMD_FLAGS="$CMD_FLAGS [ATOM B]";;
        q) ESC_CLEAR=""
           PB_NEWLINE=1   ; CMD_FLAGS="$CMD_FLAGS -quiet-";;
        Q) PB_ENABLE=0    ; CMD_FLAGS="$CMD_FLAGS -pb off-";;
        G) SKIP_SUB_M=1   ; CMD_FLAGS="$CMD_FLAGS -skip submodules-";;
        S) SKIP_PARSING=1 ; CMD_FLAGS="$CMD_FLAGS -skip config-"
           PB_COLLS=100   ; ;;

        p) PRINT_PARSED=1 ; CMD_FLAGS="$CMD_FLAGS -print parsed-";;
        *)
            error "Unknown Argument '$I'"
            help_text
            exit
            ;;
    esac

done

CMD_FLAGS="${CMD_FLAGS//[/\\x1b[35m[\\x1b[36m}"
CMD_FLAGS="${CMD_FLAGS//]/\\x1b[35m]}"

msg2 "$CMD_FLAGS"

if (( SKIP_PARSING == 0 )); then
   CONFIG_FILE="$(pwd)/$CONFIG_FILE"
   parseCFG
fi


if (( SKIP_SUB_M == 0 )); then
   msg1 "Initiating and updating submodules"

   GIT_EXEC=$(which git 2>/dev/null)

   if [ -z "${GIT_EXEC}" ]; then
       warning "Unable to find git. Please run git init, sync and update manualy"
   else
       processBar 1 5 "init"
       git submodule init                                  &> /dev/null

       processBar 2 5 "sync"
       git submodule sync --recursive                      &> /dev/null

       processBar 3 5 "update --init"
       git submodule update --init --recursive             &> /dev/null

       processBar 4 5 "update"
       git submodule update --recursive                    &> /dev/null

       processBar 5 5 "Running make deps"

       makeDeps > "$DEPS_MAIN_DIR/$CMAKE_LISTS_NAME"
   fi
fi

doGlew $DO_GLEW

(( PRINT_PARSED == 1 )) && printWhatParsed
(( DO_CLEAN     == 1 )) && clean


if (( DO_LIBS == 1 )); then
    generateLogMacros "$LOG_MACRO_PATH" "$LOG_TYPES" "$LOG_GEN_UNDEF"
    compilerTests
    addTarget
    msg1 "Generating main include file $INCLUDE_FILE"
    engineHPP         1> "$INCLUDE_FILE"
fi

(( DO_TESTS      == 1 ))                 && tests
(( DO_TESTS      == 1 || DO_LIBS == 1 )) && rootCMake 1> "$(pwd)/$CMAKE_LISTS_NAME"
(( DO_FORMAT     == 1 ))                 && reformatSource
(( DO_CLOC       == 1 ))                 && countLines
(( DO_BUILD      == 1 ))                 && buildProject
(( DO_ATOM_BUILD == 1 ))                 && generateAtomBuild


msg1 "DONE\x1b[?25h"

# Switch back to the start directory where $0 was called
[ -d "$STARTDIR" ] && cd "$STARTDIR"

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
