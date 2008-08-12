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
 * \file circuit.c
 *
 * \brief Functions related to circuit structure
 *
 * This file contains some useful functions that can be used to create,
 * initialize, use, manage and destroy a circuit and all types of edges.
 */

#include "common.h"
#include "list.h"
#include "circuit.h"

/**
 * \brief Support function useful to delete a graph
 *
 * \internal
 * Using this function current and voltage graph adjacency representations can be
 * correctly and easily deleted.
 *
 * \param adjref graph adjacency reperesentation reference
 */
static void
adj_rep_del (hn_t* adjref)
{
  tn_t* tntmp;
  hn_t* hntmp;
  while(adjref) {
    while(adjref->nodes) {
      tntmp = adjref->nodes;
      adjref->nodes = list_next_entry(tn_t, adjref->nodes);
      XFREE(tntmp);
    }
    hntmp = adjref;
    adjref = list_next_entry(hn_t, adjref);
    XFREE(hntmp);
  }
}

/**
 * \brief circ_del frees the memory space pointed to by \a crct
 *
 * This function frees the memory space pointed to by \a %crct; if \a %crct is a
 * non-valid pointer, no operation is performed.
 *
 * \param crct circuit reference
 */
void
circ_del (circ_t* crct)
{
  int iter;
  if(crct != NULL) {
    list_del(crct->flist);
    adj_rep_del(crct->gi);
    adj_rep_del(crct->gv);
    for(iter = 0; iter < crct->ednum; ++iter)
      XFREE(crct->edge[iter].name);
    XFREE(crct->edge);
    XFREE(crct);
  } else warning("Null pointer!");
}

/**
 * \brief Extension function for circuit structure
 *
 * \internal
 * Using this function the size of the internal structures of the circuit will
 * be expanded (the structures will result of double size), that is, more edges
 * will be allocated.
 *
 * \param crct circuit reference
 */
static void
circ_ext (circ_t* crct)
{
  crct->free += (crct->dim) ? crct->dim : STDDIM; 
  crct->dim = (crct->dim) ? crct->dim * 2 : STDDIM;
  crct->edge = XREALLOC(edge_t, crct->edge, crct->dim);
}

/**
 * \brief Control function about available space of the internal representation
 *
 * \internal
 * Calling this function results in check for available free space into the
 * internal structure of the circuit, and in case a consequent expansion.
 *
 * \param crct circuit reference
 */
static void
circ_chkfree (circ_t* crct)
{
  if(crct->free == 0)
    circ_ext(crct);
}

/**
 * \brief Function used to add head nodes
 *
 * \internal
 * This is a support function useful to add head nodes
 *
 * \param head head of the graph
 * \param node node value to be added
 * \return node of interesting
 */
static hn_t*
add_head (hn_t** head, const node_t node)
{
  hn_t* tmp;
  while(*head && (*head)->node < node)
    head = &((*head)->next);
  if(!*head || (*head && (*head)->node != node)) {
    tmp = *head;
    *head = XMALLOC(hn_t, 1);
    (*head)->next = tmp;
    (*head)->prev = tmp ? tmp->prev : NULL;
    if((*head)->prev)
      (*head)->prev->next = *head;
    if((*head)->next)
      (*head)->next->prev = *head;
    (*head)->node = node;
    (*head)->nodes = NULL;
  }
  return *head;
}

/**
 * \brief Function used to add tail nodes
 *
 * \internal
 * This is a support function useful to add tail nodes
 *
 * \param tail %list of edges
 * \param head %head node reference
 * \param node node value to be added
 * \param eref related %edge value
 * \return node of interesting
 */
static tn_t*
add_tail (tn_t** tail, hn_t* head, const node_t node, const uint eref)
{
  tn_t* tmp;
  tmp = *tail;
  *tail = XMALLOC(tn_t, 1);
  (*tail)->prev = NULL;
  (*tail)->next = tmp;
  if((*tail)->next)
    (*tail)->next->prev = *tail;
  (*tail)->head = head;
  (*tail)->node = node;
  (*tail)->edge = eref;
  return *tail;
}

/**
 * \brief Adding new %edge to the circuit is simpler, now.
 *
 * \internal
 * This function permits to add new edges to the circuit via a simple way.
 *
 * \param crct circuit reference
 * \param gih %edge's head node into the current representation
 * \param git %edge's tail node into the current representation
 * \param gvh %edge's head node into the voltage representation
 * \param gvt %edge's tail node into the voltage representation
 * \result new raw %edge reference
 */
