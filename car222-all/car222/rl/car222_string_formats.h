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
 * car222_string_formats.h
 *
 *  Created on: Jul 27, 2018
 *      Author: M.S.Khan
 */


#ifndef  CAR222_STRING_FORMATS_H_
#define  CAR222_STRING_FORMATS_H_


// format for state string
#define STATE_PRINT_FORMAT         "%+03d|%+04.1f|%+03d|%+03d|%+06.1f|%+06.1f"
// format for action string
#define ACTION_PRINT_FORMAT        "%4.3f"

// format for masking state string
#define STATE_MASK                 "%*30c|"
// format for scanning action
#define ACTION_READ_FORMAT         "%f"

// format for scanning a line
#define MAP_READ_FORMAT            "%30c|%5c=%f\n"
// format for writing a line
#define MAP_WRITE_FORMAT           "%s=%+012f\n"


// length excludes the trailing null character
// length of map key
#define INPUT_LENGTH               36
// length of state string
#define STATE_NAME_LENGTH          30
// length of action string
#define ACTION_NAME_LENGTH         5


#endif      /* ifndef CAR222_STRING_FORMATS_H_ */

