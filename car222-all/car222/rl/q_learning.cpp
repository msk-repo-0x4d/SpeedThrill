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
 * q_learning.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: M.S.Khan
 */


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>

#include "car222_string_formats.h"
#include "car222_Q_maps.h"
#include "q_learning.h"



/* constructor for Q_state */
controller::struct_Q_state::struct_Q_state() :
    speed_x(0), speed_y(0),
    right_side_distance(0),
    left_side_distance(0),
    path(0), next_path(0)
{ }


/* constructor for Q_action */
controller::struct_Q_action::struct_Q_action() :
    accel(0)
{ }


/* returns string representation of the Q_state */
std::string controller::Q_state::get_string() const
{
    char value_arr[64];

    sprintf(value_arr, STATE_PRINT_FORMAT,
            speed_x, speed_y,
            right_side_distance,
            left_side_distance,
            path, next_path);

    return std::string(value_arr);
}


/* returns string representation of the Q_action */
std::string controller::Q_action::get_string() const
{
    char value_arr[16];
    sprintf(value_arr, ACTION_PRINT_FORMAT, accel);
    return std::string(value_arr);
}


controller::QLearner::QLearner(
        controller_storage::Q_maps_storage & a_ref_Q_maps_storage,
        float t_learning_rate, float t_discount, float t_epsilon)
{
    m_learning_rate = t_learning_rate;
    m_discount = t_discount;
    m_epsilon = t_epsilon;

    ref_Q_maps_storage = &a_ref_Q_maps_storage;

    m_current_state_reward = 0;
    m_current_state = new Q_state;
    m_next_state = new Q_state;
    m_current_action = new Q_action;
    m_next_action = new Q_action;
}


controller::QLearner::~QLearner()
{
    delete m_current_state;
    delete m_next_state;
    delete m_current_action;
    delete m_next_action;
}


/**
 * select an action that has not been tried i.e. select an action
 * such that the action is not there in the given "all_action_record"
 * (actions that are not there in the "all_action_record" have not been
 * tried and have zero Q value). Also, "all_actions_record" is supposed to
 * be a "proper subset" of the action space. The values of the selected
 * action are copied to the second argument "selected_action".
 **/
void select_action_not_tried(
        const std::vector<std::string> & all_actions_record,
        controller::Q_action & selected_action)
{
    selected_action.accel = 0;

    // scan the float values from the string values
    std::vector<float> actions_in_record;
    for(std::vector<std::string>::const_iterator
            record_iterator = all_actions_record.begin();
            record_iterator != all_actions_record.end();
            record_iterator++)
    {
        float an_action_in_record;
        sscanf(record_iterator->c_str(),
                STATE_MASK ACTION_READ_FORMAT,
                &(an_action_in_record));
        actions_in_record.push_back(an_action_in_record);
    }

    // move over each action in the entire action space and
    // try to find an action that is not there in the records
    int test_action_index = 0;     // starting index of action space
    while(test_action_index < controller::TOTAL_NUM_ACTIONS)
    {
        float test_action = controller::values_0_to_1_in_9_steps[test_action_index];

        int matched = 0;
        // iterate over each action in the records
        for(std::vector<float>::iterator
                record_iterator = actions_in_record.begin();
                record_iterator != actions_in_record.end();
                record_iterator++)
        {
            // in later version this equality check could
            // be a check for very small difference in floating value
            if(test_action == *record_iterator)
            {
                // action matched a record so
                // no need to match other action in records
                matched = 1;
                break;
            }
        }

        if(!matched)
        {
            // no record matched this test action so search ends here
            // as found an action that is not there in the action records
            selected_action.accel = test_action;
            break;
        }

        test_action_index++;
    }
}


/**
 * get second argument as the suggested action for given state
 * using ε-greedy (epsilon-greedy) policy
 **/
