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
 * car222_Q_maps.cpp
 *
 *  Created on: Jul 29, 2018
 *      Author: M.S.Khan
 */


#include <stdio.h>
#include <string>
#include <map>
#include <vector>

#include "car222_string_formats.h"
#include "car222_Q_maps.h"


controller_storage::Q_maps::Q_maps()
{
    // initialize training counter and Q value file name
    m_training_counter = 0;
    m_Q_value_file_name = "";

    // velocity Q map pointers
    for(int i = 0; i < Q_MAP_ARRAY_ROWS; i++)
    {
        for(int j = 0; j < Q_MAP_ARRAY_COLUMNS; j++)
        {
            velocity_Q_map_pointers[i][j] = new state_action_Q_map;
        }
    }

    // default velocity Q map pointer
    default_velocity_Q_map_pointer = new state_action_Q_map;

    // map for state to max Q value and corresponding action
    max_Q_and_action_map_pointer = new state_max_Q_and_action_map;
}


controller_storage::Q_maps::~Q_maps()
{
    // delete max Q and action map values
    for(std::map<std::string, max_Q_and_action * >::iterator
            max_iterator = max_Q_and_action_map_pointer->begin();
            max_iterator != max_Q_and_action_map_pointer->end();
            max_iterator++)
    {
        delete max_iterator->second;
    }
    // delete max Q and action map pointer
    delete max_Q_and_action_map_pointer;


    // delete velocity Q map pointers
    for(int i = 0; i < Q_MAP_ARRAY_ROWS; i++)
    {
        for(int j = 0; j < Q_MAP_ARRAY_COLUMNS; j++)
        {
            delete velocity_Q_map_pointers[i][j];
        }
    }

    // delete default velocity Q map pointer
    delete default_velocity_Q_map_pointer;
}


float controller_storage::Q_maps::get_Q_value_for(
        const std::string & state_string,
        const std::string & action_string) const
{
    // get the map for the given state
    std::map<std::string, float> state_Q_value_map =
        get_map_for(state_string);
    // get the combined map key for state and action
    const std::string map_key = state_string + '|' + action_string;

    std::map<std::string, float>::const_iterator
        iterator_to_searched_key = state_Q_value_map.find(map_key);

    // return 0 if not found else return the value
    return (iterator_to_searched_key == state_Q_value_map.end())
        ? 0 : iterator_to_searched_key->second;
}


float controller_storage::Q_maps::get_max_Q_value_for(
        const std::string & state_name) const
{
    state_max_Q_and_action_map::const_iterator iterator_max_to_searched_state =
        max_Q_and_action_map_pointer->find(state_name);

    // return 0 if state is not found else return the max Q value
    return (iterator_max_to_searched_state == max_Q_and_action_map_pointer->end())
        ? 0 : iterator_max_to_searched_state->second->max_Q_value;
}


