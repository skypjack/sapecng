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
#include "list.h"

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
 * \brief Edge reference number
 *
 * Short and compact way to retrieve %edge reference number.
 */
#define edge_number(crct, ref) \
  (int)(((int)ref - (int)crct->edge) / sizeof(edge_t))

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
 * \brief Tail node type
 */
typedef
struct tail_node
tn_t;

/**
 * \brief Head node
 *
 * The head node is that one edges come from, of course the tail of the
 * edges. Edges head are represented by a sub-list of nodes called tail
 * node. Yes, this is a rebus!
 */
struct head_node
{
  struct head_node* next;  /**< Successor in the %list */
  struct head_node* prev;  /**< Previous in the %list */
  node_t node;  /**< Node identifier */
  tn_t* nodes;  /**< Sub-list of linked nodes */
};

/**
 * \brief Head node type
 */
typedef
struct head_node
hn_t;

/**
 * \brief Tail node
 *
 * The tail node is that one edges come to, of course the head of
 * the edges. Edges tail is represented by a link to a node called head
 * node. Yes, this is a rebus!
 */
struct tail_node
{
  tn_t* next;  /**< Successor in the %list */
  tn_t* prev;  /**< Previous in the %list */
  hn_t* head;  /**< Head (tail of %edge) reference */
  node_t node;  /**< Node identifier */
  uint edge;  /**< Related %edge identifier */
};

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
  tn_t* giref[2];  /**< References into the current graph */
  tn_t* gvref[2];  /**< References into the voltage graph */
};

/**
 * \brief Simpler %struct %edge definition
 */
typedef
struct edge
edge_t;

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
  int dim;  /**< Number of allocated edges */
  int free;  /**< Number of free edges */
  edge_t* edge;  /**< Edges store */
  hn_t* gi;  /**< Current circuit representation */
  hn_t* gv;  /**< Voltage circuit representation */
  list_t* flist;  /**< Forced edges %list */
  edge_t* yref;  /**< Extra component involved into resolution process */
  edge_t* gref;  /**< Extra component involved into resolution process */
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
addsimple (circ_t*, const node_t, const node_t, const node_t, const node_t, const char*, const etype_t, const short int, const double, const int);

extern int
addnullor (circ_t*, const node_t, const node_t, const node_t, const node_t, const char*, const double, const int);

#endif /* CIRCUIT_H */
