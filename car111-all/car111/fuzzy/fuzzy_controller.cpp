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
 * fuzzy_controller.cpp
 *
 *     version  : 1.0.0
 *  created on  : 18 Feb 2018
 *      author  : M.S.Khan
 */


#include<cmath>
#include<string>

#include "fuzzy_controller.h"
#include "fuzzy_values.h"

#include <fl/Engine.h>
#include <fl/norm/s/AlgebraicSum.h>
#include <fl/norm/s/Maximum.h>
#include <fl/term/Ramp.h>
#include <fl/term/Rectangle.h>
#include <fl/term/Trapezoid.h>
#include <fl/variable/InputVariable.h>
#include <fl/variable/OutputVariable.h>


controller::FuzzyController::FuzzyController()
{
    m_fuzzy_engine = new fl::Engine;
    m_fuzzy_engine->setName("Fuzzy Controller Engine");
    m_fuzzy_engine->setDescription("fuzzy controller for deciding control values");

    m_fuzzy_outputs = {0, 0, 0, 1};    // initialize steer, accel, gear and brake values
    m_speed_at_gear_change = 0;

    // add input variables to the engine
    add_input_variables();

    // add output variables to the engine
    add_output_variables();

    // add rules to the engine
    add_rules();

    // display version information with status
    std::cout<<"Fuzzy Controller v"<<FUZZY_CONTROLLER_VERSION<<" - ";

    // make sure engine is ready
    std::string message;
    if(!m_fuzzy_engine->isReady(&message))
    {
        std::cout<<"Error loading Fuzzy engine : "
            <<std::endl<<message<<std::endl;
    }
    else
    {
        std::cout<<"Loaded successfully."<<std::endl;
    }
}


void controller::FuzzyController::add_input_variables()
{
    // deleted in class fl::Engine
    fl::InputVariable *speed = new fl::InputVariable(INPUT_SPEED);
    m_fuzzy_engine->addInputVariable(speed);
    // all terms deleted in class fl::Variable
    speed->addTerm(new fl::Trapezoid(VERY_VERY_SLOW, -0.5, -0.1, 0.1, 0.5));
    speed->addTerm(new fl::Trapezoid(VERY_SLOW, 0.4999, 2, 10, 15));
    speed->addTerm(new fl::Trapezoid(SLOW, 15, 25, 40, 45));
    speed->addTerm(new fl::Trapezoid(MEDIUM, 40, 45, 60, 65));
    speed->addTerm(new fl::Trapezoid(FAST, 60, 65, 80, 85));
    speed->addTerm(new fl::Ramp(VERY_FAST, 80, 85));


    // deleted in class fl::Engine
    fl::InputVariable *acceleration = new fl::InputVariable(INPUT_ACCELERATION);
    m_fuzzy_engine->addInputVariable(acceleration);
    // all terms deleted in class fl::Variable
    acceleration->addTerm(new fl::Ramp(NEGATIVE, 0, -1));
    acceleration->addTerm(new fl::Trapezoid(VERY_SLOW, 0, 0.5, 1, 1.5));
    acceleration->addTerm(new fl::Trapezoid(SLOW, 1, 1.5, 4, 6));
    acceleration->addTerm(new fl::Trapezoid(MEDIUM, 4, 6, 15, 20));
    acceleration->addTerm(new fl::Trapezoid(FAST, 12, 15, 20, 25));
    acceleration->addTerm(new fl::Ramp(VERY_FAST, 20, 30));


    // deleted in class fl::Engine
    fl::InputVariable *path = new fl::InputVariable(INPUT_PATH);
    m_fuzzy_engine->addInputVariable(path);
    // all terms deleted in class fl::Variable
    path->addTerm(new fl::Ramp(TOO_LEFT, -0.4, -0.5));
    path->addTerm(new fl::Trapezoid(LEFT, -0.5, -0.35, -0.2, -0.1));
    path->addTerm(new fl::Trapezoid(STRAIGHT, -0.15, -0.07, 0.07, 0.15));
    path->addTerm(new fl::Trapezoid(RIGHT, 0.1, 0.2, 0.35, 0.5));
    path->addTerm(new fl::Ramp(TOO_RIGHT, 0.4, 0.5));


    // deleted in class fl::Engine
    fl::InputVariable *next_path = new fl::InputVariable(INPUT_NEXT_PATH);
    m_fuzzy_engine->addInputVariable(next_path);
    // all terms deleted in class fl::Variable
    next_path->addTerm(new fl::Ramp(LEFT, -0.15, -0.4));
    next_path->addTerm(new fl::Trapezoid(STRAIGHT, -0.16, -0.1, 0.1, 0.16));
    next_path->addTerm(new fl::Ramp(RIGHT, 0.15, 0.4));


    // deleted in class fl::Engine
    fl::InputVariable *stability = new fl::InputVariable(INPUT_STABILITY, 0, 1);
    m_fuzzy_engine->addInputVariable(stability);
    // all terms deleted in class fl::Variable
    stability->addTerm(new fl::Ramp(STABLE, 0.2, 0.000));
    stability->addTerm(new fl::Ramp(UNSTABLE, 0.2, 0.4));
}


