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
 * car222_race_init.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: M.S.Khan
 */


#include "car222_Q_maps.h"


namespace controller
{
    // Q maps storage that has the Q maps
    controller_storage::Q_maps_storage  _Q_maps_storage;


#ifdef TRAINING_MODE  /* only available in training mode */

    // race counter for training
    int training_race_counter = 0;

#endif

}