void controller::QLearner::get_suggested_action(
        const Q_state& given_state,
        Q_action & suggested_action)
{
    // string representation of the given state
    const std::string state_string = given_state.get_string();
    // the map that has Q values for this state
    const std::map<std::string, float> state_Q_map =
        ref_Q_maps_storage->_Q_maps->get_map_for(state_string);

    // records of Q value of the given state for various actions
    std::vector<std::string> all_matching_records;
    // default max Q value (if no records for the given state is found)
    float max_Q_value = 0;
    std::string action_with_max_Q;
    for(std::map<std::string, float>::const_iterator
            map_iterator = state_Q_map.begin();
            map_iterator != state_Q_map .end();
            map_iterator++) 
    {
        if(map_iterator->first.find(state_string) == 0)
        {
            if(max_Q_value < map_iterator->second || action_with_max_Q.empty())
            {
                action_with_max_Q = map_iterator->first;
                max_Q_value = map_iterator->second;
            }

            // add to records of Q value for this state
            all_matching_records.push_back(map_iterator->first);
        }
    }

    float rand_value = (float) rand()/RAND_MAX;
    if(rand_value >= m_epsilon)    // get action with best Q value
    {
        // check if there are some actions still left to be tried
        // as their Q value would be zero which is better than negative Q value
        if(all_matching_records.size() < TOTAL_NUM_ACTIONS)
        {
            // check if no action has been tried yet for this state
            // to select a random action
            if(action_with_max_Q.empty())
            {
                // pick from random array index
                suggested_action.accel = controller::
                    values_0_to_1_in_9_steps[(rand() % TOTAL_NUM_ACTIONS)];
            }
            else if(max_Q_value >= 0)
                // if positive Q value choose this action with max Q value as
                // other untried actions have still zero Q values
            {
                // choose the best action found
                sscanf(action_with_max_Q.c_str(),
                        STATE_MASK "%f", &(suggested_action.accel));
            }
            else // maps have all negative Q values as max_Q_value < 0
                // select an action that is not tried
            {
                select_action_not_tried(all_matching_records, suggested_action);
            }
        }
        else // no more untried actions left (even if max Q value is negative
             // still no untried action with zero Q value is left to be tried)
        {
            // select the best action found as there are no more actions to try
            sscanf(action_with_max_Q.c_str(),
                    STATE_MASK "%f", &(suggested_action.accel));
        }
    }
    else    // explore by choosing a random action
    {
        suggested_action.accel = controller::
            values_0_to_1_in_9_steps[(rand() % TOTAL_NUM_ACTIONS)];
    }
}



#ifdef TRAINING_MODE


/* update Q value for current state and action pair */
void controller::QLearner::update_Q_value()
{
    // string representation of current state and action
    const std::string current_state_string = m_current_state->get_string();
    const std::string current_action_string = m_current_action->get_string();

    // max Q value for next state
    const float max_Q_value_for_next_state = ref_Q_maps_storage->
        _Q_maps->get_max_Q_value_for(m_next_state->get_string());

    // Q value for state and action pair before updating
    const float t_Q_value_for_state_action = ref_Q_maps_storage->
        _Q_maps->get_Q_value_for(current_state_string, current_action_string);

    // calculate updated Q value
    const float updated_Q_value_for_current_state_current_action
        = ((1 - m_learning_rate) * t_Q_value_for_state_action)
        + (m_learning_rate *
                (m_current_state_reward + (m_discount * max_Q_value_for_next_state)));

    // update Q value in the map
    ref_Q_maps_storage->_Q_maps->update_Q_value_for(
            current_state_string, current_action_string,
            updated_Q_value_for_current_state_current_action);
}


void controller::QLearner::set_state_action_and_reward(
        const Q_state & given_state,
        const Q_action & given_action,
        float given_state_reward)
{
    // update current state and action
    // to previously stored next state and action
    *m_current_state = *m_next_state;
    *m_current_action = *m_next_action;

    // use the given state action pair
    // as next state and next action
    *m_next_state = given_state;
    *m_next_action = given_action;

    // this reward is observed after taking
    // current action in current state
    m_current_state_reward = given_state_reward;

    // update Q value for current state-action pair
    // with the given next state and given reward
    update_Q_value();
}


#endif


