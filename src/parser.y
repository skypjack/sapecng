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

%defines
%error_verbose

%{
#include "common.h"
#include "circuit.h"
#include "circapi.h"

int
yylex (void);

void
yyerror (const circ_t*, const char*);

int
yyparse (circ_t*);

void
elm (int na, int nb, const int nac, const int nbc, const int sym, const double val, char* id, circ_t* crep)
{
  int swp, noerr;
  if((na < 0) || (nb < 0) || (nac < 0) || (nbc < 0))
    yyerror(crep, "negative node ... what's kind of node is it?");
  if(sym < 0)
    yyerror(crep, "to be symbolic or not to be symbolic? negative value means nothing ...");
  noerr = 1;
  switch(id[0]) {
  case 'R':
    noerr &= add_R (crep, id, na, nb, val, sym);
    break;
  case 'L':
    noerr &= add_L (crep, id, na, nb, val, sym);
    break;
  case 'G':
    noerr &= add_G (crep, id, na, nb, val, sym);
    break;
  case 'C':
    noerr &= add_C (crep, id, na, nb, val, sym);
    break;
  case 'V':
    noerr &= add_V (crep, id, na, nb, val, sym);
    break;
  case 'I':
    if(SAPWIN()) {
      swp = na;
      na = nb;
      nb = swp;
    }
    noerr &= add_I (crep, id, na, nb, val, sym);
    break;
  case 'H':  // VCCS
    noerr &= add_VCCS (crep, id, na, nb, nac, nbc, val, sym);
    break;
  case 'E':  // VCVS
    noerr &= add_VCVS (crep, id, na, nb, nac, nbc, val, sym);
    break;
  case 'F':  // CCCS
    noerr &= add_CCCS (crep, id, na, nb, nac, nbc, val, sym);
    break;
  case 'Y':  // CCVS
    noerr &= add_CCVS (crep, id, na, nb, nac, nbc, val, sym);
    break;
  case 'A':  // AMP.OP.
    noerr &= add_OP_AMPL (crep, id, na, nb, nac, nbc);
    break;
  }
  if(!noerr)
    yyerror(crep, "parser internal error adding celm!");
}

%}

%parse-param { circ_t* crep }

%union {
  char* string;
  int integer;
  double real;
}

%token  <string>  SID
%token  <string>  CID
%token  <string>  ACID
%token  <integer>  INT
%token  <real>  REAL
%token  END
%token  OUT

%%

circ: elm_tokens END
    {
      setblock(crep);
    }
  ;

elm_tokens:
  | elm_tokens selm
  | elm_tokens celm
  | elm_tokens acelm
  | elm_tokens out
  ;

selm: SID INT INT REAL INT
    {
      int na, nb, sym;
      double val;
      char* id;
      id = $<string>1;
      na = $<integer>2;
      nb = $<integer>3;
      val = $<real>4;
      sym = $<integer>5;
      elm(na, nb, 0, 0, !sym, val, id, crep);
      XFREE(id);
    }
  | SID INT INT INT INT
    {
      int na, nb, sym, ival;
      double val;
      char* id;
      id = $<string>1;
      na = $<integer>2;
      nb = $<integer>3;
      ival = $<integer>4;
      sym = $<integer>5;
      val = (double) ival;
      elm(na, nb, 0, 0, !sym, val, id, crep);
      XFREE(id);
    }
  ;

celm: CID INT INT INT INT REAL INT
    {
      int na, nb, nac, nbc, sym;
      double val;
      char* id;
      id = $<string>1;
      na = $<integer>2;
      nb = $<integer>3;
      nac = $<integer>4;
      nbc = $<integer>5;
      val = $<real>6;
      sym = $<integer>7;
      elm(na, nb, nac, nbc, !sym, val, id, crep);
      XFREE(id);
    }
  | CID INT INT INT INT INT INT
    {
      int na, nb, nac, nbc, sym, ival;
      double val;
      char* id;
      id = $<string>1;
      na = $<integer>2;
      nb = $<integer>3;
      nac = $<integer>4;
      nbc = $<integer>5;
      ival = $<integer>6;
      sym = $<integer>7;
      val = (double) ival;
      elm(na, nb, nac, nbc, !sym, val, id, crep);
      XFREE(id);
    }
  ;

acelm: ACID INT INT INT INT
    {
      int na, nb, nac, nbc;
      char* id;
      id = $<string>1;
      na = $<integer>2;
      nb = $<integer>3;
      nac = $<integer>4;
      nbc = $<integer>5;
      elm(na, nb, nac, nbc, 0, 0, id, crep);
      XFREE(id);
    }

out: OUT INT
    {
      int node;
      node = $<integer>2;
      if(node < crep->nnum)
      crep->onode = node;
      else yyerror(crep, "wrong output node value");
    }
  ;

%%

void
yyerror (const circ_t* crep, const char* err)
{
  fatal(err);
}
