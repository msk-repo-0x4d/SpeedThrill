#!/bin/bash
#
# file              : set_home.sh
# description       : make sure home directory for car222 exists
# created           : 23 Nov 2018
# copyright         : (C) 2018 M.S.Khan
# license           : GNU GPLv3
#


if [[ ! -d "$HOME/.torcs/drivers/car222/" ]]; then
    mkdir -p "$HOME/.torcs/drivers/car222/"
fi

