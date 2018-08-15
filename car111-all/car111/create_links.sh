#!/bin/sh
#
# file              : create_links.sh
# description       : create link to source files in child directories
# created           : 23 May 2018
# copyright         : (C) 2018 M.S.Khan
# license           : GNU GPLv3
#


find . -mindepth 2 \( -iname '*\.h' -o -iname '*\.cpp' \) -type f -ok ln -sf {} \;

