################################################################################
#
#    file                 : Makefile
#    description          : Makefile for module "car111".
#                           The skeleton of this file was originally generated
#                           from "robotgen" script that came with "torcs-1.3.7".
#    created              : 23 May 2018
#    copyright            : (C) 2018 M.S.K.
#    license              : GNU GPLv3
#
#################################################################################

ROBOT       = car111
MODULE      = ${ROBOT}.so
MODULEDIR   = drivers/${ROBOT}
SOURCES     = ${ROBOT}.cpp fuzzy_controller.cpp fuzzy_rules.cpp

SHIPDIR     = drivers/${ROBOT}
SHIP        = ${ROBOT}.xml cg-nascar-rwd.rgb
SHIPSUBDIRS = 

PKGSUBDIRS  = ${SHIPSUBDIRS}
src-robots-car111_PKGFILES = $(shell find * -maxdepth 0 -type f -print)
src-robots-car111_PKGDIR   = ${PACKAGE}-${VERSION}/$(subst ${TORCS_BASE},,$(shell pwd))

include ${MAKE_DEFAULT}

# append fuzzylite library path for includes and linker
INCFLAGS   := $(INCFLAGS) -I${FUZZYLITE_HOME}
LDFLAGS    := $(LDFLAGS) -L${FUZZYLITE_HOME}/release/bin -lfuzzylite

