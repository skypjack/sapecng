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
 * \file xmemrep.c
 *
 * \brief Critical functions supplied with memory management
 *
 * This file contains some critical functions with an integrated memory
 * management, so if there are some problems using these functions the program
 * will be terminated immediately.
 */

#include "common.h"

/**
 * \brief malloc function with memory management
 *
 * xmalloc is like malloc but with integrated memory management, so if there are
 * some problems allocating memory the program will be terminated immediately.
 *
 * \param num xmalloc allocates (if possible) \a num bytes of memory
 * \return a pointer to the allocated memory
 */
void*
xmalloc (const size_t num)
{
  void* new = malloc(num);
  if (!new) fatal("Memory exhausted");
  return new;
}

/**
 * \brief realloc function with memory management
 *
 * xrealloc is like realloc but with integrated memory management, so if there
 * are some problems reallocating memory the program will be terminated
 * immediately.
 *
 * \param ref a pointer to the memory block; if \a ref is equal to zero, the
 *   call is equivalent to \e malloc(num))
 * \param num xrealloc changes (if possible) the size of the block pointed to by
 *   \a ref to \a num bytes; if \a num is equal to zero, the call is
 *   equivalent to \e free(ref) and will brings to the termination of the
 *   program
 * \return a pointer to the newly allocated memory
 */
void*
xrealloc (void *ref, const size_t num)
{
  void* new;
  if (!ref) return xmalloc(num);
  new = realloc (ref, num);
  if (!new) fatal("Memory exhausted");
  return new;
}

/**
 * \brief calloc function with memory management
 *
 * xcalloc is like calloc but with integrated memory management, so if there are
 * some problems allocating memory the program will be terminated
 * immediately.
 *
 * \param num xcalloc allocates (if possible) memory for an array of \a num
 *   elements of \a size bytes each
 * \param size each element of the array has a size of \a size bytes
 * \return a pointer to the allocated memory
 */
void*
xcalloc (const size_t num, const size_t size)
{
  void* new = xmalloc(num * size);
  bzero(new, num * size);
  return new;
}

/**
 * \brief strdup function with memory management
 *
 * xstrdup is like strdup but with integrated memory management, so if there are
 * some problems allocating memory for the new string the program will be
 * terminated immediately.
 *
 * \param str pointer to the original string to be duplicated
 * \return a pointer to a new string which is a duplicate of the original string
 *   pointed to by \a str
 */
char*
xstrdup (const char* str)
{
  char* new = NULL;
  if(str) {
    new = (char*) xmalloc(strlen(str) + 1);
    strcpy(new, str);
  }
  return new;
}
