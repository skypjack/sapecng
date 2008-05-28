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
 * \file circuit.h
 *
 * \brief Circuit type and related values
 *
 * This file contains a definition for the circuit structure and some related
 * values and structures that are parts of the circuit.
 */

/**
 * \brief Useful to manage multiple inclusions
 */
#ifndef CIRCUIT_H
#define CIRCUIT_H 1

#include "common.h"

/**
 * \brief Base size of the internal representation
 *
 * It represents the default size of the internal structures of the circuit;
 * this structures has dynamic size and grows accordingly to use of the circuit.
 */
#define STDDIM 32

/**
 * \brief Max number of nodes
 *
 * The circuit can contains a number of nodes less than or equal to \e LIMIT.
 */
#define LIMIT 1024

/**
 * \brief yref as support %edge
 *
 * This macro sets yref as support %edge.
 */
#define set_support_yref(crep) \
  crep->esupport = (isolated_t*) crep->yref

/**
 * \brief gref as support %edge
 *
 * This macro sets gref as support %edge.
 */
#define set_support_gref(crep) \
  crep->esupport = (isolated_t*) crep->gref

/**
 * \brief Possible types for %edge
 *
 * Edges (logical components of the circuit that join together two nodes) can
 * assumes a limited set of values that defines them type.
 */
enum etype
{
  Z,  /**< Impedance type */
  Y,  /**< Conductance type */
  F,  /**< Forced type */
  YREF,  /**< Part of additional block (special type) */
  GREF  /**< Part of additional block (special type) */
};

/**
 * \brief Simpler %enum %etype definition
 */
typedef
enum etype
etype_t;

/**
 * \brief Node type
 */
typedef
short int
node_t;

/**
 * \brief Edge type
 *
 * This structure defines the type for links between nodes, so that these links
 * can be associated with names, types, degrees, values and status; these
 * informations are very important for the symbolic resolution of the circuit.
 */
struct edge
{
  char* name;  /**< Name of the link */
  etype_t type;  /**< Type of the link */
  short int degree;  /**< Degree of the link */
  short int sym;  /**< Status of the link (symbolic or not) */
  double value;  /**< Value of the link */
};

/**
 * \brief Simpler %struct %edge definition
 */
typedef
struct edge
edge_t;

/**
 * \brief Forced %edge type
 *
 * Edges of this type are %forced edges and it means that during common trees
 * find algorithm they are imperatively always present into these trees; there
 * are more "free" edges that %forced edges and the seconds are represented with
 * a special kind of %list (not the standard one of the project).
 */
struct forced
{
  struct forced* next;  /**< Pointer to the next element of the %list */
  edge_t* data;  /**< Edge related informations block */
  node_t gitoken[2];  /**< Current graph representation */
  node_t gvtoken[2];  /**< Voltage graph representation */
};

/**
 * \brief Simpler %struct %forced definition
 */
typedef
struct forced
forced_t;

/**
 * \brief Special additional %edge type
 *
 * Edges of this type are special edges that are added to the circuit because
 * they are the only way by means of which the circuit can be solved.
 */
struct isolated
{
  edge_t* data;  /**< Edge related informations block */
  node_t gitoken[2];  /**< Current graph representation */
  node_t gvtoken[2];  /**< Voltage graph representation */
};

/**
 * \brief Simpler %struct %isolated definition
 */
typedef
struct isolated
isolated_t;

/**
 * \brief Alternative %struct %isolated definition
 */
typedef
isolated_t
yref_t;

/**
 * \brief Alternative %struct %isolated definition
 */
typedef
isolated_t
gref_t;

/**
 * \brief Circuit type
 *
 * It represents the %circuit structure within all needed informations.
 */
struct circ
{
  node_t reserved;  /**< Reserved node (base free counter) */
  node_t reference;  /**< Reference node for voltage dependent sources */
  node_t basenode;  /**< Ground node (default is zero) */
  node_t onode;  /**< Output node */
  int offset;  /**< Offset used to calculate free available node */
  int nnum;  /**< Number of nodes (%onode must be smaller than %nnum) */
  int ednum;  /**< Number of standard edges */
  int efnum;  /**< Number of %forced edges */
  int dim;  /**< Size of internal representation */
  int free;  /**< Number of free nodes */
  edge_t* edge;  /**< Edges store */
  node_t* gi;  /**< Current circuit representation */
  node_t* gv;  /**< Voltage circuit representation */
  forced_t* flist;  /**< Forced edges %list */
  isolated_t* esupport; /**< Support edge for a tree */
  yref_t* yref;  /**< Special %edge */
  gref_t* gref;  /**< Special %edge */
};

/**
 * \brief Simpler %struct %circ definition
 */
typedef
struct circ
circ_t;

extern void
circ_del (circ_t*);

extern void
circ_init (circ_t*);

extern node_t
circ_getfree (circ_t*);

extern void
circ_normalize (circ_t*);

extern int
setblock (circ_t*);

extern int
addsimple (circ_t*, const node_t, const node_t, const node_t, const node_t, char*, const etype_t, const short int, const double, const int);

extern int
addnullor (circ_t*, const node_t, const node_t, const node_t, const node_t, char*, const double, const int);

#endif /* CIRCUIT_H */
