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
 * car222_Q_maps.h
 *
 *  Created on: Apr 17, 2018
 *      Author: M.S.Khan
 */


#ifndef  CAR222_Q_MAPS_H_
#define  CAR222_Q_MAPS_H_


#include <string>
#include <map>
#include <vector>


// version of Q_maps
#define Q_MAPS_VERSION "v1.0.0"

// number of rows in Q map array
#define Q_MAP_ARRAY_ROWS 6
// number of column in Q map array
#define Q_MAP_ARRAY_COLUMNS 10


namespace controller_storage
{

    /**
     * pair containing max Q value and the corresponding action
     **/
    typedef struct max_Q_and_action_struct
    {

        float max_Q_value;
        float max_Q_action;

    } max_Q_and_action;

    /**
     * maps for storing Q values for state-action pair
     **/
    typedef std::map<std::string, float> state_action_Q_map;

    /**
     * map for storing max Q value and action (as float values) for each state
     **/
    typedef std::map<std::string, max_Q_and_action * > state_max_Q_and_action_map;

    /*
     * ==========================================================================
     *        Class:  Q_maps
     *  Description:  The purpose of this class is to store large Q value maps
     *                in an organized way to facilitate quicker and convenient
     *                retrieval and update of the maps. It also provides
     *                methods to read and write these maps to/from files along
     *                with other relevant information while reading or writing
     *                from file.
     * ===========================================================================
     */
    class Q_maps
    {
        public :

            /** MEMBER VARIABLE **/

            /* training stage described by the counter */
            long long int m_training_counter;
            /* file name that is currently loaded */
            std::string m_Q_value_file_name;


            /** MEMBER FUNCTIONS **/

            Q_maps();

            ~Q_maps();

            /**
             *  loads maps of Q values from given file
             *  (also sets the "m_Q_value_file_name" when file is loaded successfully).
             *  It returns one of the 2 values -
             *    - 0 for empty file or for error reading file
             *    - a training counter found in the file
             **/
            long long int load_maps_from_file(const std::string & t_Q_value_file_name);

            /**
             * returns Q value for given state and action pair
             * (returns 0 if the pair was not found in maps).
             **/
            float get_Q_value_for(const std::string & state_string,
                    const std::string & action_string) const;

            /**
             * returns max Q value for given state
             * (returns 0 if the state name was not found in the max Q value map).
             * It uses max Q and action map to return the max Q value. It does not
             * consider Q values of untried actions as it has no knowledge of the
             * action space. For more information look into "check_and_update_max_Q"
             * method.
             **/
            float get_max_Q_value_for(const std::string & state_string) const;

            /**
             * returns the Q value map for a given state. It uses characters
             * at position 1 and 2 of the state string (positions are 0-based)
             * and internally calls the private method "get_map_for" with these
             * characters. It differs from the private method in arguments
             * as well as the const qualification of return type. This method
             * returns a const map while the private method returns a non-const map.
             *
             * For example, if a state is as shown below :
             *
             *        +15|+0.4|+06|+06|-000.6|-000.2
             *         ↑↑
             *
             * then the characters at position 1, 2 are "1" and "5".
             * (these correspond to velocity_x value of the state and for each
             * of these character combinations there is a separate map).
             * It will return the Q value map that has states with characters "1"
             * and "5" at position 1 and 2.
             **/
            inline const state_action_Q_map & get_map_for(
                    const std::string & state_string) const
                {
                    return get_map_for(state_string.at(1), state_string.at(2));
                }

            /**
             * get total size of the current Q value maps.
             * If "print_info" is ON ( non-zero ) then prints size of all the maps.
             * "print_info" default is OFF ( zero ).
             **/
            long long int get_total_size(const int print_info = 0) const;

            /**
             * updates Q value for state and action pair
             * also updates max Q and action map
             **/
            void update_Q_value_for(const std::string & state_string,
                    const std::string & action_string, const float t_Q_value);

            /* write Q maps to file (optionally overwrite training_race_counter) */
            int write_maps_to_file(const std::string & t_Q_value_file_name,
                    const long long int race_counter = 0);


        private :

            /** MEMBER VARIABLES **/