void controller_storage::Q_maps::check_and_update_max_Q(
        const std::string & state_name,
        const std::string & action_name,
        const float given_Q_value,
        const std::map<std::string, float> & state_Q_value_map)
{
    // update max Q value and max Q value action for this state if -
    // 1. state is not there in the max_Q_and_action_map
    // 2. max Q value for the state has changed
    // 3. Q value for max Q action has changed

    // scan action as float value
    float action_as_float_value;
    sscanf(action_name.c_str(), ACTION_READ_FORMAT, &(action_as_float_value));

    state_max_Q_and_action_map::iterator iterator_max_to_searched_state =
        max_Q_and_action_map_pointer->find(state_name);

    // check if this state is not there in the map
    if(iterator_max_to_searched_state == max_Q_and_action_map_pointer->end())
    {
        // add this state and set iterator to newly added state
        iterator_max_to_searched_state = (max_Q_and_action_map_pointer->
                insert( std::pair<std::string, max_Q_and_action*>
                    (state_name, new max_Q_and_action) )).first;

        // update max Q value and max Q action for this new added state
        iterator_max_to_searched_state->second->max_Q_value = given_Q_value;
        iterator_max_to_searched_state->second->max_Q_action = action_as_float_value;
    }
    // if given Q value is greater than previous max Q value
    else if(iterator_max_to_searched_state->second->max_Q_value <= given_Q_value)
    {
        // update max Q value to given Q value
        iterator_max_to_searched_state->second->max_Q_value = given_Q_value;
        // update max Q value action to given action
        iterator_max_to_searched_state->second->max_Q_action = action_as_float_value;
    }
    // else check if this is the Q value of max Q action
    else if(iterator_max_to_searched_state->second->max_Q_action ==
            action_as_float_value)
    {
        // initialize max Q value to given Q value
        iterator_max_to_searched_state->second->max_Q_value = given_Q_value;
        // initialize max Q action to given action
        iterator_max_to_searched_state->second->max_Q_action = action_as_float_value;

        // check matching states if they have Q value greater than set max Q value
        for(std::map<std::string, float>::const_iterator
                state_Q_value_map_iterator = state_Q_value_map.begin();
                state_Q_value_map_iterator != state_Q_value_map.end();
                state_Q_value_map_iterator++)
        {
            // if state name matches and the Q value is greater than set max Q value
            // then update the max Q value and max Q action
            if( (state_Q_value_map_iterator->
                        first.compare(0, STATE_NAME_LENGTH, state_name) == 0) &&
                    (state_Q_value_map_iterator->second >
                     iterator_max_to_searched_state->second->max_Q_value) )
            {
                // for setting max Q action
                // scan action from the state-action key as float value
                sscanf(state_Q_value_map_iterator->first.c_str(),
                        STATE_MASK ACTION_READ_FORMAT,
                        &(iterator_max_to_searched_state->second->max_Q_action));

                // set Q value for this state-action as the max Q value
                iterator_max_to_searched_state->second->max_Q_value =
                    state_Q_value_map_iterator->second;
            }
        }
    }
}


void controller_storage::Q_maps::update_Q_value_for(
        const std::string & state_name,
        const std::string & action_name,
        const float t_Q_value)
{
    // first character and second characters are the keys
    // 0 is a sign character
    std::map<std::string, float> & state_Q_value_map =
        get_map_for(state_name.at(1), state_name.at(2));
    const std::string map_key = state_name + '|' + action_name;

    // update Q value
    // adds this key if not there in the map
    state_Q_value_map[map_key] = t_Q_value;

    // check and update max Q value and action for this state
    check_and_update_max_Q(state_name, action_name,
            t_Q_value, state_Q_value_map);
}


void controller_storage::Q_maps::get_all_Q_value_maps(
        std::vector<state_action_Q_map * > & map_list) const
{
    map_list.clear();

    // push each velocity map pointer to the vector
    for(int i = 0; i < Q_MAP_ARRAY_ROWS; i++)
    {
        for(int j = 0; j < Q_MAP_ARRAY_COLUMNS; j++)
        {
            map_list.push_back(velocity_Q_map_pointers[i][j]);
        }
    }

    // push default map pointer to the vector
    map_list.push_back(default_velocity_Q_map_pointer);
}


long long int controller_storage::Q_maps::get_total_size(
        const int print_info) const
{
    long long int total_size = 0;
    for(int i = 0; i < Q_MAP_ARRAY_ROWS; i++)
    {
        for(int j = 0; j < Q_MAP_ARRAY_COLUMNS; j++)
        {
            total_size += (velocity_Q_map_pointers[i][j])->size();
        }
    }

    total_size += default_velocity_Q_map_pointer->size();

    // for displaying sizes of different maps
    if(print_info)
    {
        for(int i = 0; i < Q_MAP_ARRAY_ROWS; i++)
        {
            for(int j = 0; j < Q_MAP_ARRAY_COLUMNS; j++)
            {
                printf("state_Q_value_map %d%d size - %lu\n", i, j,
                        (velocity_Q_map_pointers[i][j])->size());
            }
        }

        printf("default state_Q_value_map size - %lu\n",
                default_velocity_Q_map_pointer->size());

        printf("max Q action map size - %lu\n",
                max_Q_and_action_map_pointer->size());
    }

    return total_size;
}


