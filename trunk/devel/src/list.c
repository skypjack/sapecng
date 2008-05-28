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
 * \file list.c
 *
 * \brief List related functions implementation
 *
 * This file contains implementation for %list related functions, so that
 * everyone can manages %list and also every kind of listed types simply using
 * cast operator.
 */

#include "common.h"
#include "list.h"

/**
 * \brief New %list token constructor
 *
 * It makes a new %list element (a ring of the %list) using \a %udata as payload
 * of that element (\a %udata can be null).
 *
 * \param udata new %list element's payload
 * \return newly allocated %list element
 */
list_t*
list_new (void* udata)
{
  list_t* new = XMALLOC(list_t, 1);
  new->next = NULL;
  new->data = udata;
  return new;
}

/**
 * \brief Useful to extend a %list adding a new element
 *
 * This function adds a new element in front of the %list and returns the new
 * obtained %list; if \a %head is a non-valid pointer (like a null pointer) no
 * operation is performed.
 *
 * \param head element to be added
 * \param tail %list reference
 * \return extended %list
 */
list_t*
list_add (list_t* head, list_t* tail)
{
  if(head != NULL)
    head->next = tail;
  return (head != NULL) ? head : tail;
}

/**
 * \brief It retrieves the next element of the %list
 *
 * Simply, it returns the next element of the %list, if there is one.
 *
 * \param head %list reference
 * \return the next element of the %list if there is one, zero otherwise
 */
list_t*
list_next (const list_t* head)
{
  if(head != NULL)
    return head->next;
  else return NULL;
}

/**
 * \brief Length of %list calculator
 *
 * It returns the length of the %list, nothing more.
 *
 * \param head %list reference
 * \return the length of the %list
 */
size_t
list_length (list_t* head)
{
  int cnt;
  for(cnt = 0; head; ++cnt)
    head = head->next;
  return cnt;
}
