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
 * \file sapec-ng.c
 *
 * \brief This is the main file of the project (without GUI) and it contains
 * core functions.
 *
 * Here the core functions are present. There are a command-line options
 * handler, usage and info functions and the resolve function that is, of
 * course, the function which to compute the results is demanded. There is also
 * the main function, obviously.
 */

#include "common.h"
#include "parser.h"
#include "circuit.h"
#include "list.h"
#include "expr.h"

extern int
yyparse (circ_t*);

/**
 * \brief Usage function
 *
 * This function explains how to use the program from the command-line.
 */
void
usage ()
{
  printf("\n");
  printf("Symbolic Analysis Program for Electric Circuit\n");
  printf("Usage: sapec-ng [OPTION] [FILE]\n");
  printf("-------------------------------------------\n");
  printf("Option:\n \
  -h : this help\n \
  -i : informations about sapec-ng\n \
  -v : verbose mode\n \
  -s : SapWin compatibility (reverse current generator)\n \
  -b : input from binary file\n");
  printf("\n");
}

/**
 * \brief Sapec-NG related informations
 *
 * It permits to get more informations about Sapec-NG.
 */
void
info ()
{
  printf("\n");
  printf("Sapec-NG: Next Generation Sapec Core\n");
  printf("Symbolic Analysis Program for Electric Circuit\n");
  printf("Version: %s\n", VERSION);
  printf("Copyright (C)  2007  Michele Caini <skypjack@gmail.com>\n");
  printf("\n");
}

/**
 * \brief Core function
 *
 * This function manages the flow of data through the program and invokes
 * several functions in a correct manner, so that the results will be computed.
 *
 * \param ifile input file (which is where circuit is stored)
 */
void
resolve (const char* ifile)
{
  int ul;
  int dl;
  int length;
  circ_t* crep;
  list_t* yrefchain;
  list_t* grefchain;
  char* buf;
  extern FILE* yyin;
  FILE* fref;
  if(ifile != NULL) {
    yrefchain = NULL;
    grefchain = NULL;
    crep = XMALLOC(circ_t, 1);
    circ_init(crep);
    // parser link ! :-)
    if((yyin = fopen(ifile, "r")) != NULL) {
      VERBOSE(".");
      yyparse(crep);
      fclose(yyin);
    }
    // ! parser link !
    VERBOSE(".");
    circ_normalize(crep);
    VERBOSE(".");
    if(circ_to_expr(crep, &yrefchain, &grefchain)) {
      VERBOSE(".");
      length = strlen(ifile);
      buf = XMALLOC(char, length + 4 + 1);
      strncpy(buf, ifile, length);
      buf[length] = '\0';
      strcat(buf, ".out");
      if((fref = fopen(buf, "w")) != NULL) {
	VERBOSE(".");
        ul = splash((expr_t*) grefchain, NULL, 0);
        dl = splash((expr_t*) yrefchain, NULL, 0);
        splash((expr_t*) grefchain, fref, 1);
        sep(((dl > ul) ? dl : ul), fref);
        splash((expr_t*) yrefchain, fref, 1);
        fclose(fref);
      }
      buf[length] = '\0';
      strcat(buf, ".fdt");
      if((fref = fopen(buf, "wb")) != NULL) {
	VERBOSE(".");
	expr_to_file((expr_t*) grefchain, fref);
	expr_to_file((expr_t*) yrefchain, fref);
	fclose(fref);
      }
      XFREE(buf);
      free_expr((expr_t*) yrefchain);
      free_expr((expr_t*) grefchain);
    }
    circ_del(crep);
  }
  VERBOSE(".\n");
}

/**
 * \brief Binary file resolution function
 *
 * This function loads a binary file containing expression data and splashes
 * theme in a standard manner.
 *
 * \param ifile input binary file
 */
void
load_and_splash (const char* ifile)
{
  int ul;
  int dl;
  int length;
  list_t* yrefchain;
  list_t* grefchain;
  char* buf;
  FILE* fref;
  if(ifile != NULL) {
    yrefchain = NULL;
    grefchain = NULL;
    if((fref = fopen(ifile, "r")) != NULL) {
      VERBOSE("parsing file ... \n");
      grefchain = (list_t*) expr_from_file(fref);
      yrefchain = (list_t*) expr_from_file(fref);
      fclose(fref);
    }
    length = strlen(ifile);
    buf = XMALLOC(char, length + 4 + 1);
    strncpy(buf, ifile, length);
    buf[length] = '\0';
    strcat(buf, ".out");
    if((fref = fopen(buf, "w")) != NULL) {
      VERBOSE("writing text file ...\n");
      ul = splash((expr_t*) grefchain, NULL, 0);
      dl = splash((expr_t*) yrefchain, NULL, 0);
      splash((expr_t*) grefchain, fref, 1);
      sep(((dl > ul) ? dl : ul), fref);
      splash((expr_t*) yrefchain, fref, 1);
      fclose(fref);
    }
    XFREE(buf);
    free_expr((expr_t*) yrefchain);
    free_expr((expr_t*) grefchain);
  }
  VERBOSE("terminate ...\n");
}

/**
 * \brief The main function
 *
 * This is the main function. It handles command-line options and calls the core
 * function to compute the results. Nothing more.
 *
 * \param argc number of command-line options
 * \param argv command-line options
 * \return exit with success / exit with failure
 */
int
main (int argc, char** argv)
{
  char opt;
  CLEAR_FLAGS();
  SET_RUNNABLE();
  while((opt = getopt(argc, argv, "bsvih")) != -1) {
    switch(opt){
    case 'v':
      SET_VERBOSE();
      break;
    case 'i':
      SET_INFO();
      break;
    case 'h':
      SET_HELP();
      break;
    case 's':
      SET_SAPWIN();
      break;
    case 'b':
      SET_BINARY();
      break;
    default:
      SET_HELP();
      printf("Unknow option: %c\n", optopt);
    }
  }
  if(HELP()) {
    usage();
    CLEAR_FLAGS();
  }
  if(INFO()) {
    info();
    CLEAR_FLAGS();
  }
  if(BINARY()) {
    if(optind < argc) load_and_splash(argv[optind]);
    else load_and_splash("./circuit.fdt");
    CLEAR_FLAGS();
  }
  if(RUNNABLE()) {
    if(optind < argc) resolve(argv[optind]);
    else resolve("./circuit");
  }
  return EXIT_SUCCESS;
}
