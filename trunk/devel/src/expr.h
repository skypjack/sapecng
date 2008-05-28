/****************************************************************************************

   Sapec-NG, Next Generation Symbolic Analysis Program for Electric Circuit
   Copyright (C)  2007  Michele Caini


   This file is part of Sapec-NG.

   Sapec-NG is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA


   To contact me:   skypjack@gmail.com

****************************************************************************************/

/**
 * \file expr.h
 *
 * \brief Expression type and related functions
 *
 * This file contains a definition for the expression token type, useful at the
 * end of the resolution process to handle all the informations in a most
 * excellent manner.
 */

/**
 * \brief Useful to manage multiple inclusions
 */
#ifndef EXPR_H
#define EXPR_H 1

#include "common.h"
#include "circuit.h"
#include "list.h"

/**
 * \brief Internal buffer size
 *
 * This is the size of the internal buffer used loading expression from file.
 */
#define BUF_SIZE 32

/**
 * \brief Expression token type
 *
 * This structure represents a token type for an expression, with a numeric part
 * (in form of double value), a %list of symbolic parts, and a %degree which
 * determines the exponent of the s part; several tokens are linked together
 * like "expr1 + ... + exprN" while the single token represents a component of
 * an expression like the following: vpart * epart1 * ... * epartN * s^degree .
 */
struct expr
{
  struct expr* next;  /**< \e Next element of the %list */
  double vpart;  /**< Numeric part of the expression */
  int etoken;  /**< Number of symbolic elements */
  short int degree;  /**< \e Degree of the specific token */
  list_t* epart;  /**< List of symbolic elements */
};

/**
 * \brief Simpler %struct %expr definition.
 */
typedef
struct expr
expr_t;

extern void
sep (const int, FILE*);

extern int
splash (expr_t*, FILE*, const int);

extern int
expr_to_file (expr_t*, FILE*);

extern expr_t*
expr_from_file (FILE*);

extern expr_t*
expr_new ();

void
free_expr (expr_t*);

expr_t*
expr_sort (expr_t*);

int
circ_to_expr (circ_t*, list_t**, list_t**);

#endif /* EXPR_H */