void controller::FuzzyController::add_output_variables()
{
    // deleted in class fl::Engine
    fl::OutputVariable * steer = new fl::OutputVariable(OUTPUT_STEER, -1, 1);
    m_fuzzy_engine->addOutputVariable(steer);
    steer->setDescription("angle of steer to be applied");
    steer->setEnabled(true);
    steer->setDefaultValue(0);            // default value
    steer->setLockPreviousValue(false);
    // stored in smart pointer
    steer->setAggregation(new fl::AlgebraicSum);
    // stored in smart pointer
    steer->setDefuzzifier(new fl::Centroid(100));
    // all terms deleted in class fl::Variable
    steer->addTerm(new fl::Ramp(TOO_LEFT, -0.3, -0.4));
    steer->addTerm(new fl::Trapezoid(LEFT, -0.4, -0.3, -0.15, -0.1));
    steer->addTerm(new fl::Trapezoid(STRAIGHT, -0.12, -0.05, 0.05, 0.12));
    steer->addTerm(new fl::Trapezoid(RIGHT, 0.1, 0.15, 0.3, 0.4));
    steer->addTerm(new fl::Ramp(TOO_RIGHT, 0.3, 0.4));


    // deleted in class fl::Engine
    fl::OutputVariable * accel = new fl::OutputVariable(OUTPUT_ACCEL, 0, 1);
    m_fuzzy_engine->addOutputVariable(accel);
    accel->setDescription("intensity of accelerator to be applied");
    accel->setEnabled(true);
    accel->setDefaultValue(1.0);            // default value
    accel->setLockPreviousValue(false);
    // stored in smart pointer
    accel->setAggregation(new fl::AlgebraicSum);
    // stored in smart pointer
    accel->setDefuzzifier(new fl::Centroid(100));
    // all terms deleted in class fl::Variable
    accel->addTerm(new fl::Ramp(VERY_SLOW, 0.2, 0.1));
    accel->addTerm(new fl::Trapezoid(SLOW, 0.15, 0.3, 0.5, 0.6));
    accel->addTerm(new fl::Trapezoid(MEDIUM, 0.4, 0.5, 0.6, 0.7));
    accel->addTerm(new fl::Trapezoid(FAST, 0.55, 0.7, 0.8, 0.95));
    accel->addTerm(new fl::Ramp(VERY_FAST, 0.9, 1.0));


    // deleted in class fl::Engine
    fl::OutputVariable * gear = new fl::OutputVariable(OUTPUT_GEAR, -1, 6);
    m_fuzzy_engine->addOutputVariable(gear);
    gear->setDescription("value of gear to be applied");
    gear->setEnabled(true);
    gear->setDefaultValue(1);            // default value
    gear->setLockPreviousValue(true);
    // stored in smart pointer
    gear->setAggregation(new fl::Maximum);
    // stored in smart pointer
    gear->setDefuzzifier(new fl::Centroid(100));
    // all terms deleted in class fl::Variable
    gear->addTerm(new fl::Ramp(REVERSE_GEAR, 0, -1));
    gear->addTerm(new fl::Rectangle(VERY_LOW_GEAR, 1, 2));
    gear->addTerm(new fl::Rectangle(LOW_GEAR, 2, 3));
    gear->addTerm(new fl::Rectangle(MEDIUM_GEAR, 3, 4));
    gear->addTerm(new fl::Rectangle(HIGH_GEAR, 4, 5));
    gear->addTerm(new fl::Ramp(VERY_HIGH_GEAR, 5, 6));


    // deleted in class fl::Engine
    fl::OutputVariable * brake = new fl::OutputVariable(OUTPUT_BRAKE, 0, 1);
    m_fuzzy_engine->addOutputVariable(brake);
    brake->setDescription("intensity of brake to be applied");
    brake->setEnabled(true);
    brake->setDefaultValue(0);            // default value
    brake->setLockPreviousValue(false);
    // stored in smart pointer
    brake->setAggregation(new fl::AlgebraicSum);
    // stored in smart pointer
    brake->setDefuzzifier(new fl::Centroid(100));
    // all terms deleted in class fl::Variable
    brake->addTerm(new fl::Ramp(VERY_SLOW, 0.05, 0.02));
    brake->addTerm(new fl::Trapezoid(SLOW, 0.02, 0.05, 0.08, 0.09));
    brake->addTerm(new fl::Trapezoid(MEDIUM, 0.08, 0.09, 0.1, 0.11));
    brake->addTerm(new fl::Trapezoid(FAST, 0.11, 0.115, 0.12, 0.125));
    brake->addTerm(new fl::Ramp(VERY_FAST, 0.12, 0.13));
}


