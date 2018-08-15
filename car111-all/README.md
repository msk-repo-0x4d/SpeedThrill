# SpeedThrill
*`Autonomous vehicles in a race environment`*

SpeedThrill is a project on autonomous vehicles running in race environment. This project uses **TORCS** as the simulator for simulating car races. SpeedThrill is written in C/C++. It has been developed and tested on x86\_64 bit machines running 64 bit version of Linux. It has been built and tested using GCC compilers. Right now it has following autonomous cars :

- *car111*



---

**Table of Contents**

[TOC]



## 1. car111
*The autonomous vehicle with fuzzy control system*

This is the simplest autonomous control with all the basic navigation commands on board. It can navigate in simple race conditions where the control decisions are simple. It uses *fuzzy rules* to find control values for gear, steer, accel and brake.

*car111* uses only one module described below :

1. **fuzzy**
    - latest version - `v1.0.0`
    - module provides control values for
        - gear
        - steer
        - accel
        - brake



## 2. Setting up car111 with TORCS
Here is a brief overview of things that you need to do to get *car111* running in **torcs-1.3.7**:

- Install **torcs-1.3.7**
- Install third-party libraries needed for *car111*
- Copy `car111` in `torcs-1.3.7` source
- Run some setup scripts after copying `car111`
    - Configure car model (use `install-cg-nascar.sh` or patch using `model_car7_trb1.patch`)
    - Create links to source files (use `create_links.sh`)
- Run make targets in `car111` after everything is setup
- Run *torcs* and select *car111* for race