static edge_t*
circ_addedge (circ_t* crct, const node_t git, const node_t gih, const node_t gvt, const node_t gvh)
{
  edge_t* ret;
  hn_t* head;
  int max;
  circ_chkfree(crct);
  max = 0;
  ret = &(crct->edge[crct->dim - crct->free]);
  head = add_head(&(crct->gi), git);
  ret->giref[0] = add_tail(&(head->nodes), head, gih, crct->dim - crct->free);
  head = add_head(&(crct->gi), gih);
  ret->giref[1] = add_tail(&(head->nodes), head, git, crct->dim - crct->free);
  head = add_head(&(crct->gv), gvt);
  ret->gvref[0] = add_tail(&(head->nodes), head, gvh, crct->dim - crct->free);
  head = add_head(&(crct->gv), gvh);
  ret->gvref[1] = add_tail(&(head->nodes), head, gvt, crct->dim - crct->free);
  --(crct->free);
  ++(crct->ednum);
  if((gih < LIMIT) && (gih > max)) max = gih;
  if((git < LIMIT) && (git > max)) max = git;
  if((gvh < LIMIT) && (gvh > max)) max = gvh;
  if((gvt < LIMIT) && (gvt > max)) max = gvt;
  crct->nnum = ((max + 1 ) > crct->nnum) ? (max + 1) : crct->nnum;
  return ret;
}

/**
 * \brief Adding new %forced %edge to the circuit is simpler, now.
 *
 * \internal
 * This function permits to add new %forced edges to the circuit via a simple
 * way, based on %circ_addedge function. Forced edges are tracked like payload
 * into an ad-hoc list for further elaboration.
 *
 * \param crct circuit reference
 * \param gih %edge's head node into the current representation
 * \param git %edge's tail node into the current representation
 * \param gvh %edge's head node into the voltage representation
 * \param gvt %edge's tail node into the voltage representation
 * \result new raw %edge reference
 */
static edge_t*
circ_addforced (circ_t* crct, const node_t git, const node_t gih, const node_t gvt, const node_t gvh)
{
  edge_t* ret;
  ret = circ_addedge (crct, git, gih, gvt, gvh);
  ++(crct->efnum);
  crct->flist = list_add(list_new(ret), crct->flist);
  return ret;
}

/**
 * \brief Circuit initialization function
 *
 * This function is used to initialize a pre-allocated circuit; if \a crct is a
 * non-valid pointer, no operation is performed.
 *
 * \param crct a pointer to the allocated circuit
 */
void
circ_init (circ_t* crct)
{
  if(crct != NULL) {
    crct->reference = 0;
    crct->offset = 0;
    crct->reserved = LIMIT + crct->offset++;
    crct->basenode = 0;
    crct->onode = 0;
    crct->nnum = 0;
    crct->ednum = 0;
    crct->efnum = 0;
    crct->dim = 0;
    crct->free = 0;
    crct->edge = NULL;
    crct->gi = NULL;
    crct->gv = NULL;
    crct->flist = NULL;
    crct->yref = NULL;
    crct->gref = NULL;
    circ_ext(crct);
  } else warning("Null pointer!");
}

/**
 * \brief It returns the next free available node of the circuit
 *
 * Building the circuit, there are some cases that require a temporary free
 * unused node to absolve their jobs; of course, circuit structure knows what
 * are its limits in term of max node number and it can distributes free nodes
 * references that need to be normalized subsequently.
 *
 * \param crct circuit reference
 * \result available free node number if \a crct is a valid pointer, zero
 *   otherwise.
 */
node_t
circ_getfree (circ_t* crct)
{
  node_t ret;
  if(crct != NULL)
    ret = LIMIT + crct->offset++;
  else {
    warning("Null pointer!");
    ret = 0;
  }
  return ret;
}

/**
 * \brief Adjacency %list normalization function
 *
 * \internal
 * It normalizes adjacency %list graph representation.
 *
 * \param adjref adjacency %list graph representation reference
 * \param nnum number of graph nodes
 */
static void
adj_rep_normalize (hn_t* adjref, const int nnum)
{
  tn_t* tail;
  while(adjref) {
    if(adjref->node >= LIMIT)
      adjref->node = nnum + adjref->node % LIMIT;
    tail = adjref->nodes;
    while(tail) {
      if(tail->node >= LIMIT)
	tail->node = nnum + tail->node % LIMIT;
      tail = list_next_entry(tn_t, tail);
    }
    adjref = list_next_entry(hn_t, adjref);
  }
}

/**
 * \brief Circuit normalization function
 *
 * It normalizes the circuit, or better it maps node number from a value to
 * another one where it is necessary (in fact, it maps values which must have
 * been returned by a previous call to \e circ_getfree); if \a crct is a
 * non-valid pointer, no operation is performed.
 *
 * \param crct circuit reference
 */
