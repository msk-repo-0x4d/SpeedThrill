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
 * car_utils.h
 *
 * general utility file for car222
 *
 *  Created on: Jul 23, 2018
 *      Author: M.S.Khan
 */

#ifndef  CAR_UTILS_H
#define  CAR_UTILS_H


/**
 * clips given value between MIN VALUE and MAX VALUE
 **/
extern float clip_min_max(const float & given_value,
        const float & MIN_VALUE,
        const float & MAX_VALUE);


#endif


