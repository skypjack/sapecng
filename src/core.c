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
 * \file core.c
 *
 * \brief Core routine function
 *
 * This file contains a collection of functions used to find common trees
 * between graphs. There is an entry point and several private functions, used
 * by the main routine to do the work.
 */

#include "common.h"
#include "expr.h"
#include "list.h"
#include "circuit.h"

/**
 * \brief Bridge-Finder support function
 *
 * \internal
 * Support function of the bf routine.
 *
 * \param node node to explore
 * \param hnp graph representation head
 * \param nmem nodes related memory
 * \param emem edges related memory
 * \return backtrack size
 */
static uint
bf_support (const node_t node, hn_t* hnp, uint* nmem, uint* emem)
{
  uint backtrack;
  uint bt;
  tn_t* tni;
  backtrack = bt = 0;
  while(hnp->node != node) {
    if(hnp->node < node) {
      hnp = hnp->next;
    } else {
      hnp = hnp->prev;
    }
  }
  tni = hnp->nodes;
  while(tni) {
    if(!emem[tni->edge]) {
      ++nmem[tni->node];
      ++emem[tni->edge];
      if(nmem[tni->node] >= 2) {
	++emem[tni->edge];
	++backtrack;
      } else {
	bt = bf_support(tni->node, hnp, nmem, emem);
	if (bt > 0) {
	  ++emem[tni->edge];
	  backtrack += bt;
	}
      }
      while (nmem[node] >= 2) {
	--backtrack;
	--nmem[node];
      }
      tni = list_next_entry(tn_t, tni);
    }
  }
  return backtrack;
}

/**
 * \brief Bridge-Finder entry point
 *
 * \internal
 * Useful function used to initialize memory,
 *
 * \param hnp graph representation head
 * \param nmem nodes related memory
 * \param nmsize size of nodes related memory
 * \param emem edges related memory
 * \param emsize size of edges related memory
 * \param bridge bridges related memory
 * \param mark invocation counter
 */
static void
bridgefind (const hn_t* hnp, uint* nmem, const size_t nmsize, uint* emem, const size_t emsize, uint* bridge, const int mark)
{
  size_t iter;
  for (iter = 0; iter < nmsize; ++iter) { nmem[iter] = 0; }
  for (iter = 0; iter < emsize; ++iter) { emem[iter] = 0; }
  while(hnp) {
    if(!nmem[hnp->node]) {
      ++nmem[hnp->node];
      if(bf_support(hnp->node, hnp, nmem, emem)) {
	error("bridge finder internal error");
      }
    }
    hnp = list_next_entry(hn_t, hnp);
  }
  for (iter = 0; iter < emsize; ++iter) {
    if (emem[iter] == 1 && !bridge[iter])
      bridge[iter] = mark;
  }
}

/**
 * \brief Cycle-Finder support function
 *
 * \internal
 * Support function of the cf routine.
 *
 * \param node node to explore
 * \param hnp graph representation head
 * \param nmem nodes related memory
 * \return 1 if graph contains a cycle, 0 otherwise
 */
static uint
cf_support (const node_t node, hn_t* hnp, uint* nmem)
{
  uint res;
  tn_t* tni;
  res = 0;
  while(hnp->node != node) {
    if(hnp->node < node) {
      hnp = hnp->next;
    } else {
      hnp = hnp->prev;
    }
  }
  tni = hnp->nodes;
  while(tni && !res) {
    if(!nmem[tni->node]) {
      ++nmem[tni->node];
      res = cf_support(tni->node, hnp, nmem);
    } else ++res;
    tni = list_next_entry(tn_t, tni);
  }
  return res;
}

/**
 * \brief Cicle-finder entry point
 *
 * \internal
 * Useful function used to initialize memory
 *
 * \param hnp graph representation head
 * \param nmem nodes related memory
 * \param nmsize size of nodes related memory
 * \return 1 if graph contains a cycle, 0 otherwise
 */
static uint
ciclefind (const hn_t* hnp, uint* nmem, const size_t nmsize)
{
  uint res;
  size_t iter;
  res = 0;
  for (iter = 0; iter < nmsize; ++iter) { nmem[iter] = 0; }
  while(hnp && !res) {
    if(!nmem[hnp->node]) {
      ++nmem[hnp->node];
      res = cf_support(hnp->node, hnp, nmem);
    }
    hnp = list_next_entry(hn_t, hnp);
  }
  return res;
}

// GRIMBLEBY FUNCTIONS

/**
 * \brief This function is used by \e circ_to_expr function to complete its work
 *
 * \internal
 * This function is the core of grimbleby's algorithm, that means this function
 * finds all the common trees between two proposed graphs storing them as an
 * ordered, human readable circuit expression (thanks to to_expr function).
 *
 * \param crep circuit representation reference
 * \param chain %list pointer wiht the newly allocated stacks as payload
 * \param ccgi circuit graph's common components
 * \param ccgv voltage graph's common components
 * \result zero if some error occurs, a positive value otherwise
 */