void
circ_normalize (circ_t* crct)
{
  if(crct != NULL) {
    adj_rep_normalize(crct->gi, crct->nnum);
    adj_rep_normalize(crct->gv, crct->nnum);
    if(crct->reference >= LIMIT) crct->reference = crct->nnum + crct->reference % LIMIT;
    if(crct->reserved >= LIMIT) crct->reserved = crct->nnum + crct->reserved % LIMIT;
    crct->nnum += crct->offset;
    crct->offset = 0;
  } else warning("Null pointer!");
}

/**
 * \brief It permits to add the special block useful for resolution purpose.
 *
 * \internal
 * To perform symbolic analysis of the circuit is needed to add special elements
 * using specific informations that can be retrived directly from the circuit
 * itself; this function uses \e circ_addyref and \e circ_addgref submitting
 * appropriate values to do that.
 *
 * \param crep circuit reference
 * \result zero if some error occurs, a positive value otherwise
 */
int
setblock (circ_t* crep)
{
  edge_t* eptr = NULL;
  int flag = 1;
  if(crep != NULL) {
    if((crep->reference)&&(crep->reserved)&&(crep->onode)) {
      if((eptr = circ_addedge(crep,crep->reference,crep->reserved,crep->reference,crep->reserved)) != NULL) {
        eptr->name = NULL;
        eptr->type = YREF;
        eptr->degree = 0;
        eptr->value = 1;
        eptr->sym = 0;
	crep->yref = eptr;
      } else {
        warning("Unable to set yref!");
        flag = 0;
      }
      if((eptr = circ_addedge(crep,crep->reference,crep->reserved,crep->basenode,crep->onode)) != NULL) {
        eptr->name = NULL;
        eptr->type = GREF;
        eptr->degree = 0;
        eptr->value = 1;
        eptr->sym = 0;
	crep->gref = eptr;
      } else {
        warning("Unable to set gref!");
        flag = 0;
      }
    } else flag = 0;
  } else flag = 0;
  return flag;
}

/**
 * \brief Entry point that can be used to add simple edges to the circuit;
 *
 * This function permits to add new simple edges to the circuit; "simple" means
 * that newly added %edge is a non-forced %edge and it will be initialized with
 * submitted informations.
 *
 * \param crep circuit reference
 * \param nh %edge's tail node into the current representation
 * \param nt %edge's head node into the current representation
 * \param nhc %edge's tail node into the voltage representation
 * \param ntc %edge's head node into the voltage representation
 * \param name %edge's string identifier (%edge associated %name)
 * \param type %edge's %type (%type of element which %edge comes from)
 * \param degree %edge's degree, it depends on element which %edge comes from
 * \param value %edge's weight, useful in combination with \a %sym equal to zero
 * \param sym %edge's status, determines where it is or not a symbolic token
 * \result zero if some error occurs, a positive value otherwise
 */
int
addsimple (circ_t* crep, const node_t nt, const node_t nh, const node_t ntc, const node_t nhc, const char* name, const etype_t type, const short int degree, const double value, const int sym)
{
  edge_t* eptr;
  int ret;
  if(crep && (eptr = circ_addedge(crep, nt, nh, ntc, nhc)) != NULL) {
    eptr->name = xstrdup(name);
    eptr->type = type;
    eptr->degree = degree;
    eptr->value = value;
    eptr->sym = sym;
    ret = 1;
  } else {
    warning("Unable to add edge!");
    ret = 0;
  }
  return ret;
}

/**
 * \brief Entry point that can be used to add nullor edges to the circuit;
 *
 * This function permits to add new nullor edges to the circuit; "nullor" means
 * that newly added %edge is a %forced %edge and some of its fields will be
 * initialized with submitted informations while the rest of its fields will be
 * initialized with default data.
 *
 * \param crep circuit reference
 * \param nh %edge's tail node into the current representation
 * \param nt %edge's head node into the current representation
 * \param nhc %edge's tail node into the voltage representation
 * \param ntc %edge's head node into the voltage representation
 * \param name %edge's string identifier (%edge associated %name)
 * \param value %edge's weight, useful in combination with \a %sym equal to zero
 * \param sym %edge's status, determines where it is or not a symbolic token
 * \result zero if some error occurs, a positive value otherwise
 */
int
addnullor (circ_t* crep, const node_t nt, const node_t nh, const node_t ntc, const node_t nhc, const char* name, const double value, const int sym)
{
  edge_t* eptr;
  int ret;
  if(crep && (eptr = circ_addforced(crep,nt,nh,ntc,nhc)) != NULL) {
    eptr->name = xstrdup(name);
    eptr->type = F;
    eptr->degree = 0;
    eptr->value = value;
    eptr->sym = sym;
    ret = 1;
  } else {
    warning("Unable to add forced!");
    ret = 0;
  }
  return ret;
}
