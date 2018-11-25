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
 * race_reward.cpp
 *
 *     version  : 1.0.0
 *  created on  : 12 Jul 2018
 *      author  : M.S.Khan
 */


#include <cmath>

#include "race_reward.h"


/* total damages incurred until current step */
float prev_damages;


/* 
 * ===  FUNCTION  ============================================================
 *         Name:  get_reward
 *  Description:  It returns reward for car being in current state ( how fast
 *                it is moving, whether it is outside the track, whether
 *                there was any damage ).
 * ===========================================================================
 */
float get_reward(tCarElt * car)
{
    //reward accumulated in current step
    float reward = 0;
    //damage incurred in current step
    float damage_for_step = 0;

    // penalize if there is damage in current step
    if(car->_dammage > prev_damages)
    {
        // damage incurred in this step
        damage_for_step = car->_dammage - prev_damages;

        // penalty is a product of DAMAGE_COEFFICIENT and damage for this step
        reward -= DAMAGE_COEFFICIENT*damage_for_step;

        // reset previous damages to total damages so far
        prev_damages = car->_dammage;
    }


    // if outside of right side of the track
    if(car->_trkPos.toRight < 0)
    {
        // reward is negative when outside
        reward -= PENALTY_FOR_GOING_OUT;
    }
    // else if outside of left side of the track
    else if(car->_trkPos.toLeft < 0)
    {
        // reward is negative when outside
        reward -= PENALTY_FOR_GOING_OUT;
    }
    else    // if inside the track
    {
        // reward and penalty for speed only if there was no foul, i.e.
        // - no damage incurred in current step
        // - moving inside the track
        if(damage_for_step == 0)
        {
            // for simplicity - get rid of the fractional part of the speed
            int speed_x = (int) car->_speed_x;

            // this version of reward system does not assume reverse gear as necessary
            // penalize for going slow or reverse
            if(speed_x < MIN_SLOW_SPEED)
            {
                // negative cubic proportional reward for going slower
                // than MIN_SLOW_SPEED
                reward -= std::pow(( MIN_SLOW_SPEED - speed_x ), 3)
                    *SLOW_SPEED_PENALTY_COEFFICIENT;
            }
            else    // positive reward for moving faster
            {
                // there is always more reward for moving faster as long as
                // there is no damage and is moving inside the track
                reward += ( speed_x )*SPEED_UNIT_REWARD;

                // extra reward for moving with high speed
                // when it is alteast equal to HIGH_SPEED_CUTOFF
                if(speed_x >= HIGH_SPEED_CUTOFF)
                {
                    // cubic proportional reward for going faster
                    reward += std::pow(( speed_x - HIGH_SPEED_CUTOFF ), 3)
                        *HIGH_SPEED_REWARD_COEFFICIENT;
                }

            }
        }
    }

    return reward;
}


