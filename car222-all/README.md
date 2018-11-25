# SpeedThrill
*`Autonomous vehicles in a race environment`*

SpeedThrill is a project on autonomous vehicles running in race environment. This project uses **TORCS** as the simulator for simulating car races. SpeedThrill is written in C/C++. It has been developed and tested on x86\_64 bit machines running 64 bit version of Linux. It has been built and tested using GCC compilers. Right now it has following autonomous cars :

- *car111*


<img src="../../../wiki/car111_race0.gif" alt="car111" width="33%" />

---

- *car222*


<img src="../../../wiki/car222_track_medium.gif" alt="car222" width="33%" />




---

**Table of Contents**

[TOC]



## car222
*The autonomous vehicle with Q Learning and fuzzy control*

**car222** is an autonomous vehicle with Q Learning and Fuzzy control system. This is an advancement to **car111** autonomous control. It uses all the control values same as that of **car111** except for the acceleration pedal. It overrides the control value for the acceleration pedal by that of a value suggested by its Q Learner. In current implementation **car222** lacks generalization ability (currently it has generality as much as its states are generic in description) and cannot be run reliably on tracks with unseen states. In current version both **car111** and **car222** also lack decision making for multi-agent environment as they do not consider position of other cars on the track.


*car222* uses two modules described below :

1. **fuzzy**
    - latest version - `v1.0.0`
    - module provides control values for
        - gear
        - steer
        - accel (overridden by **`rl`** module)
        - brake

1. **rl**
    - latest version - `Accel-v1.0.0`
    - best performance when trained for specific race track
    - module provides control values for
        - accel



## Modes for `car222`

There are two modes in which **car222** can be run :

1. **TRAINING MODE**
    - This mode is used to train **car222** on a specific track. In this mode, races restart one after another until training ends or game is paused using training session parameters. The training session can run across multiple sessions as Q values get saved to files at periodic interval.
    - It is also possible to resume training in TRAINING\_MODE after running in RACE\_MODE.
    - Changing to this mode requires re-compilation of two modules :
        - raceengineclient (`src/libs/raceengineclient`)
        - car222     (`src/drivers/car222`)
    - An alternative would be to run one *Practice Race* instead of multiple *Quick Race* but it was observed that this is memory inefficient as after several races the memory size grows rapidly. On the other hand in TRAINING\_MODE for *Quick Race* nothing is stored in memory for car222 except its Q values and a race_counter.

2. **RACE MODE**
    - This mode uses Q values learnt for a specific track, along with its fuzzy control system, to navigate the track. This mode is an ordinary mode where after each race, game continues as usual.
    - Changing to this mode also requires re-compilation of two modules :
        - raceengineclient (`src/libs/raceengineclient`)
        - car222     (`src/drivers/car222`)



## Steps for running car222

Below is a summary of steps for running **car222** (here it is assumed that you already have **car111** running in **torcs**) :

1. Copy `car222` inside torcs and create links
    - use [car222/create_links_in_libs.sh](car222/create_links_in_libs.sh) for linking files in `car222` to `$TORCS_BASE/src/lib/raceengineclient/`
    - use [car222/create_links.sh](car222/create_links.sh) to create links to files in child directories of `car222`

2. Apply [patches/libs_car222_training.patch](patches/libs_car222_training.patch) to get into TRAINING\_MODE for **car222**

