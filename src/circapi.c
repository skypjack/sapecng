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
 * \file circapi.c
 *
 * \brief Circuit creation functions
 *
 * This file contains several functions that can be used to create
 * circuit. Every parser should use this functions including
 * circuit.h file (prototypes file).
 */

#include "circapi.h"

/**
 * \brief Component handler: R
 *
 * Handler for R component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na related node
 * \param nb related node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_R (circ_t* crep, char* id, int na, int nb, const double val, const int sym)
{
  return addsimple(crep, na, nb, na, nb, id, Z, 0, val, sym);
}

/**
 * \brief Component handler: L
 *
 * Handler for L component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na related node
 * \param nb related node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_L (circ_t* crep, char* id, int na, int nb, const double val, const int sym)
{
  return addsimple(crep, na, nb, na, nb, id, Z, 1, val, sym);
}

/**
 * \brief Component handler: G
 *
 * Handler for G component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na related node
 * \param nb related node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_G (circ_t* crep, char* id, int na, int nb, const double val, const int sym)
{
  return addsimple(crep, na, nb, na, nb, id, Y, 0, val, sym);
}

/**
 * \brief Component handler: C
 *
 * Handler for C component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na related node
 * \param nb related node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_C (circ_t* crep, char* id, int na, int nb, const double val, const int sym)
{
  return addsimple(crep, na, nb, na, nb, id, Y, 1, val, sym);
}

/**
 * \brief Component handler: V
 *
 * Handler for V component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related node
 * \param nb negative related node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_V (circ_t* crep, char* id, int na, int nb, const double val, const int sym)
{
  int fn, noerr;
  if(!crep->reference)
    crep->reference = (na ? na : nb);
  fn = circ_getfree(crep);
  noerr = addsimple(crep, fn, crep->reserved, crep->reserved, crep->reference, id, Y, 0, val, sym);
  // reverse-sign mode injection
  // noerr &= addsimple(crep, fn, crep->reserved, nb, na, NULL, Y, 0, -1, 0);
  noerr &= addsimple(crep, fn, crep->reserved, nb, na, NULL, Y, 0, 1, 0);
  noerr &= addnullor(crep, nb, na, crep->reserved, fn, NULL, 1, 1);
  return noerr;
}

/**
 * \brief Component handler: I
 *
 * Handler for I component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related node
 * \param nb negative related node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_I (circ_t* crep, char* id, int na, int nb, const double val, const int sym)
{
  if(!crep->reference)
    crep->reference = (na ? na : nb);
  return addsimple(crep, na, nb, crep->reserved, crep->reference, id, Y, 0, val, sym);
}

/**
 * \brief Component handler: VCCS
 *
 * Handler for VCCS component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related output node
 * \param nb negative related output node
 * \param nac positive related input node
 * \param nbc negative related input node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_VCCS (circ_t* crep, char* id, int na, int nb, int nac, int nbc, const double val, const int sym)
{
  return addsimple(crep, na, nb, nac, nbc, id, Y, 0, val, sym);
}

/**
 * \brief Component handler: VCVS
 *
 * Handler for VCVS component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related output node
 * \param nb negative related output node
 * \param nac positive related input node
 * \param nbc negative related input node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_VCVS (circ_t* crep, char* id, int na, int nb, int nac, int nbc, const double val, const int sym)
{
  int fn, noerr;
  fn = circ_getfree(crep);
  noerr = addsimple(crep, fn, nbc, nac, nbc, id, Y, 0, val, sym);
  // reverse-sign mode injection
  // noerr &= addsimple(crep, fn, nbc, nb, na, NULL, Y, 0, -1, 0);
  noerr &= addsimple(crep, fn, nbc, nb, na, NULL, Y, 0, 1, 0);
  noerr &= addnullor(crep, nb, na, nbc, fn, NULL, 1, 1);
  return noerr;
}

/**
 * \brief Component handler: CCCS
 *
 * Handler for CCCS component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related output node
 * \param nb negative related output node
 * \param nac positive related input node
 * \param nbc negative related input node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_CCCS (circ_t* crep, char* id, int na, int nb, int nac, int nbc, const double val, const int sym)
{
  int fn, noerr;
  fn = circ_getfree(crep);
  noerr = addsimple(crep, nac, nbc, nbc, fn, id, Y, 0, val, sym);
  // reverse-sign mode injection
  // noerr &= addsimple(crep, na, nb, nbc, fn, NULL, Y, 0, -1, 0);
  noerr &= addsimple(crep, na, nb, nbc, fn, NULL, Y, 0, 1, 0);
  noerr &= addnullor(crep, nbc, fn, nbc, nac, NULL, 1, 1);
  return noerr;
}

/**
 * \brief Component handler: CCVS
 *
 * Handler for CCVS component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related output node
 * \param nb negative related output node
 * \param nac positive related input node
 * \param nbc negative related input node
 * \param val component value
 * \param sym component type (symbolic or not)
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_CCVS (circ_t* crep, char* id, int na, int nb, int nac, int nbc, const double val, const int sym)
{
  int noerr;
  noerr = addsimple(crep, nac, nbc, na, nb, id, Z, 0, val, sym);
  noerr &= addnullor(crep, nb, na, nbc, nac, NULL, 1, 1);
  return noerr;
}

/**
 * \brief Component handler: OP. AMPL.
 *
 * Handler for OP. AMPL. component.
 *
 * \param crep circuit reference
 * \param id component name
 * \param na positive related output node
 * \param nb negative related output node
 * \param nac positive related input node
 * \param nbc negative related input node
 * \result zero if some error occurs, a positive value otherwise
 */
int
add_OP_AMPL (circ_t* crep, char* id, int na, int nb, int nac, int nbc)
{
  return addnullor(crep, nb, na, nbc, nac, NULL, 1, 1);
}
