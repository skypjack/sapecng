/****************************************************************************************
 *
 *  Sapec-NG, Next Generation Symbolic Analysis Program for Electric Circuit
 *  Copyright (C)  2007  Michele Caini
 *
 *
 *  This file is part of Sapec-NG.
 *
 *  Sapec-NG is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 *  To contact me:   skypjack@gmail.com
 *
 ***************************************************************************************/

/**
 * \file circapi.h
 *
 * \brief Circuit creation functions prototypes
 *
 * This file contains prototypes for several functions that can be used to
 * create circuit. Every parser should include this file and use those
 * functions.
 */

/**
 * \brief Useful to manage multiple inclusions
 */
#ifndef CIRCAPI_H
#define CIRCAPI_H 1

#include "circuit.h"

int
add_R (circ_t*, char*, int, int, const double, const int);

int
add_L (circ_t*, char*, int, int, const double, const int);

int
add_G (circ_t*, char*, int, int, const double, const int);

int
add_C (circ_t*, char*, int, int, const double, const int);

int
add_V (circ_t*, char*, int, int, const double, const int);

int
add_I (circ_t*, char*, int, int, const double, const int);

int
add_VCCS (circ_t*, char*, int, int, int, int, const double, const int);

int
add_VCVS (circ_t*, char*, int, int, int, int, const double, const int);

int
add_CCCS (circ_t*, char*, int, int, int, int, const double, const int);

int
add_CCVS (circ_t*, char*, int, int, int, int, const double, const int);

int
add_OP_AMPL (circ_t*, char*, int, int, int, int);

#endif /* CIRCAPI_H */
