#!/bin/sh
#
# file              : create_links_in_libs.sh
# description       : creates links of source/header files in race engine library
# created           : 13 Jul 2018
# copyright         : (C) 2018 M.S.Khan
# license           : GNU GPLv3
#


cd $TORCS_BASE/src/libs/raceengineclient/

# these links will get re-exported by Makefile
ln -sf $TORCS_BASE/src/drivers/car222/rl/car222_string_formats.h
ln -sf $TORCS_BASE/src/drivers/car222/rl/car222_Q_maps.h
ln -sf $TORCS_BASE/src/drivers/car222/rl/car222_Q_maps.cpp
ln -sf $TORCS_BASE/src/drivers/car222/rl/car222_race_config.h
ln -sf $TORCS_BASE/src/drivers/car222/rl/car222_race_init.cpp

