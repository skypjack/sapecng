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
 * \file error.c
 *
 * \brief Error handling functions
 *
 * This file contains several functions that can be used to handle error
 * situation, showing a warning message, an error message and/or killing the
 * program (as consequence of fatal errors).
 */

#include "common.h"

/**
 * \brief Error handler function
 *
 * \internal
 * This function is called by all the other functions involved into the error
 * handling process; it shows a message that describes what's happened and
 * decides if the program needs to be terminated or not.
 *
 * \param exit_status value that determines if program will be aborted or not
 * \param mode call modality
 * \param message error message
 */
static void
error_handler (const int exit_status, const char *mode, const char *message)
{
  fprintf (stderr, "sapec-ng: %s: %s\n", mode, message);
  if (exit_status >= 0)
    exit(exit_status);
}

/**
 * \brief Warning function
 *
 * It shows a warning message, the programm will not be aborted.
 *
 * \param message error message
 */
void
warning (const char *message)
{
  error_handler(-1, "Warning", message);
}

/**
 * \brief Error function
 *
 * It shows an error message, the programm will not be aborted.
 *
 * \param message error message
 */
void
error (const char *message)
{
  error_handler(-1, "ERROR", message);
}

/**
 * \brief Fatal error function
 *
 * It shows an error message, the programm will be aborted.
 *
 * \param message error message
 */
void
fatal (const char *message)
{
  error_handler(EXIT_FAILURE, "FATAL", message);
}
