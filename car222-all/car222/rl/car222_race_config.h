/*
 * =====================================================================================
 * Copyright (C) 2018 M.S.Khan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * =====================================================================================
 */

/*
 * car222_race_config.h
 *
 *  Created on: Apr 16, 2018
 *      Author: M.S.Khan
 */


#ifndef  CAR222_RACE_CONFIG_H_
#define  CAR222_RACE_CONFIG_H_


#include <stdlib.h>

#include "car222_Q_maps.h"


// format for describing a general Q value file for a track
#define Q_VALUE_FILE_NAME_FORMAT  "%s/%s%s.%s"
// Q value file name for a given track
#define Q_VALUE_FILE_NAME(track_name)  \
    getenv("HOME"), ".torcs/drivers/car222/q_learner_", track_name, "txt"


namespace controller
{
    extern controller_storage::Q_maps_storage  _Q_maps_storage;


/* only available in training mode */
#ifdef TRAINING_MODE

    extern int training_race_counter;

    // number of training stages ( each stage has its own learning parameters )
    extern const int LEARNING_STAGES = 4;

    // learning parameters for various stages
    extern const float LEARNING_PARAMETERS[][4] =
    {
        /**
         * Four columns are -
         * ------------------
         * upper bound race counter, learning rate, discount, epsilon
         * ----------------------------------------------------------
         **/
        {30000, 1.0/4, 1020.0/1024, 1.0/256},
        {50000, 1.0/4, 1020.0/1024, 1.0/1024},
        {70000, 3.0/16, 1020.0/1024, 1.0/1024},
        {75000, 3.0/16, 1020.0/1024, 0.0},
        {0, 0.0, 0.0, 0.0}
    };


// pause after 20000 races ( would require key press to resume )
#define TRAINING_PAUSE_COUNTER       20000
// take a break after each 200 races
// would resume on its own after fixed duration ( set to 5 seconds )
#define TRAINING_BREAK_COUNTER       200
// take a break of 5 seconds while taking a break ( set to 200 races )
#define TRAINING_BREAK_DURATION      5
// take a break of 500000 micro seconds ( 0.5 second ) after each race
#define RACE_BREAK_DURATION          500000

// write Q values to file after 1000 races
#define WRITE_AFTER_N_RACES          1000


#endif    // #ifdef TRAINING_MODE

}


#endif    // #ifndef  CAR222_RACE_CONFIG_H_