3. If needed configure following types of parameters for **car222**
    - learning parameters 
    - training session parameters 
    - reward function/parameters
    - Files for configuring these parameters are -
        - `car222/rl/car222_race_config.h`
        - `car222/race_reward.h`
        - `car222/race_reward.cpp`
    - For more check section on [configuring car222 parameters](#configuring-car222-parameters)

4. Recompile and reinstall **raceengineclient** library and **car222** in TRAINING\_MODE
    - compile and re-install **raceengineclient** library (this would make races restart one after another)
    - compile and re-install **car222**

5. Create `car222` directory inside `$HOME/.torcs/drivers/` for saving Q value files
    - One can use [scripts/set_home.sh](scripts/set_home.sh) to make sure Q values would be saved inside `$HOME/.torcs/drivers/car222/`

6. Apply [patches/no_display.patch](patches/no_display.patch) (this patch is applied from $HOME directory) to run torcs without displaying the race

7. Configure following race parameters for a *race type* by using GUI (by running torcs) or by using appropriate race config. files in `$HOME/.torcs/config/raceman/`
    - *race track*
    - *number of laps for race*
    - *players* (select just **car222** for TRAINING\_MODE)
    - For demo, the following configuration was used
        - Race Type - **"Quick Race"**
        - Track - **"CG track 2"**
        - Number of Laps - **"2"**
        - Players - **"car222"**

8. Start training
    - Run torcs using GUI by running 
    ```
    torcs
    ```

   **Alternatively,**

    - Run torcs without GUI as
    ```
    torcs -r $HOME/.torcs/config/raceman/quickrace.xml | grep --line-buffered "shutdown"
    ```

9. After training is complete
    - Apply [patches/race_mode.patch](patches/race_mode.patch) to run in RACE\_MODE
    - This **race_mode.patch** removes the TRAINING\_MODE flag and adds a RACE\_MODE flag
    - Re-compile and re-install **raceengineclient** and **car222** modules

10. Apply reverse patch [patches/no_display.patch](patches/no_display.patch) from $HOME directory (if this patch was applied) to get back the display

11. Run **torcs** for a test drive

12. In case more training is needed after running in RACE\_MODE then reverse apply the [patches/race_mode.patch](patches/race_mode.patch) patch (this will take you back to TRAINING\_MODE). But then you have to clean, re-compile and re-install **raceengineclient** and **car222** modules

Also check below the script version of the steps described above.



## Summary of steps ( script version )
Below is a bash script version of steps needed to get **car222** running. Here it is assumed that you already have **car111** running in **torcs**.

```sh
# copy car222 (not car222-all but its child directory car222)
# to torcs drivers directory
cp -ai /path/to/car222 $TORCS_BASE/src/drivers/

cd $TORCS_BASE/src/drivers/car222/

# create links in raceengineclient library from car222 directory
./create_links_in_libs.sh

# create links for files in child directory of car222
./create_links.sh

# apply patch for training mode
cd $TORCS_BASE
patch -p0 -i /path/to/patches/libs_car222_training.patch

## tune learning parameters, session parameters and
## reward function/parameters as needed in :
## rl/car222_race_config.h
## race_reward.h
## race_reward.cpp

# move to raceengineclient directory
cd src/libs/raceengineclient/

# clean, recompile and reinstall raceengineclient library
make clean && make && make install

# move to car222 directory
cd ../../drivers/car222/

# clean, recompile and reinstall car222 files
make clean && make && make install

# create car222 directory inside $HOME/.torcs/drivers
# using set_home.sh script
/path/to/scripts/set_home.sh

# apply no display patch to turn off display while training
# this patch is applied from $HOME directory
cd $HOME
patch -p0 -i /path/to/no_display.patch

## set race type, track, number of laps and player settings
## for training by running :

#torcs

## Or by using appropriate race config file
## for "Quick Race" config file is -
## "$HOME/.torcs/config/raceman/quickrace.xml"
##
## Running torcs and selecting using GUI is a simpler option

# start training race
torcs -r $HOME/.torcs/config/raceman/quickrace.xml \
          | grep --line-buffered "shutdown"

# or run torcs and select "Quick Race" by running :
#torcs

# after training apply patch for race mode
cd $TORCS_BASE
patch -p0 -i /path/to/race_mode.patch

# recompile and reinstall "raceengineclient" and "car222" modules
cd src/libs/raceengineclient/
make clean && make && make install

cd ../../drivers/car222/
make clean && make && make install

# apply reverse patch for no_display from $HOME directory
# (if this patch was applied)
cd $HOME
patch -p0 -R -i /path/to/no_display.patch

# start torcs for a test drive
torcs

## in case more training is needed, reverse apply
## the race_mode patch and it will take you back to
## training mode as shown below:
#
#cd $TORCS_BASE
#patch -p0 -R -i /path/to/race_mode.patch
#
## but you have to clean, recompile and reinstall
## "raceengineclient" and "car222"


```



## Configuring car222 Parameters

While training `car222` on a track, several parameters can be configured -
- learning parameters
- training session parameters
- reward function/parameters

After configuration is done, one should recompile the **raceengineclient** library and **car222** with their respective Makefile.



#### Configure Learning Parameters

Learning is done in multiple stages (one can keep just one row of learning parameters to make it a single stage learning). These are configured in [car222/rl/car222_race_config.h](car222/rl/car222_race_config.h).
- Learning parameters for each stage is
    - **upper limit of race number** - race number to which this stage parameters are used
    - **learning rate** - learning rate of Q values (for a given learning stage)
    - **discount factor** - discount value for Q value update (for a given learning stage)
    - **exploration rate** - exploration rate for using epsilon-greedy policy while training (for a given learning stage)
- For added safety an additional learning stage is added at the end that has 0 for each parameter. This makes races run in TRAINING\_MODE after training is over without making any updates to Q values.



#### Configure Training Session Parameters

In TRAINING\_MODE races restart one after another. For not letting CPU fry (if you are running training session in hundred thousand races), after each race the program takes a small break. After large number of races, program is given a bigger break periodically. Even after larger number of races (generally after running for a day or so) program is paused (resumes after pressing keys). These break duration and race numbers are controlled by training session parameters defined in [car222/rl/car222_race_config.h](car222/rl/car222_race_config.h). These parameters control
- How long it would sleep **(default is 0.5 seconds)** between each race
- After how many races **(default is 200)** there will be a break **(default is for 5 seconds)** and would resume on its own
- After how many races **(default is 20000)** there will be a pause (in GUI mode one can resume by pressing keys and in **Torcs non-gui mode** it will end the program)

- Additionally, there is a parameter that defines after how many races **(default is 1000)** the Q values in the memory will be saved to file (inside `$HOME/.torcs/drivers/car222/` directory).



#### Configure Reward Function

Reward function parameters are stored in [car222/race_reward.h](car222/race_reward.h) and the function is defined in [car222/race_reward.cpp](car222/race_reward.cpp). The **REWARD\_ID** is unique for each function and configuration. Similar tracks may reuse the same reward configuration but tracks that are very different may need different reward parameters or even different reward function for efficient training.



## Quick Points

- Learning parameters can be tuned in
    - **`car222/rl/car222_race_config.h`**

- Session parameters can be tuned in
    - **`car222/rl/car222_race_config.h`**

- Reward function/parameters can be tuned in
    - **`car222/race_reward.h`**
    - **`car222/race_reward.cpp`**

- Q value files are saved in
    - **`$HOME/.torcs/drivers/car222/`**
    - create this directory if it does not exit (see **`scripts/set_home.sh`**)

- **Torcs** is run in non-gui mode by using **`-r`** option along with its config. file as parameter value
    - Config. file for **"Quick Race"** is **`$HOME/.torcs/config/raceman/quickrace.xml`**

- One can also do training in gui mode by turning off the display (use **`no_display.patch`** )
    - to turn on the display again reverse apply **`no_display.patch`**

- **`race_mode`** patch is only applied after **`libs_car222_training`** patch has been applied i.e. RACE\_MODE patch is applied only if it is in TRAINING\_MODE