### 2.1 Set up TORCS
First of all, you have to install [**torcs-1.3.7**](https://sourceforge.net/projects/torcs/files/all-in-one/1.3.7/) where *car111* would run. Check [TORCS homepage](http://torcs.sourceforge.net/) to learn more about TORCS. It has been tested with the so-far latest available stable version of **TORCS** -- **v1.3.7**. Previous version of TORCS might be able to run *car111* but all documents and wikipages would use **v1.3.7** as the reference version.

After successfully installing and running *torcs*, you would have following additional variables exported to your shell :

```sh
export TORCS_BASE=/path/to/torcs-1.3.7
export MAKE_DEFAULT=$TORCS_BASE/Make-default.mk
```

We will use this **$TORCS\_BASE** in following sections while setting up *car111*.



### 2.2 Set up 3rd-Party Libraries
*car111* uses following free and open-source 3rd-party libraries:

- **fuzzylite v6.0**  ( https://www.fuzzylite.com )



#### 2.2.1 Set up Fuzzylite
To get *car111* running, one has to download and install [**fuzzylite v6.0**](https://www.fuzzylite.com/downloads/) and do the following setup:

- Set **$FUZZYLITE\_HOME** variable to add path to header files and shared library

```sh
export FUZZYLITE_HOME='/path/to/fuzzylite-6.0/fuzzylite/'
```

This will provide required path to header includes and to shared library to **Makefile** in `car111`. While compiling, **Makefile** looks for this directory that has `fl/` (this directory contains header files) and `release/bin/` (this directory contains shared library `libfuzzylite.so`) directories inside it.

- While running torcs, the shared library path has to be added to the library search path

```sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${FUZZYLITE_HOME}/release/bin
```



#### 2.2.2 Summary
The script version of setup after installing *fuzzylite* is :

```sh
# used in Makefile
# this path should have "fl/" and "release/bin/" directories
export FUZZYLITE_HOME=/path/to/fuzzylite-6.0/fuzzylite/

# add path to shared library, used while running torcs
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${FUZZYLITE_HOME}/release/bin

```

One option is to put the above settings in an rc file like "**.bashrc**" to get this setup loaded everytime one has to compile *car111* or run *torcs*.



### 2.3 Copy *car111*
If you have already installed **torcs-1.3.7** and it is running, then copy the `car111` directory to `$TORCS_BASE/src/drivers/` similar to :

```sh
# copy car111 directory to "$TORCS_BASE/src/drivers"
cp -ri /path/to/car111 -t $TORCS_BASE/src/drivers
```

__NOTE__ : `car111` directory is copied NOT its parent directory `car111-all`



### 2.4 Create links to source files
While compiling, the **Makefile** in `car111` looks for source files in its current directory ( i.e. in directory `car111` ). The script [create_links.sh](car111/create_links.sh) creates symbolic links to all **`.cpp`** and **`.h`** files in its sub-directories. Use this script as shown below :

```sh
# move to car111 directory
cd $TORCS_BASE/src/drivers/car111

# if needed change permission of "create_links.sh"
# to make it executable and run it
chmod u+x create_links.sh
./create_links.sh
```
Also, you will have to confirm creation of link files while executing `create_links.sh`.



### 2.5 Configure car model
*car111* has been tested for **torcs-1.3.7**. The car model **cg-nascar-rwd** included in *car111* is not there in downloaded file **`torcs-1.3.7.tar.bz2`**. This car model **cg-nascar-rwd** and its category **Nascar** have to be manually installed to get this car running. A simpler alternative would be to change the car model of *car111* to one which is already there in **torcs-1.3.7**. Here are details of both the alternatives :



#### 2.5.1 Install car model **cg-nascar-rwd**
- Download **`TORCS-1.3.0-data-cars-nascar.tgz`** from [torcs-cars repo](https://sourceforge.net/projects/torcs/files/torcs-cars/1.3.0/)
- Use this [install-cg-nascar.sh](scripts/install-cg-nascar.sh) script to install it as shown below:
    - you have to pass the path to directory containing downloaded **TORCS-1.3.0-data-cars-nascar.tgz** file while running the above script (unless **TORCS-1.3.0-data-cars-nascar.tgz** is in the same directory as the `install-cg-nascar.sh` script) 

```sh
# move to directory containing the "install-cg-nascar.sh" script
cd /path/to/directory/containing/install-cg-nascar.sh

# check permissions and make it executable if needed as line below
chmod u+x /path/to/script/install-cg-nascar.sh 

# run it with path to directory containing "TORCS-1.3.0-data-cars-nascar.tgz" as its argument
./install-cg-nascar.sh  "/path/to/directory/containing/TORCS-1.3.0-data-cars-nascar.tgz"
```



#### 2.5.2 Change car model to some existing model in torcs-1.3.7
Change car model to **`car7-trb1`** (one of the car models in **torcs-1.3.7**) by applying this [model_car7_trb1.patch](patches/model_car7_trb1.patch) from $TORCS_BASE as shown below :

```sh
# move to $TORCS_BASE before applying patch
cd $TORCS_BASE

# apply patch to change car model to "car7-trb1"
patch -p0 -i /path/to/patch/model_car7_trb1.patch
```



### 2.6 Run make targets
After all setups, you have to compile the source files, link them together and install the entire module. This is done my running make targets `make` and `make install` in directory `car111`. Here are the steps :
- Move to `car111` directory in **torcs** source
- Run `make` followed by `make install`
    - If there is an error then fix it and run `make clean` and re-run `make` and `make install`

Here is the script version of the above steps :
```sh
# move to car111 directory
cd $TORCS_BASE/src/drivers/car111

# compile and install "car111"
make
make install
```

You can now start **torcs**



### 2.7 Select car111 in torcs
Start **torcs** by executing `torcs` in a shell. Press **Enter** if it is **TORCS** initial screen. Select **Race** followed by selecting race type (say selected **Quick Race**). Then select **Configure Race** and press **Enter**. Select a track in **Select Track** and press **Enter**. On right side you will see list of drivers that are not selected for a race. When running *car111* for the first time it would be in this list of not selected drivers. You may have to scroll down to find **car111**. Select it and click on **(De)Select** at the middle column of the screen. You will then find **car111** in the list of **Selected Drivers**. Press **Enter** and continue until you get back to the same **New Race** / **Configure Race** screen. Select **New Race** this time and continue by pressing **Enter**.


**You are DONE here!!!**



## 3. Summary
Here is a checklist of things that should have been done :

-[ ] Downloaded and installed **torcs-1.3.7**

-[ ] Downloaded and installed **fuzzylite-6.0**

-[ ] Exported **FUZZYLITE\_HOME** and added it to library path

-[ ] Copied `car111` to torcs source

-[ ] Created links to source files inside `car111`

-[ ] Installed car model **cg-nascar-rwd** or changed the car model to an existing one

-[ ] Did run make targets (**make** and **make install**) inside `car111` without any error

-[ ] Did run *torcs* and selected *car111* in the list of cars



## 4. License
This is licensed under [GNU GPL v3](LICENSE).


