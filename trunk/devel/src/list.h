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
 * \file list.h
 *
 * \brief List type and related functions
 *
 * This file contains a definition for the %list structure.
 */

/**
 * \brief Useful to manage multiple inclusions
 */
#ifndef LIST_H
#define LIST_H 1

#include "common.h"

/**
 * \brief list_next with integrated cast
 *
 * This macro gets the %next element casting it before return.
 */
#define list_next_entry(type, head) \
  (type*) list_next ((list_t*) head)

/**
 * \brief Gets %data function with integrated cast
 *
 * This macro gets %data element casting them before return.
 */
#define list_data(type, head) \
  (type*) head->data

/**
 * \brief List type
 *
 * This structure is a general-purpose %list structure that every kind of listed
 * types can use (capitalizing also on the associated functions), simply putting
 * \e next pointer as the first field and exploiting cast operation.
 */
struct list
{
  struct list* next;  /**< \e Next element of the %list */
  void* data;  /**< Potential %data (without type) */
};

/**
 * \brief Simpler %struct %list definition.
 */
typedef
struct list
list_t;

extern list_t*
list_new (void*);

extern list_t*
list_add (list_t*, list_t*);

extern list_t*
list_next (const list_t*);

extern size_t
list_length (list_t*);

#endif /* LIST_H */