const controller::fuzzy_outputs & controller::FuzzyController::get_output(
        const fuzzy_inputs * t_fuzzy_inputs)
{
    // apply fuzzy inputs
    m_fuzzy_engine->setInputValue(INPUT_SPEED, t_fuzzy_inputs->speed);
    m_fuzzy_engine->setInputValue(INPUT_ACCELERATION, t_fuzzy_inputs->acceleration);
    m_fuzzy_engine->setInputValue(INPUT_PATH, t_fuzzy_inputs->path);
    m_fuzzy_engine->setInputValue(INPUT_NEXT_PATH, t_fuzzy_inputs->next_path);
    m_fuzzy_engine->setInputValue(INPUT_STABILITY, t_fuzzy_inputs->stability);

    // process the input
    m_fuzzy_engine->process();

    // copy the calculated outputs
    m_fuzzy_outputs.steer = m_fuzzy_engine->getOutputVariable(OUTPUT_STEER)->getValue();
    m_fuzzy_outputs.accel = m_fuzzy_engine->getOutputVariable(OUTPUT_ACCEL)->getValue();
    m_fuzzy_outputs.brake = m_fuzzy_engine->getOutputVariable(OUTPUT_BRAKE)->getValue();

    /**
     * Modify gear value
     * -----------------
     * For discouraging frequent gear change only make suggested gear change when either :
     *
     * 1. The difference of current speed and speed at previous gear change
     *    is large enough (defined by MIN_ABS_SPEED_DIFF_FOR_GEAR_CHANGE).
     *
     * 2. The suggested gear is a small value (defined by LOW_GEAR_FOR_FREE_GEAR_CHANGES).
     *
     */
    if(std::fabs(t_fuzzy_inputs->speed - m_speed_at_gear_change)
            >= MIN_ABS_SPEED_DIFF_FOR_GEAR_CHANGE
            || m_fuzzy_outputs.gear <= LOW_GEAR_FOR_FREE_GEAR_CHANGES)
    {
        float fuzzy_gear = m_fuzzy_engine->getOutputVariable(OUTPUT_GEAR)->getValue();

        // use std::ceil for normal gears and std::floor for reverse gear
        m_fuzzy_outputs.gear = fuzzy_gear > 0 ?
            std::ceil(fuzzy_gear) : std::floor(fuzzy_gear);

        // record this speed for later comparison
        m_speed_at_gear_change = t_fuzzy_inputs->speed;
    }

    return m_fuzzy_outputs;
}


controller::FuzzyController::~FuzzyController()
{
    if(m_fuzzy_engine != NULL)
    {
        delete m_fuzzy_engine;
        m_fuzzy_engine = NULL;
    }
}


