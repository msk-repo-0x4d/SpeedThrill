#!/bin/bash
#
# file              : install-cg-nascar.sh
# description       : script for installing car model "cg-nascar-rwd"
#                     and car category "Nascar" for torcs-1.3.7
# created           : 23 May 2018
# copyright         : (C) 2018 M.S.Khan
# license           : GNU GPLv3
#


# fallback directory is the current directory for
# looking "TORCS-1.3.0-data-cars-nascar.tgz"
export CAR_NASCAR_TGZ="."

if [[ -n "$1" ]]; then
    export CAR_NASCAR_TGZ="$1";
fi

# exit if "TORCS-1.3.0-data-cars-nascar.tgz" is not found
if [[ ! -f "$CAR_NASCAR_TGZ/TORCS-1.3.0-data-cars-nascar.tgz" ]]; then
    echo ERROR: file \"TORCS-1.3.0-data-cars-nascar.tgz\" was not found in "$CAR_NASCAR_TGZ"
    exit 1
fi


cd $TORCS_BASE

# for adding category "Nascar"
mkdir data/cars/categories/Nascar

# copy "Nascar.xml" to data/cars/categories/Nascar
tar -C data/cars/categories/Nascar -xzvf $CAR_NASCAR_TGZ/TORCS-1.3.0-data-cars-nascar.tgz \
        ./categories/Nascar.xml --strip-components=2

# extract "cg-nascar-rwd" to data/cars/models
tar -C data/cars/models/ -xzvf $CAR_NASCAR_TGZ/TORCS-1.3.0-data-cars-nascar.tgz \
        ./cars/cg-nascar-rwd --strip-components=2


# date value to be used in Makefile
FORMATTED_DATE=$(date '+%d %b %Y')
# author value to be used in Makefile
MAKE_AUTHOR=$LOGNAME


# Add Makefile to data/cars/categories/Nascar

echo -e "\
################################################################################\n\
#\n\
#    file                 : Makefile\n\
#    description          : Makefile for category \"Nascar\".\n\
#                           This file was generated from \"install-cg-nascar.sh\"\n\
#                           script while porting car category \"Nascar\" to\n\
#                           \"torcs-1.3.7\".\n\
#    created              : $FORMATTED_DATE\n\
#    copyright            : (C) 2018 $MAKE_AUTHOR\n\
#    license              : GNU GPLv3\n\
#\n\
#################################################################################\n\
\n\
\n\
DATADIR         = categories\n\
\n\
DATA            = \$(shell find * -maxdepth 0 -type f -name '*.xml')\n\
\n\
data_PKGFILES   = \$(DATA)\n\
\n\
data_PKGDIR     = \$(DATADIR)\n\
\n\
include \${MAKE_DEFAULT}\n\
\n"   > data/cars/categories/Nascar/Makefile


# Add Makefile to data/cars/models/cg-nascar-rwd

echo -e "\
################################################################################\n\
#\n\
#    file                 : Makefile\n\
#    description          : Makefile for car model \"cg-nascar-rwd\".\n\
#                           This file was generated from \"install-cg-nascar.sh\"\n\
#                           script while porting car model \"cg-nascar-rwd\" to\n\
#                           \"torcs-1.3.7\".\n\
#    created              : $FORMATTED_DATE\n\
#    copyright            : (C) 2018 $MAKE_AUTHOR\n\
#    license              : GNU GPLv3\n\
#\n\
#################################################################################\n\
\n\
\n\
CAR                     = cg-nascar-rwd\n\
\n\
DATADIR                 = cars/\$(CAR)\n\
\n\
DATA                    = \$(CAR).xml \\\\\n\
                          \$(CAR).rgb shadow.rgb tex-wheel.rgb \\\\\n\
                          \$(shell find *.acc -maxdepth 0 -type f -print) \\\\\n\
                          readme.txt \n\
\n\
data_PKGFILES           = \$(DATA) \$(shell find *.xcf -maxdepth 0 -type f -print)\n\
\n\
data_PKGDIR             = \$(DATADIR)\n\
\n\
include \${MAKE_DEFAULT}\n\
\n"   > data/cars/models/cg-nascar-rwd/Makefile


# install car category "Nascar"
cd data/cars/categories/Nascar/
make datainstall


# install car model "cg-nascar-rwd"
cd ../../models/cg-nascar-rwd/
make datainstall

