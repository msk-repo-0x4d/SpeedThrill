################################################################################
#
#    file                 : Makefile
#    description          : Makefile for module "car222".
#                           The skeleton of this Makefile was originally
#                           generated from "robotgen" script that came with
#                           "torcs-1.3.7".
#    created              : 12 Jul 2018
#    copyright            : (C) 2018 M.S.K.
#    license              : GNU GPLv3
#
#################################################################################

ROBOT       = car222
MODULE      = ${ROBOT}.so
MODULEDIR   = drivers/${ROBOT}
SOURCES     = ${ROBOT}.cpp fuzzy_controller.cpp fuzzy_rules.cpp race_reward.cpp\
              car_utils.cpp q_learning.cpp

SHIPDIR     = drivers/${ROBOT}
SHIP        = ${ROBOT}.xml cg-nascar-rwd.rgb
SHIPSUBDIRS = 

PKGSUBDIRS  = ${SHIPSUBDIRS}
src-robots-car222_PKGFILES = $(shell find * -maxdepth 0 -type f -print)
src-robots-car222_PKGDIR   = ${PACKAGE}-${VERSION}/$(subst ${TORCS_BASE},,$(shell pwd))

include ${MAKE_DEFAULT}

# append fuzzylite library path for includes and linker
INCFLAGS   := $(INCFLAGS) -I${FUZZYLITE_HOME}
LDFLAGS    := $(LDFLAGS) -L${FUZZYLITE_HOME}/release/bin -lfuzzylite

# append this flag for training mode
CFLAGSD    := $(CFLAGSD) -DTRAINING_MODE

