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
 * fuzzy_rules.cpp
 *
 *     version  : 1.0.0
 *  created on  : 18 Feb 2018
 *      author  : M.S.Khan
 */

#include "fuzzy_controller.h"
#include "fuzzy_values.h"

#include <fl/activation/First.h>
#include <fl/activation/Proportional.h>
#include <fl/norm/s/AlgebraicSum.h>
#include <fl/norm/s/Maximum.h>
#include <fl/norm/t/AlgebraicProduct.h>
#include <fl/norm/t/Minimum.h>
#include <fl/rule/Rule.h>
#include <fl/rule/RuleBlock.h>


void controller::FuzzyController::add_rules()
{
    // add steering rules to the engine
    add_steer_rules();

    // add gear rules to the engine
    add_gear_rules();

    // add acceleration rules to the engine
    add_accel_rules();

    // add acceleration rules to the engine
    add_brake_rules();
}


void controller::FuzzyController::add_steer_rules()
{
    // deleted in class fl::Engine
    fl::RuleBlock * steer_rule_block = new fl::RuleBlock("steer_rule_block");
    m_fuzzy_engine->addRuleBlock(steer_rule_block);

    steer_rule_block->setName("steer_rule_block");
    steer_rule_block->setDescription("rule for steering");
    steer_rule_block->setEnabled(true);

    // Conjunction, Disjunction, Implication and Activation
    // are stored in smart pointer
    steer_rule_block->setConjunction(new fl::Minimum);
    steer_rule_block->setDisjunction(new fl::Maximum);
    steer_rule_block->setImplication(new fl::AlgebraicProduct);
    steer_rule_block->setActivation(new fl::Proportional);

    steer_rule_block->addRule(fl::Rule::parse("if path is " STRAIGHT
                " then steer is " STRAIGHT, m_fuzzy_engine));

    steer_rule_block->addRule(fl::Rule::parse("if path is " RIGHT
                " then steer is " RIGHT, m_fuzzy_engine));

    steer_rule_block->addRule(fl::Rule::parse("if path is " LEFT
                " then steer is " LEFT, m_fuzzy_engine));

    steer_rule_block->addRule(fl::Rule::parse("if path is " TOO_RIGHT
                " then steer is " TOO_RIGHT, m_fuzzy_engine));

    steer_rule_block->addRule(fl::Rule::parse("if path is " TOO_LEFT
                " then steer is " TOO_LEFT, m_fuzzy_engine));
}


void controller::FuzzyController::add_gear_rules()
{
    // deleted in class fl::Engine
    fl::RuleBlock * gear_rule_block = new fl::RuleBlock("gear_rule_block");
    m_fuzzy_engine->addRuleBlock(gear_rule_block);

    gear_rule_block->setName("gear_rule_block");
    gear_rule_block->setDescription("rule block for gear change");
    gear_rule_block->setEnabled(true);

    // Conjunction, Disjunction, Implication and Activation
    // are stored in smart pointer
    gear_rule_block->setConjunction(new fl::Minimum);
    gear_rule_block->setDisjunction(new fl::Maximum);
    gear_rule_block->setImplication(new fl::AlgebraicProduct);
    gear_rule_block->setActivation(new fl::First);

    gear_rule_block->addRule(fl::Rule::parse("if speed is " VERY_FAST
                " then gear is " VERY_HIGH_GEAR, m_fuzzy_engine));

    gear_rule_block->addRule(fl::Rule::parse("if speed is " FAST
                " then gear is " HIGH_GEAR, m_fuzzy_engine));

    gear_rule_block->addRule(fl::Rule::parse("if speed is " MEDIUM
                " then gear is " MEDIUM_GEAR, m_fuzzy_engine));

    gear_rule_block->addRule(fl::Rule::parse("if speed is " SLOW
                " then gear is " LOW_GEAR, m_fuzzy_engine));

    gear_rule_block->addRule(fl::Rule::parse("if speed is " VERY_SLOW
                " then gear is " VERY_LOW_GEAR, m_fuzzy_engine));
}


void controller::FuzzyController::add_accel_rules()
{
    // deleted in class fl::Engine
    fl::RuleBlock * accel_rule_block = new fl::RuleBlock("accel_rule_block");
    m_fuzzy_engine->addRuleBlock(accel_rule_block);

    accel_rule_block->setName("accel_rule_block");
    accel_rule_block->setDescription("rule for acceleration");
    accel_rule_block->setEnabled(true);

    // Conjunction, Disjunction, Implication and Activation
    // are stored in smart pointer
    accel_rule_block->setConjunction(new fl::Minimum);
    accel_rule_block->setDisjunction(new fl::Maximum);
    accel_rule_block->setImplication(new fl::AlgebraicProduct);
    accel_rule_block->setActivation(new fl::First);

    accel_rule_block->addRule(fl::Rule::parse("if (path is " TOO_LEFT
                " or path is " TOO_RIGHT
                ") and speed is " VERY_FAST
                " then accel is " VERY_SLOW, m_fuzzy_engine));

    accel_rule_block->addRule(fl::Rule::parse("if (path is " TOO_LEFT
                " or path is " TOO_RIGHT
                ") and speed is " FAST
                " then accel is " SLOW, m_fuzzy_engine));

    accel_rule_block->addRule(fl::Rule::parse("if (path is " TOO_LEFT
                " or path is " TOO_RIGHT
                ") and speed is " MEDIUM
                " then accel is " SLOW, m_fuzzy_engine));
}


void controller::FuzzyController::add_brake_rules()
{
    // deleted in class fl::Engine
    fl::RuleBlock * brake_rule_block = new fl::RuleBlock("brake_rule_block");
    m_fuzzy_engine->addRuleBlock(brake_rule_block);

    brake_rule_block->setName("brake_rule_block");
    brake_rule_block->setDescription("rule for brakeeration");
    brake_rule_block->setEnabled(true);

    // Conjunction, Disjunction, Implication and Activation
    // are stored in smart pointer
    brake_rule_block->setConjunction(new fl::Minimum);
    brake_rule_block->setDisjunction(new fl::Maximum);
    brake_rule_block->setImplication(new fl::AlgebraicProduct);
    brake_rule_block->setActivation(new fl::First);

    brake_rule_block->addRule(fl::Rule::parse("if (path is " TOO_LEFT
                " or path is " TOO_RIGHT
                ") and speed is " VERY_FAST
                " then brake is " VERY_FAST, m_fuzzy_engine));

    brake_rule_block->addRule(fl::Rule::parse("if (path is " TOO_LEFT
                " or path is " TOO_RIGHT
                ") and speed is " FAST
                " then brake is " FAST, m_fuzzy_engine));

    brake_rule_block->addRule(fl::Rule::parse("if (path is " TOO_LEFT
                " or path is " TOO_RIGHT
                ") and speed is " MEDIUM
                " then brake is " MEDIUM, m_fuzzy_engine));
}