long long int controller_storage::Q_maps::load_maps_from_file(
        const std::string & t_Q_value_file_name)
{
    printf("loading Q_maps-%s from file\n", Q_MAPS_VERSION);
    // check if file name is empty
    if(t_Q_value_file_name.empty())
    {
        puts("empty file name, not loading Q values");
        return 0;
    }

    // open output file
    FILE * t_Q_value_file = fopen(t_Q_value_file_name.c_str(), "r");
    if(t_Q_value_file == NULL)
    {
        printf("error reading file \"%s\"\n", t_Q_value_file_name.c_str());
        return 0;
    }
    else
    {
        printf("reading map from file \"%s\"\n", t_Q_value_file_name.c_str());
        char * line = NULL;

        int line_count = 0;
        int line_character_count = 0;
        size_t len = 0;
        while((line_character_count = getline(&line, &len, t_Q_value_file)) != -1
                && line[0] != 's')               // Q values ends with "stat" line
        {
            // break line into state, action and Q value
            char state_name[STATE_NAME_LENGTH + 1];
            char action_name[ACTION_NAME_LENGTH + 1];
            float t_Q_value_float;

            // scan line as fixed length map key (state and action) and Q value
            sscanf(line, MAP_READ_FORMAT, state_name, action_name, &t_Q_value_float);

            // append null character to char arrays
            state_name[STATE_NAME_LENGTH] = '\0';
            action_name[ACTION_NAME_LENGTH] = '\0';

            update_Q_value_for(state_name, action_name, t_Q_value_float);

            line_count++;
        }

        // check for a line containing "stats" followed by
        // a line with training counter
        if((line[0] == 's') && (line[1] == 't') &&
                (line[2] == 'a') && (line[3] == 't') && (line[4] == 's'))
        {
            if( (line_character_count = getline(&line, &len, t_Q_value_file)) != -1 )
            {
                sscanf(line, "%lld", &m_training_counter);
            }
            else
            {
                puts("error reading stats training counter line");
            }
        }

        // set member Q value file_name to given file name
        m_Q_value_file_name = t_Q_value_file_name;

        fclose(t_Q_value_file);

        puts("Q value File loaded successfully");
        return m_training_counter;
    }
}


int controller_storage::Q_maps::write_maps_to_file(
        const std::string & t_Q_value_file_name,
        const long long int race_counter)
{
    int close_value = -1;
    if(t_Q_value_file_name.empty())
    {
        puts("empty file name, not writing Q values");
        return close_value;
    }

    FILE * t_Q_value_file = fopen(t_Q_value_file_name.c_str(), "w");
    if(t_Q_value_file == NULL)
    {
        printf("couldn't open file \'%s\' for writing the map.\n",
                t_Q_value_file_name.c_str());
    }
    else
    {
        printf("writing map to file \'%s\'\n", t_Q_value_file_name.c_str());

        // get list of all the maps available
        std::vector<controller_storage::state_action_Q_map *> map_pointer_list;
        get_all_Q_value_maps(map_pointer_list);

        // iterate over list of maps
        for(std::vector<controller_storage::state_action_Q_map *>::iterator
                map_list_iterator = map_pointer_list.begin();
                map_list_iterator != map_pointer_list.end();
                ++map_list_iterator)
        {
            // iterate over each map
            for(controller_storage::state_action_Q_map::iterator
                    map_iterator = (*(map_list_iterator))->begin();
                    map_iterator != (*(map_list_iterator))->end();
                    ++map_iterator)
            {
                fprintf(t_Q_value_file, MAP_WRITE_FORMAT,
                        map_iterator->first.c_str(), map_iterator->second);
            }
        }

        if(race_counter > 0)
        {
            // resetting training counter
            m_training_counter = race_counter;
        }

        // save this counter in the file
        fprintf(t_Q_value_file, "stats\n%lld\n", race_counter);

        // closing the file
        close_value = fclose(t_Q_value_file);

        if(close_value == EOF)
        {
            printf("error closing file \'%s\'\n", t_Q_value_file_name.c_str());
        }
        else
        {
            t_Q_value_file = NULL;
            printf("file closed \'%s\'\n", t_Q_value_file_name.c_str());
        }
    }

    return close_value;
}


