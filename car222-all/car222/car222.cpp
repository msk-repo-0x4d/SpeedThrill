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
 * car222.cpp
 *
 * The autonomous car with Q Learning and fuzzy control
 *
 * This is an extension to autonomous car111. It uses all the control values same as that
 * of car111 except for the acceleration pedal. It overrides the control value for the
 * acceleration pedal by that of a value suggested by its Q Learner.
 *
 * car222 uses control values of car111 ( fuzzy control - "v1.0.0" ) for three controls -
 * - steer
 * - gear 
 * - brake
 *
 * On top of this it uses Q Learner ( "Accel-v1.0.0" ) for controlling
 * - acceleration pedal
 *
 * Some of the features/limitations of this version of QLearner ( "Accel-v1.0.0" ) are -
 * - This QLearner is trained with fuzzy control on board which means it assumes certain
 *   state action relation to hold true during its training and deployment. This should be
 *   guaranteed by using the same fuzzy control version for training and deployment.
 * - This QLearner lacks generalization ability for unseen states. This implies that this
 *   QLearner can only be deployed on tracks that it has been trained upon.
 *
 *  Note - The skeleton of this file (using various TORCS APIs) was originally generated
 *  from "robotgen" script that came with "torcs-1.3.7".
 *
 *     version  : 1.0.0
 *  created on  : 12 Jul 2018
 *      author  : M.S.Khan
 */


#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>
#include <robot.h>

#undef _name  // interferes with a MACRO in fuzzylite
#include "fuzzy_controller.h"
#include "q_learning.h"
#include "car222_race_config.h"
#include "race_reward.h"
#include "car_utils.h"


// maximum length of Q value file name
#define FILE_NAME_BUFFER_SIZE         1024
// speed y is clipped to this value
#define SPEED_Y_CLIP_VALUE            1
// distance on left/right side of the track is clipped to this value
#define TRACK_SIDE_CLIP_DISTANCE      6


extern tRmInfo	*ReInfo;

static tTrack    *curTrack;

static controller::FuzzyController m_fuzzy_controller;

static controller::QLearner m_q_learner(controller::_Q_maps_storage);

static const int SC = 1;
static float distance_raced = 0;
static char QLearner_File[FILE_NAME_BUFFER_SIZE] = "";


static void initTrack(int index, tTrack* track, void *carHandle,
        void **carParmHandle, tSituation *s);

static void newrace(int index, tCarElt* car, tSituation *s);
static void drive(int index, tCarElt* car, tSituation *s);
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt);


/* 
 * Module entry point  
 */
extern "C" int 
car222(tModInfo *modInfo)
{
    memset(modInfo, 0, 10*sizeof(tModInfo));

    modInfo->name    = strdup("car222");                    /* name of the module */
    modInfo->desc    = strdup(
            "Autonomous vehicle with Q Learning and fuzzy control");      /* description */

    modInfo->fctInit = InitFuncPt;                          /* init function */
    modInfo->gfId    = ROB_IDENT;                           /* supported framework version */
    modInfo->index   = 1;

    return 0;
}

/* Module interface initialization. */
static int 
InitFuncPt(int index, void *pt)
{
    tRobotItf *itf  = (tRobotItf *)pt;

    itf->rbNewTrack = initTrack;     /* Give the robot the track view called */
                                    /* for every track change or new race */
    itf->rbNewRace  = newrace;      /* Start a new race */
    itf->rbDrive    = drive;        /* Drive during race */
    itf->rbPitCmd   = NULL;
    itf->rbEndRace  = endrace;      /* End of the current race */
    itf->rbShutdown = shutdown;     /* Called before the module is unloaded */
    itf->index      = index;        /* Index used if multiple interfaces */
    return 0;
}

/* Called for every track change or new race. */
static void  
initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s)
{
    curTrack = track;
    *carParmHandle = NULL;
}


#if TRAINING_MODE

void adjust_learning_parameters()
{
    int learning_stage = 0;

    // get learning stage from training_race_counter by comparing the first column
    // of LEARNING_PARAMETERS with training_race_counter
    while( (learning_stage < controller::LEARNING_STAGES) &&
            (controller::training_race_counter >
            controller::LEARNING_PARAMETERS[learning_stage][0]) )
    {
        learning_stage++;
    }

    // set parameters for corresponding learning_stage
    m_q_learner.set_learning_rate(controller::LEARNING_PARAMETERS[learning_stage][1]);
    m_q_learner.set_discount(controller::LEARNING_PARAMETERS[learning_stage][2]);
    m_q_learner.set_epsilon(controller::LEARNING_PARAMETERS[learning_stage][3]);
}

#endif


/* Start a new race. */
static void  
newrace(int index, tCarElt* car, tSituation *s)
{
    setbuf(stdout, NULL);

    sprintf(QLearner_File, Q_VALUE_FILE_NAME_FORMAT, Q_VALUE_FILE_NAME(curTrack->name));

#ifdef TRAINING_MODE

    // try to read values from Q file for the first time
    // this race counter is Zero when initialized at game start
    if(controller::training_race_counter == 0)
    {
        controller::training_race_counter = controller::_Q_maps_storage.
            _Q_maps->load_maps_from_file(QLearner_File);

        printf("training counter set to - %d\n", controller::training_race_counter);

        printf("reward configuration set to - %s\n", RACE_REWARD_ID);
    }

    adjust_learning_parameters();

#else

    // in RACE_MODE loads Q File for each new race
    // files would be different for different tracks
    controller::_Q_maps_storage._Q_maps->load_maps_from_file(QLearner_File);

#endif

    // reset previous damages
    prev_damages = 0;

    // reset distance raced
    distance_raced = 0;
}


