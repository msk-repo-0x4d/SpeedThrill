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
 * race_reward.h
 *
 *     version  : 1.0.0
 *  created on  : 12 Jul 2018
 *      author  : M.S.Khan
 */


#ifndef RACE_REWARD_H_
#define RACE_REWARD_H_

#include <car.h>


// id of this reward configuration
#define  RACE_REWARD_ID               "1.1.0-10.0-5-45-1.0D8-1.0D1024-10000-1.0D256"


// to calculate penalty for damage at any step
#define  DAMAGE_COEFFICIENT                 10.0
// minimum slow speed that is not penalized
#define  MIN_SLOW_SPEED                     5
// min speed that is rewarded as high speed
#define  HIGH_SPEED_CUTOFF                  45
// penalty coefficient for moving slower
#define  SLOW_SPEED_PENALTY_COEFFICIENT     1.0/8
// reward coefficient for moving very fast
#define  HIGH_SPEED_REWARD_COEFFICIENT      1.0/1024
// penalty for moving out of the track
#define  PENALTY_FOR_GOING_OUT              10000
// reward for unit speed gain
#define  SPEED_UNIT_REWARD                  1.0/256


// total damages incurred until current step
extern float prev_damages;

// returns reward for car being in current state
extern float get_reward(tCarElt * car);


#endif      /** ifndef RACE_REWARD_H_ **/


