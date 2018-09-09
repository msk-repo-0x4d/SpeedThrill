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
 * fuzzy_controller.h
 *
 *     version  : 1.0.0
 *  created on  : 18 Feb 2018
 *      author  : M.S.Khan
 */


#ifndef FUZZY_CONTROLLER_H_
#define FUZZY_CONTROLLER_H_

#include <fl/Engine.h>


// Fuzzy controller version - 1.0.0
#define FUZZY_CONTROLLER_VERSION "1.0.0"


// Threshold for discouraging frequent gear changes
#define MIN_ABS_SPEED_DIFF_FOR_GEAR_CHANGE 5
// Rule does not apply for lower gears
#define LOW_GEAR_FOR_FREE_GEAR_CHANGES 2

// Input and Output names used in fuzzy engine
#define INPUT_SPEED "speed"
#define INPUT_ACCELERATION "acceleration"
#define INPUT_PATH "path"
#define INPUT_NEXT_PATH "next_path"
#define INPUT_STABILITY "stability"

#define OUTPUT_STEER "steer"
#define OUTPUT_ACCEL "accel"
#define OUTPUT_GEAR "gear"
#define OUTPUT_BRAKE "brake"


namespace controller
{

    /** fuzzy controller input variables **/
    typedef struct fuzzy_input_struct
    {

        float speed;
        float acceleration;
        float path;
        float next_path;
        float stability;

    } fuzzy_inputs;


    /** fuzzy controller output variables **/
    typedef struct fuzzy_output_struct
    {

        float steer;
        float accel;
        int gear;
        float brake;

    } fuzzy_outputs;


    /*
     * =====================================================================================
     *        Class:  FuzzyController
     *  Description:  This class has a fuzzy engine which accepts fuzzy_input_struct,
     *                fuzzifies the input values, applies rules to them,
     *                gets fuzzy outputs, defuzzifies them and then returns
     *                the defuzzified outputs.
     * =====================================================================================
     */
    class FuzzyController
    {
        public:

            FuzzyController();
            ~FuzzyController();

            // get fuzzy outputs for the given set of fuzzy inputs
            const fuzzy_outputs & get_output(const fuzzy_inputs * m_fuzzy_inputs);


        private:

            /** MEMBER VARIABLES **/

            // fuzzy engine
            fl::Engine * m_fuzzy_engine;
            // fuzzy output values
            fuzzy_outputs m_fuzzy_outputs; 

            // recorded speed at last gear change
            float m_speed_at_gear_change;


            /** MEMBER FUNCTIONS **/

            // add input variables to the fuzzy engine
            void add_input_variables();
            // add output variables to the fuzzy engine
            void add_output_variables();
            // add rules to the fuzzy engine
            void add_rules();

            // add rules for various outputs
            void add_gear_rules();
            void add_steer_rules();
            void add_accel_rules();
            void add_brake_rules();

            // copy constructor
            FuzzyController(const FuzzyController &other);

            // assignment operator
            FuzzyController& operator=(const FuzzyController &other);

    };       /** class FuzzyController **/

}

#endif      /** ifndef FUZZY_CONTROLLER_H_ **/