static int
grimbleby (const circ_t* crep, list_t** chain, int* ccgi, int* ccgv)
{
  int ret;
  int pos;
  int cnt;
  int sdim;
  node_t* nodes;
  int* giimat;
  int* gvimat;
  int* mask;
  int iter;
  int maskmark;
  expr_t* elist;
  enum {
    TF,  // Test Flag
    SF,  // Select Flag
    LF,  // Loop Flag
    IF,  // Include Flag
    EF,  // End Flag
    BF,  // BackTrack Flag
    OF  // Out Flag
  } flag = SF;
  ret = 1;
  pos = -1;
  cnt = 0;
  sdim = crep->nnum - 1 - crep->efnum - 1;
  nodes = XMALLOC(node_t, sdim);
  mask = XMALLOC(int, crep->ednum);
  giimat = XMALLOC(int, (crep->nnum * (crep->nnum - 1)));
  gvimat = XMALLOC(int, (crep->nnum * (crep->nnum - 1)));
  elist = NULL;
  for(iter = 0; iter < crep->ednum; ++iter)
    mask[iter] = 0;
  maskmark = 0;
  while((ret)&&(flag != OF)) {
    switch(flag) {
    case TF:
      if(cnt == sdim) {
	VERBOSE(".");
	// "burn"
	elist = to_expr (crep, nodes, mask, maskmark++, giimat, gvimat, elist);
	// ! "burn"
	flag = BF;
      } else flag = SF;
      break;
    case SF:
      ++pos;
      if(sdim - cnt > crep->ednum - pos) flag = EF;
      else flag = LF;
      break;
    case LF:
      if(testloop(ccgi, crep->gi[2*pos], crep->gi[2*pos+1])) flag = SF;
      else if(testloop(ccgv, crep->gv[2*pos], crep->gv[2*pos+1])) flag = SF;
      else flag = IF;
      break;
    case IF:
      if(cnt == sdim) ret = 0;
      else {
	nodes[cnt++] = pos;
	ctrlplus(ccgi, crep->gi[2*pos], crep->gi[2*pos+1], crep->nnum);
	ctrlplus(ccgv, crep->gv[2*pos], crep->gv[2*pos+1], crep->nnum);
	flag = TF;
      }
      break;
    case EF:
      if(cnt == 0) flag = OF;
      else flag = BF;
      break;
    case BF:
      if(cnt == 0) ret = 0;
      else {
	pos = nodes[--cnt];
	ctrlminus(ccgi, crep->gi[2*pos], crep->gi[2*pos+1], crep->nnum);
	ctrlminus(ccgv, crep->gv[2*pos], crep->gv[2*pos+1], crep->nnum);
	flag = SF;
      }
      break;
    case OF:
      break;
    }
  }
  *chain = (list_t*) elist;
  XFREE(gvimat);
  XFREE(giimat);
  XFREE(mask);
  XFREE(nodes);
  return ret;
}

/**
 * \brief Circuit-to-expression conversion function
 *
 * This function permits to convert a %circuit into an expression composed by
 * some tokens that involve names, degree, numeric part and so on; expressions
 * are final, easy to manage parts of the resolution process. As a matter of
 * fact, this function does more: indeed, it returns a tight and sorted
 * expression, all-in-one! :-)
 *
 * \param crep %circuit reference
 * \param yrefchain pointer to be used to store the first %list
 * \param grefchain pointer to be used to store the second %list
 * \result zero if some error occurs, a positive value otherwise
 */
int
circ_to_expr (circ_t* crep, list_t** yrefchain, list_t** grefchain)
{
  int ret;
  int* ccgi;
  int* ccgv;
  int iter;
  forced_t* fptr;
  isolated_t* iptr;
  int (*cf) (const circ_t*, list_t**, int*, int*);
  cf = grimbleby;
  if(crep != NULL) {
    ret = 1;
    ccgi = XMALLOC(int, 2*crep->nnum);
    ccgv = XMALLOC(int, 2*crep->nnum);
    for(iter = 0; iter < crep->nnum; ++iter) {
      ccgi[2*iter] = iter;
      ccgv[2*iter] = iter;
      ccgi[2*iter + 1] = -1;
      ccgv[2*iter + 1] = -1;
    }
    fptr = crep->flist;
    while((fptr != NULL) && (ret)) {
      if(testloop(ccgi, fptr->gitoken[0], fptr->gitoken[1])) ret = 0;
      if(testloop(ccgv, fptr->gvtoken[0], fptr->gvtoken[1])) ret = 0;
      ctrlplus(ccgi, fptr->gitoken[0], fptr->gitoken[1], crep->nnum);
      ctrlplus(ccgv, fptr->gvtoken[0], fptr->gvtoken[1], crep->nnum);
      fptr = list_next_entry(forced_t, fptr);
    }
    if(crep->yref != NULL) {
      set_support_yref(crep);
      iptr = crep->yref;
      ctrlplus(ccgi, iptr->gitoken[0], iptr->gitoken[1], crep->nnum);
      ctrlplus(ccgv, iptr->gvtoken[0], iptr->gvtoken[1], crep->nnum);
      if(ret) ret = (*cf)(crep, yrefchain, ccgi, ccgv);
      ctrlminus(ccgi, iptr->gitoken[0], iptr->gitoken[1], crep->nnum);
      ctrlminus(ccgv, iptr->gvtoken[0], iptr->gvtoken[1], crep->nnum);
    }
    if(crep->gref != NULL) {
      set_support_gref(crep);
      iptr = crep->gref;
      ctrlplus(ccgi, iptr->gitoken[0], iptr->gitoken[1], crep->nnum);
      ctrlplus(ccgv, iptr->gvtoken[0], iptr->gvtoken[1], crep->nnum);
      if(ret) ret = (*cf)(crep, grefchain, ccgi, ccgv);
      ctrlminus(ccgi, iptr->gitoken[0], iptr->gitoken[1], crep->nnum);
      ctrlminus(ccgv, iptr->gvtoken[0], iptr->gvtoken[1], crep->nnum);
    }
    XFREE(ccgi);
    XFREE(ccgv);
  } else {
    warning("Null pointer!");
    ret = 0;
  }
  return ret;
}
