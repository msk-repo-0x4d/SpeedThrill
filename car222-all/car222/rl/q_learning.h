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
 * q_learning.h
 *
 *  Created on: Mar 13, 2018
 *      Author: M.S.Khan
 */

#ifndef  Q_LEARNING_H_
#define  Q_LEARNING_H_

#include <string>

#include "car222_Q_maps.h"


#define Q_LEARNER_VERSION     "v1.0.0"
#define Q_LEARNER_ID          "QLearner-Accel-" Q_LEARNER_VERSION


#define DEFAULT_LEARNING_RATE  1.0/4
#define DEFAULT_DISCOUNT_RATE  1020.0/1024
#define DEFAULT_EPSILON        1.0/1024


namespace controller
{

    // action space is 9 equal intervals in [0,1]
    static const int TOTAL_NUM_ACTIONS = 9;
    static const float values_0_to_1_in_9_steps[TOTAL_NUM_ACTIONS]
        = {0.0, 1.0/8, 2.0/8, 3.0/8, 4.0/8, 5.0/8, 6.0/8, 7.0/8, 8.0/8};


    /* struct for describing state of a Q Learner */
    typedef struct struct_Q_state
    {

        // to reduce state space using int approximation for
        // speed_x, right_side_distance, left_side_distance
        int speed_x;                // speed in x direction
        float speed_y;              // speed in y direction
        int right_side_distance;    // distance to track edge on right side
        int left_side_distance;     // distance to track edge on left side
        float path;                 // curvature of current track segment
        float next_path;            // curvature of next track segment

        struct_Q_state();

        // string representation of the state
        std::string get_string() const;

    } Q_state;


    /* struct for describing action of a Q Learner */
    typedef struct struct_Q_action
    {

        float accel;                // value for acceleration pedal

        struct_Q_action();

        // string representation of the action
        std::string get_string() const;

    } Q_action;


    /*
     * ==============================================================================
     *        Class:  QLearner
     *  Description:  This class represents a Q Learner. It has sets of states,
     *                actions and a reward value. In current implementation, it
     *                only holds a reference to Q maps that contain Q values of
     *                known state and action pair. In these maps, it can retrieve
     *                and update Q values for state and action pairs.
     *
     *                It also has a method to get an update for the current state
     *                of the QLearner, the reward that it has got in this state
     *                and the action that is being taken. It also implements an
     *                epsilon-greedy policy that generates action for a given
     *                state which it returns as a suggested action (this action may
     *                or may not be taken in actual).
     * ==============================================================================
     */
    class QLearner
    {
        public:

            /*--------------------------------------------------------------
             *                 MEMBER FUNCTIONS
             *--------------------------------------------------------------*/

            /**
             * constructor with
             * reference to a Q_maps_storage that stores the Q values,
             * learning rate (default is DEFAULT_LEARNING_RATE) for updating Q value,
             * discount (default is  DEFAULT_DISCOUNT_RATE) for updating Q value,
             * epsilon (default is DEFAULT_EPSILON) for policy.
             **/
            explicit QLearner(
                    controller_storage::Q_maps_storage & a_ref_Q_maps_storage,
                    const float t_learning_rate = DEFAULT_LEARNING_RATE,
                    const float t_discount = DEFAULT_DISCOUNT_RATE,
                    const float t_epsilon = DEFAULT_EPSILON);

            ~QLearner();

            /**
             * modifies second argument as the suggested action for given state
             * using ε-greedy (epsilon-greedy) policy
             **/
            void get_suggested_action(const Q_state & given_state,
                    Q_action & suggested_action);

#ifdef TRAINING_MODE

            /**
             * updates the state-action pair and Q value for QLearner.
             * It updates the Q value for the previous step's state and action
             * using the given state and action as its next state and action
             * and given reward as its reward for previous action in previous state.
             *
             * Note :- It lags one step behind for updating Q value of the given state
             * as it waits to find out the next state and reward.
             **/
            void set_state_action_and_reward(
                    const Q_state & given_state,
                    const Q_action & given_action,
                    float given_state_reward);

            /* get learning rate α */
            float get_learning_rate()
            {
                return m_learning_rate;
            }

            /* get discount value γ */
            float get_discount()
            {
                return m_discount;
            }

            /* get exploration value epsilon ε */
            float get_epsilon()
            {
                return m_epsilon;
            }

            /* set learning rate α */
            void set_learning_rate(const float t_learning_rate)
            {
                m_learning_rate = t_learning_rate;
            }

            /* set discount value γ */
            void set_discount(const float t_discount)
            {
                m_discount = t_discount;
            }

            /* set exploration value epsilon ε */
            void set_epsilon(const float t_epsilon)
            {
                m_epsilon = t_epsilon;
            }

#endif

        private:

            /*--------------------------------------------------------------
             *                 MEMBER VARIABLES
             *--------------------------------------------------------------*/

            /* a reference to Q map storage */
            controller_storage::Q_maps_storage * ref_Q_maps_storage;

            /* current state */
            Q_state * m_current_state;
            /* current action */
            Q_action * m_current_action;
            /* Reward in current state */
            float m_current_state_reward;
            /* next state */
            Q_state * m_next_state;
            /* next action */
            Q_action * m_next_action;

            /* learning rate α */
            float m_learning_rate;
            /* discount value γ */
            float m_discount;
            /* exploration rate ε */
            float m_epsilon;

            /*--------------------------------------------------------------
             *                 MEMBER FUNCTIONS
             *--------------------------------------------------------------*/

#ifdef TRAINING_MODE

            /* update Q value for current state and action pair */
            void update_Q_value();

#endif

            /* copy constructor */
            QLearner(const QLearner & other) = delete;

            /* assignment operator */
            QLearner& operator=(const QLearner &other) = delete;

    };

}

#endif    /* ifndef Q_LEARNING_H_ */