/* Drive during race. */
static void  
drive(int index, tCarElt* car, tSituation *s)
{
    // calculate steering angle
    float angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
    NORM_PI_PI(angle);
    angle -= SC*car->_trkPos.toMiddle / car->_trkPos.seg->width;

    // set fuzzy inputs
    controller::fuzzy_inputs _fuz_inputs;
    _fuz_inputs.speed = car->_speed_x;
    _fuz_inputs.acceleration = car->_accel_x;
    _fuz_inputs.path = angle / car->_steerLock;

    // calculate next segment turn angle if next segment is curved
    if (car->_trkPos.seg->next->radius != 0)
    {
        // positive for left curve as right-side radius would be greater
        _fuz_inputs.next_path = (car->_trkPos.seg->next->radiusr
                - car->_trkPos.seg->next->radiusl) / (car->_trkPos.seg->next->radius);
    }
    else
    {
        // set next segment turn angle to 0
        // when next segment is not curved
        _fuz_inputs.next_path = 0;
    }

    // for version "v1.0.0" this is left to 0
    _fuz_inputs.stability = 0;

    // run engine for fuzzy outputs
    controller::fuzzy_outputs _fuz_outputs = m_fuzzy_controller.get_output(&(_fuz_inputs));

    // set outputs
    car->ctrl.gear = _fuz_outputs.gear;
    car->ctrl.steer = _fuz_outputs.steer;
    car->ctrl.brakeCmd = _fuz_outputs.brake;
    car->ctrl.accelCmd = _fuz_outputs.accel;

    controller::Q_state t_Q_state;
    t_Q_state.speed_x = car->_speed_x;
    // clip speed y to (-SPEED_Y_CLIP_VALUE) and (+SPEED_Y_CLIP_VALUE)
    // very high values of speed y are not much relevant
    t_Q_state.speed_y = clip_min_max(car->_speed_y,
                -SPEED_Y_CLIP_VALUE, SPEED_Y_CLIP_VALUE);

    // large distances on either side do not have much information,
    // so clip right side distance on track to
    // (-TRACK_SIDE_CLIP_DISTANCE) and (+TRACK_SIDE_CLIP_DISTANCE)
    t_Q_state.right_side_distance =
        (int) clip_min_max(car->_trkPos.toRight,
                -TRACK_SIDE_CLIP_DISTANCE, TRACK_SIDE_CLIP_DISTANCE);
    // large distances on either side do not have much information,
    // so clip left side distance on track to
    // (-TRACK_SIDE_CLIP_DISTANCE) and (+TRACK_SIDE_CLIP_DISTANCE)
    t_Q_state.left_side_distance =
        (int) clip_min_max(car->_trkPos.toLeft,
                -TRACK_SIDE_CLIP_DISTANCE, TRACK_SIDE_CLIP_DISTANCE);

    // values are same as those for the fuzzy inputs
    t_Q_state.path = _fuz_inputs.path;
    t_Q_state.next_path = _fuz_inputs.next_path;

    // suggested accel value by Q Learner overrides accelCmd
    controller::Q_action suggested_action;
    m_q_learner.get_suggested_action(t_Q_state, suggested_action);
    car->ctrl.accelCmd = suggested_action.accel;

#ifdef TRAINING_MODE

    /**
     * while driving in TRAINING_MODE :
     *  1. state, action, rewards are updated in Q Learner
     *  2. race ends as soon as the car goes outside the track
     **/

    // get reward for being in current state
    float reward = get_reward(car);
    controller::Q_action t_Q_action;
    t_Q_action.accel = car->ctrl.accelCmd;

    // check if it is outside the track
    if(car->_trkPos.toRight < 0 || car->_trkPos.toLeft < 0)
    {
        // end the race if it is outside the track
        car->_state = RM_RACE_ENDED;
        ReInfo->s->_raceState = RM_RACE_ENDED;

        // race ends here and this state will not be updated
        // so set this state as the terminal state
        t_Q_state.speed_x = -99;     // largest negative speed_x "-99" to mark end state
        t_Q_state.speed_y = 0;
        t_Q_state.right_side_distance = -1.0;
        t_Q_state.left_side_distance = -1.0;
        t_Q_state.path = 0;
        t_Q_state.next_path = 0;

        // set default action for end state
        t_Q_action.accel = 0;
    }

    // update state, action and reward
    m_q_learner.set_state_action_and_reward(t_Q_state, t_Q_action, reward);

#endif

    // update distance raced
    distance_raced = car->_distRaced;
}

/* End of the current race */
static void
endrace(int index, tCarElt *car, tSituation *s)
{
}

/* Called before the module is unloaded */
static void
shutdown(int index)
{

#ifdef TRAINING_MODE

    controller::training_race_counter++; 

    // make sure it is still learning before modifying values in Q map storage
    if(controller::training_race_counter <=
            controller::LEARNING_PARAMETERS[controller::LEARNING_STAGES - 1][0])
    {
        // update training race counter in Q maps
        controller::_Q_maps_storage._Q_maps->m_training_counter =
            controller::training_race_counter;

        // write to file after each WRITE_AFTER_N_RACES
        if(controller::training_race_counter % WRITE_AFTER_N_RACES == 0)
        {
            controller::_Q_maps_storage._Q_maps->write_maps_to_file(
                    QLearner_File, controller::training_race_counter);
        }
    }

    printf("*** shutdown TRAINING RACE NO. - %d   *** total distance raced - %f\n",
            controller::training_race_counter, distance_raced);

#else

    printf("*** shutdown *** total distance raced - %f\n", distance_raced);

#endif
}

