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
 * car111.cpp
 *
 * The autonomous car with fuzzy control system.
 * The skeleton of this file was originally generated
 * from "robotgen" script that came with "torcs-1.3.7".
 *
 *     version  : 1.0.0
 *  created on  : 23 May 2018
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


static tTrack    *curTrack;

static controller::FuzzyController m_fuzzy_controller;

static const int SC = 1;
static float distance_raced = 0;


static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s);
static void newrace(int index, tCarElt* car, tSituation *s);
static void drive(int index, tCarElt* car, tSituation *s);
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt);


/* 
 * Module entry point  
 */
extern "C" int 
car111(tModInfo *modInfo)
{
    memset(modInfo, 0, 10*sizeof(tModInfo));

    modInfo->name    = strdup("car111");                    /* name of the module */
    modInfo->desc    = strdup(
            "Autonomous vehicle using fuzzy control");      /* description */

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

/* Start a new race. */
static void  
newrace(int index, tCarElt* car, tSituation *s)
{
    // reset distance raced
    distance_raced = 0;
}

/* Drive during race. */
static void  
drive(int index, tCarElt* car, tSituation *s)
{
    // clear previous values
    memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));

    // calculate steering angle
    float angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
    NORM_PI_PI(angle);
    angle -= SC*car->_trkPos.toMiddle / car->_trkPos.seg->width;

    // set fuzzy inputs
    controller::fuzzy_inputs inputs;
    inputs.speed = car->_speed_x;
    inputs.acceleration = car->_accel_x;
    inputs.path = angle / car->_steerLock;

    // calculate next segment turn angle if next segment is curved
    if (car->_trkPos.seg->next->radius != 0)
    {
        // positive for left curve as right-side radius would be greater
        inputs.next_path = (car->_trkPos.seg->next->radiusr
                - car->_trkPos.seg->next->radiusl) / (car->_trkPos.seg->next->radius);
    }
    else
    {
        // set next segment turn angle to 0
        // when next segement is not curved
        inputs.next_path = 0;
    }

    // for version "v1.0.0" this is left to 0
    inputs.stability = 0;

    // run engine for fuzzy outputs
    controller::fuzzy_outputs outputs = m_fuzzy_controller.get_output(&inputs);

    // set outputs
    car->ctrl.gear = outputs.gear;
    car->ctrl.steer = outputs.steer;
    car->ctrl.brakeCmd = outputs.brake;
    car->ctrl.accelCmd = outputs.accel;

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
    printf("*** shutdown *** total distance raced - %f\n", distance_raced);
}