            /**
             * a fixed length array of pointers to maps containing state-action and Q value.
             *
             * There are numerous state-action to Q value maps that are stored in this array.
             * For a given state-action pair, the two characters at position 1 and 2
             * ( positions are 0-based and these positions correspond to the velocity-x
             * in a state ), decide the map that is selected for storing the state-action and
             * Q value.
             *
             * So, the two states shown below would be stored in two different maps
             *
             *  +15|+0.4|+06|+06|-000.6|-000.2
             *  +35|+0.4|+06|+06|-000.6|-000.2
             *
             * This is because at position 1 and 2 the digits "1", "5" in first state are
             * different than digits "3", "5" in second state.
             *
             * While the two states shown below would be stored in the same map
             *
             *  +25|+0.4|+02|+06|-000.2|-000.3
             *  +25|+0.3|+04|+06|+000.1|+000.2
             *
             * This is because at position 1 and 2 the digits "2", "5" are same.
             *
             * Right now in version "v1.0.0" there are 60 maps for velocities 00 to 59 (sign
             * character is ignored). The states with characters other than [00, 59] at
             * position 1 and 2 are stored in a separate map "default_velocity_Q_map_pointer".
             **/
            state_action_Q_map *
                velocity_Q_map_pointers[Q_MAP_ARRAY_ROWS][Q_MAP_ARRAY_COLUMNS];

            /* pointer to default map for state with velocities that are out of range */
            state_action_Q_map * default_velocity_Q_map_pointer;

            /**
             * pointer to map for state's max Q value and action.
             * In sooner revision this should expand to an array of pointers similar to
             * "velocity_Q_map_pointers".
             **/
            state_max_Q_and_action_map * max_Q_and_action_map_pointer;


            /** MEMBER FUNCTIONS **/

            /**
             * returns the Q value map for a state with given characters
             * at position 1 and 2 of the state string (positions are 0-based).
             *
             * For example, if a state is as shown below :
             *
             *        +15|+0.4|+06|+06|-000.6|-000.2
             *         ↑↑
             *
             * then the characters at position 1, 2 are "1" and "5".
             * (these correspond to velocity_x value of the state and for each
             * of these character combinations there is a separate map).
             * It will return the Q value map that has states with "1" and "5" at position
             * 1 and 2.
             *
             * This method differs from public method "get_map_for" in arguments as well
             * as the const-qualification of the return type. The public method returns
             * a const map while this method returns a non-const map.
             **/
            inline state_action_Q_map & get_map_for(
                        const char & map_key_first_char,
                        const char &  map_key_second_char) const
                {
                    int index_1 = map_key_first_char - '0';
                    int index_2 = map_key_second_char - '0';

                    // check for bounds
                    if(index_1 < 0 || index_1 >= Q_MAP_ARRAY_ROWS ||
                            index_2 < 0 || index_2 >= Q_MAP_ARRAY_COLUMNS)
                    {
                        // for out of bound return default map
                        return *default_velocity_Q_map_pointer;
                    }

                    // return intended map from the array of maps
                    return *(velocity_Q_map_pointers[index_1][index_2]);
                }

            /**
             * clears and fills all the map pointers (also the default map)
             * to the map list
             **/
            void get_all_Q_value_maps(std::vector<state_action_Q_map *> & map_list) const;

            /**
             * check and update max Q value and action for given state
             *
             * It updates the max Q and action map for the given state.
             * While finding the max Q value for given state, any action whose Q value
             * is not there in the state Q value map (i.e. the Q values of untried
             * action) is not considered for max comparison. This is because it has no
             * knowledge of action space and only knows Q values for actions that are
             * there in the state Q value map.
             **/
            void check_and_update_max_Q(const std::string & state_name,
                    const std::string & action_name,
                    const float t_Q_value,
                    const std::map<std::string, float> & state_Q_value_map);

            // restricted copy constructor
            Q_maps(const Q_maps &other) = delete;

            // restricted assignment operator
            Q_maps& operator=(const Q_maps &other) = delete;

    };


    /*
     * ===========================================================================
     *       Struct:  Q_maps_storage
     *  Description:  Wrapper struct for Q_maps class
     *
     * ===========================================================================
     */
    typedef struct Q_maps_storage_struct
    {
        // pointer to Q_maps object
        Q_maps * _Q_maps;

        Q_maps_storage_struct()
        {
            _Q_maps = new Q_maps;
        }

        ~Q_maps_storage_struct()
        {
            if(_Q_maps != NULL)
            {
                delete _Q_maps;
                _Q_maps = NULL;
            }
        }

    } Q_maps_storage;

}


#endif      /* ifndef CAR222_Q_MAPS_H_ */


