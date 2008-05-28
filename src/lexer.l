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

%{
#include "common.h"
#include "parser.h"
%}

%option noyywrap
%option nounput

DIGIT  [0-9]
ID  [_a-zA-Z0-9]+
SID  [R|G|C|L|V|I]
CID  [H|E|F|Y]
ACID  [A]
SIGN  [+|-]
EXP  [e|E][+|-]?[0-9]+

%%

{SID}{ID}  {
  size_t pos;
  char cback;
  pos = strcspn(yytext, " \t");
  cback = yytext[pos];
  yytext[pos] = '\0';
  yylval.string = xstrdup(yytext);
  yytext[pos] = cback;
  if(yylval.string == NULL) error("Null pointer!");
  return(SID);
}

{CID}{ID}  {
  size_t pos;
  char cback;
  pos = strcspn(yytext, " \t");
  cback = yytext[pos];
  yytext[pos] = '\0';
  yylval.string = xstrdup(yytext);
  yytext[pos] = cback;
  if(yylval.string == NULL) error("Null pointer!");
  return(CID);
}

{ACID}{ID}  {
  size_t pos;
  char cback;
  pos = strcspn(yytext, " \t");
  cback = yytext[pos];
  yytext[pos] = '\0';
  yylval.string = xstrdup(yytext);
  yytext[pos] = cback;
  if(yylval.string == NULL) error("Null pointer!");
  return(ACID);
}

{SIGN}?{DIGIT}*"."{DIGIT}+{EXP}?  {
  yylval.real = (double) strtod((const char*) yytext, 0);
  return(REAL);
}

{SIGN}?{DIGIT}+{EXP}  {
  yylval.real = (double) strtod((const char*) yytext, 0);
  return(REAL);
}

{SIGN}?{DIGIT}+  {
  yylval.integer = (int) strtol((const char*) yytext, 0, 10);
  return(INT);
}

".END"  { return(END); }

".OUT"  { return(OUT); }

"*"[^\n]*

": tell me what!? :"  {
  warning("what does the cow say? mooooo!");
}

[ \t\r\n]+

.  {
  warning(yytext);
}

<<EOF>>  {
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yyfree(yy_buffer_stack);
  yyterminate();
}

%%
