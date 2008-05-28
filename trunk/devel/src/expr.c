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
 * \file expr.c
 *
 * \brief Functions that can be used to handle expression token elements
 *
 * This set of functions can be used to creates new tokens, converts from %stack,
 * frees listed tokens, sorts them in order of their degree, and splashes them
 * in a correct manner.
 */

#include "common.h"
#include "expr.h"
#include "list.h"
#include "circuit.h"

/**
 * \brief It splashes separator
 *
 * Simply, it splashes a separator that consists in a %list of '-' characters.
 *
 * \param cnt length of the %list
 * \param fref destination file
 */
void
sep (const int cnt, FILE* fref)
{
  int iter;
  for(iter = 0; iter < cnt; ++iter)
    fprintf(fref, "-");
  fprintf(fref, "\n");
}

/**
 * \brief It splashes a single degree-group
 *
 * This is an helpful function used to splash one degree-group at a time.
 * \param elist %list of expressions
 * \param fref output file
 * \param mode modality of use (length only or length plus splash)
 * \param buf pre-allocated buffer
 * \return the length of the splashed expression
 */
static int
splash_group (expr_t** elist, FILE* fref, const int mode, char* buf)
{
  expr_t* iter;
  list_t* ehook;
  int degree;
  int dtmp;
  int length;
  int zero;
  double unl;
  double acc;
  length = 0;
  acc = 0;
  zero = 1;
  iter = *elist;
  degree = iter->degree;
  while((iter != NULL) && (iter->degree == degree)) {
    if(iter->vpart != 0) {
      zero = 0;
      if(iter->epart == NULL) {
	acc += iter->vpart;
      } else {
	if(iter->vpart > 0) {
	  if(mode) fprintf(fref, " +");
	  else length += 2;
	} else {
	  if(mode) fprintf(fref, " -");
	  else length += 2;
	}
	if((iter->vpart != 1) && (iter->vpart != -1)) {
	  unl = iter->vpart;
	  if(unl < 0) unl *= -1;
	  if(mode) fprintf(fref, " %.3g", unl);
	  else {
	    if((dtmp = snprintf(buf, BUF_SIZE, " %.3g", unl)) == -1)
	      fatal("unable to manage vpart value");
	    else length += dtmp;
	  }
	}
	ehook = iter->epart;
	while(ehook != NULL) {
	  if(mode) fprintf(fref, " %s", list_data(char, ehook));
	  else length += strlen(list_data(char, ehook)) + 1;
	  ehook = list_next(ehook);
	}
      }
    }
    iter = list_next_entry(expr_t, iter);
  }
  *elist = iter;
  if((acc != 0) || (zero)) {
    if(acc < 0) {
      if(mode) fprintf(fref, " -");
      else length += 2;
      acc *= -1;
    } else {
      if(mode) fprintf(fref, " +");
      else length += 2;
    }
    if(mode) fprintf(fref, " %.3g", acc);
    else {
      if((dtmp = snprintf(buf, BUF_SIZE, " %.3g", acc)) == -1)
	fatal("unable to manage vpart value");
      else length += dtmp;
    }
  }
  return length;
}

/**
 * \brief It splashes an expressions %list.
 *
 * This function can be used to splash a %list of expressions in a correct
 * manner and/or to know the splashed representation's length; it produces
 * something like "expr1 + ... + exprN", where exprX is like
 * "vpart * epart1 * ... * epartM * s^degree" (vpart, epart, degree are all
 * fields of %struct %expr).
 *
 * \param elist %list of expressions
 * \param fref output file
 * \param mode modality of use (length only or length plus splash)
 * \return the length of the splashed expression
 */
int
splash (expr_t* elist, FILE* fref, const int mode)
{
  int degree;
  int length;
  int dtmp;
  char* buf;
  length = 0;
  degree = -1;
  if(!mode) buf = XMALLOC(char, BUF_SIZE);
  else buf = 0;
  if(elist != NULL) {
    while(elist != NULL) {
      if(degree != -1) {
	if(mode) fprintf(fref, " + (");
	else length += 4;
      } else {
	if(mode) fprintf(fref, " (");
	else length += 2;
      }
      degree = elist->degree;
      length += splash_group(&elist, fref, mode, buf);
      if(mode) fprintf(fref, " )");
      else length += 2;
      if(degree != 0) {
	if(mode) fprintf(fref, " s");
	else length += 2;
	if(degree > 1) {
	  if(mode) fprintf(fref, "^%d", degree);
	  else {
	    if((dtmp = snprintf(buf, BUF_SIZE, " %d", degree)) == -1)
	      fatal("unable to manage degree value");
	    else length += dtmp;
	  }
	}
      }
    }
  } else {
    if(mode) fprintf(fref, " NULL");
    else length += 5;
  }
  ++length;
  if(mode) fprintf(fref, "\n");
  XFREE(buf);
  return length;
}

/**
 * \brief How to put an expression on file.
 *
 * It permits to copy an expression directly to file, handling correctly the
 * internal representation.
 *
 * \param elist expression reference
 * \param file file to be used
 * \return number of errors occurred
 */
int
expr_to_file (expr_t* elist, FILE* file)
{
  list_t* iter;
  size_t ll;
  int werr;
  werr = 0;
  if(file != NULL) {
    // mem token count
    ll = list_length((list_t*) elist);
    if(fwrite(&(ll), sizeof(ll), 1, file) != 1)
      werr = (werr == 0) ? 1 : werr;
    while((elist != NULL) && (!werr)) {
      // mem degree
      if(fwrite(&(elist->degree), sizeof(elist->degree), 1, file) != 1)
	werr = (werr == 0) ? 1 : werr;
      // mem vpart
      if(fwrite(&(elist->vpart), sizeof(elist->vpart), 1, file) != 1)
	werr = (werr == 0) ? 1 : werr;
      // mem etoken
      if(fwrite(&(elist->etoken), sizeof(elist->etoken), 1, file) != 1)
	werr = (werr == 0) ? 1 : werr;
      iter = elist->epart;
      while((iter != NULL) && (!werr)) {
	// mem epart
	ll = 0;
	while((!werr) && (ll < strlen(list_data(char, iter)) + 1))
	  if(fwrite(&((list_data(char, iter))[ll++]), sizeof(char), 1, file) != 1)
	    werr = 1;
	//
	iter = list_next(iter);
      }
      elist = list_next_entry(expr_t, elist);
    }
  }
  if(werr)
    warning("Some error occurs writing expression on file!");
  return werr;
}

/**
 * \brief How to retrieve an expression from file.
 *
 * It permits to retrieve a previously wrote down expression from file, checking
 * for integrity of data.
 *
 * \param file file to be used
 * \result loaded expression if no error occurs, zero otherwise
 */
expr_t*
expr_from_file (FILE* file)
{
  expr_t* elist;
  size_t ll;
  size_t dim;
  char* tbuf;
  char* bbuf;
  char buf[BUF_SIZE];
  char ch;
  int iter;
  int rerr;
  size_t pos;
  rerr = 0;
  elist = NULL;
  if(file != NULL) {
    // get token count
    if(fread(&ll, sizeof(ll), 1, file) != 1)
      rerr = (rerr == 0) ? 1 : rerr;
    while((!rerr) && (ll > 0)) {
      elist = (expr_t*) list_add((list_t*) expr_new(), (list_t*) elist);
      // get degree
      if(fread(&(elist->degree), sizeof(elist->degree), 1, file) != 1)
	rerr = (rerr == 0) ? 1 : rerr;
      // get vpart
      if(fread(&(elist->vpart), sizeof(elist->vpart), 1, file) != 1)
	rerr = (rerr == 0) ? 1 : rerr;
      // get etoken
      if(fread(&(elist->etoken), sizeof(elist->etoken), 1, file) != 1)
	rerr = (rerr == 0) ? 1 : rerr;
      iter = elist->etoken;
      while((!rerr) && (iter > 0)) {
	// get epart
	tbuf = NULL;
	bbuf = NULL;
	pos = 0;
	if(!rerr)
	  do {
	    // pos checking
	    if(pos == BUF_SIZE) {
	      if(bbuf == NULL) dim = 0;
	      else dim = strlen(bbuf);
	      pos += dim + 1;
	      tbuf = XMALLOC(char, pos);
	      for(pos = 0; pos < dim; ++pos)
		tbuf[pos] = bbuf[pos];
	      for(pos = 0; pos < BUF_SIZE; ++pos)
		tbuf[pos + dim] = buf[pos];
	      tbuf[pos + dim] = '\0';
	      XFREE(bbuf);
	      bbuf = tbuf;
	      tbuf = NULL;
	      pos = 0;
	    }
	    // / pos checking
	    if(fread(&ch, sizeof(char), 1, file) != 1)
	      rerr = (rerr == 0) ? 1 : rerr;
	    else buf[pos++] = ch;
	  } while((ch != '\0') && (!rerr));
	// mem buf
	if(!rerr) {
	  buf[pos] = '\0';
	  if(bbuf == NULL) dim = 0;
	  else dim = strlen(bbuf);
	  pos += dim + 1;
	  tbuf = XMALLOC(char, pos);
	  for(pos = 0; pos < dim; ++pos)
	    tbuf[pos] = bbuf[pos];
	  pos = -1;
	  while(buf[++pos] != '\0')
	    tbuf[pos + dim] = buf[pos];
	  tbuf[pos] = '\0';
	}
	 XFREE(bbuf);
	// ins epart
	elist->epart = list_add(list_new((void*) tbuf), elist->epart);
	--iter;
      }
      --ll;
    }
    if(rerr) {
      free_expr(elist);
      fatal("Error loading expression!");
    }
  }
  return elist;
}


/**
 * \brief Expression token maker
 *
 * This function allocates and returns a new expression token.
 *
 * \return newly allocated expression token
 */
expr_t*
expr_new ()
{
  expr_t* eslice;
  eslice = XMALLOC(expr_t, 1);
  eslice->next = NULL;
  eslice->vpart = 1;
  eslice->etoken = 0;
  eslice->degree = 0;
  eslice->epart = NULL;
  return eslice;
}

/**
 * \brief It frees an expressions %list
 *
 * Simply, it can be used to free an expressions %list.
 *
 * \param elist expressions %list reference
 */
void
free_expr (expr_t* elist)
{
  list_t* ltmp;
  void* data;
  while(elist != NULL) {
    while(elist->epart != NULL) {
      ltmp = elist->epart;
      elist->epart = list_next(ltmp);
      data = list_data(void, ltmp);
      XFREE(data);
      XFREE(ltmp);
    }
    ltmp = list_next_entry(list_t, elist);
    XFREE(elist);
    elist = (expr_t*) ltmp;
  }
}

/**
 * \brief Adds %forced edges into the expression
 *
 * \internal
 * This function is used by \e stack_to_expr to add %forced edges into the
 * expression via a simple way (really, simply a function invocation).
 *
 * \param fiter %forced edges %list
 * \param eslice expression %list token reference
 * \param giimat current tree's incidence matrix
 * \param gvimat voltage tree's incidence matrix
 * \param imrl incidence matrices' row length
 */
static void
forced_handler (const forced_t* fiter, expr_t* eslice, int* giimat, int* gvimat, const int imrl)
{
  int cnt;
  list_t** liter;
  cnt = 0;
  while(fiter != NULL) {
    giimat[imrl * fiter->gitoken[0] + cnt] = -1;
    giimat[imrl * fiter->gitoken[1] + cnt] = 1;
    gvimat[imrl * fiter->gvtoken[0] + cnt] = -1;
    gvimat[imrl * fiter->gvtoken[1] + cnt] = 1;
    if(fiter->data->sym == 1) {
      if(fiter->data->name) {
	// ordered insertion
	liter = &(eslice->epart);
	while((*liter != NULL) && (strcmp(list_data(const char, (*liter)), fiter->data->name) < 0))
	  liter = &((*liter)->next);
	*liter = list_add(list_new((void*) xstrdup(fiter->data->name)), *liter);
	// unordered insertion
        // eslice->epart = list_add(list_new((void*) xstrdup(fiter->data->name)), eslice->epart);
        ++(eslice->etoken);
        eslice->degree += fiter->data->degree;
      }
    } else eslice->vpart *= fiter->data->value;
    fiter = list_next_entry(forced_t, fiter);
    ++cnt;
  }
}

/**
 * \brief Gaussian elimination algorithm
 *
 * \internal
 * This is a support function used to reduce incident matrices. It is based on
 * the Gauss elimination algorithm that permits to write a given <i>m*n</i>
 * matrix M as a <i>n*n</i> diagonal matrix D. This function is an ad-hoc
 * function, that means it is designed to be more efficient (of course, it is
 * also less generic), like a specific per-problem function.
 *
 * \param matrix matrix to be reduced.
 * \param row number of rows (number of nodes into the tree)
 * \param col number of columns (number of edges of the tree, that is the number
 *     of nodes minus one)
 * \return determinant of the matrix
 */
static int
to_diagonal_matrix (int* matrix, const int row, const int col)
{
  int det;
  int iter;
  int cnt;
  int swap;
  int ofs;
  int weight;
  ofs = 0;
  det = 1;
  for(ofs = 0; ofs < col; ++ofs) {
    for(iter = ofs; iter < row; ++iter) {
      if(matrix[iter * col + ofs] != 0) {
	if(iter != ofs) {
	  for(cnt = ofs; cnt < col; ++cnt) {
	    swap = matrix[iter * col + cnt];
	    matrix[iter * col + cnt] = matrix[ofs * col + cnt];
	    matrix[ofs * col + cnt] = swap;
	  }
	  det *= -1;
	}
	for(iter = ofs + 1; iter < row; ++iter) {
	  if(matrix[iter * col + ofs] != 0) {
	    weight = -1 * matrix[ofs * col + ofs] / matrix[iter * col + ofs];
	    for(cnt = ofs; cnt < col; ++cnt)
	      matrix[iter * col + cnt] += matrix[ofs * col + cnt] * weight;
	  }
	}
	iter = row;
      }
    }
  }
  for(iter = 0; iter < col; ++iter)
    det *= matrix[iter * col + iter];
  return det;
}

/**
 * \deprecated obsolete function
 *
 * \sa stack_to_expr
 *
 * \brief Sort function for expression %list
 *
 * Sorting an expression %list with this function will results in a new
 * expression %list which tokens will be ordered by their degree.<br>
 * Actually, this function is never used.
 *
 * \param elist expression %list to be sorted
 * \result sorted expression %list
 */
expr_t*
expr_sort (expr_t* elist)
{
  expr_t* tmp;
  expr_t** iter;
  expr_t* olist;
  olist = NULL;
  while(elist != NULL) {
    iter = &olist;
    while((*iter != NULL) && ((*iter)->degree > elist->degree))
        iter = &((*iter)->next);
    tmp = *iter;
    *iter = elist;
    elist = list_next_entry(expr_t, elist);
    (*iter)->next = tmp;
  }
  return olist;
}

/**
 * \brief Adds an %edge to the current partial tree
 *
 * \internal
 * It is used to add an %edge to the current partial tree in a correct manner;
 * the added %edge isn't surely a valid %edge for that tree but only a potential
 * one.
 *
 * \param cc actual common components
 * \param nt tail node of the %edge
 * \param nh head node of the %edge
 * \param nnum number of nodes
 */
static void
ctrlplus (int* cc, const node_t nt, const node_t nh, const int nnum)
{
  int root;
  int iter;
  int tmp;
  int mem;
  root = cc[2 * nh];
  for(iter = 0; iter < nnum; ++iter)
    if(cc[2 * iter] == root)
      cc[2 * iter] = cc[2 * nt];
  iter = cc[2 * nh + 1];
  mem = nh;
  if(iter != -1)
    while(mem != root) {
      tmp = cc[2 * iter + 1];
      cc[2 * iter + 1] = mem;
      mem = iter;
      iter = tmp;
    }
  cc[2 * nh + 1] = nt;
}

/**
 * \brief Deletes an %edge from the current partial tree
 *
 * \internal
 * It is used to delete an %edge from the current partial tree in a correct
 * manner; the %edge must have been added by a previous call to \e ctrlplus.
 *
 * \param cc actual common components
 * \param nt tail node of the %edge
 * \param nh head node of the %edge
 * \param nnum number of nodes
 */
static void
ctrlminus (int* cc, node_t nt, node_t nh, const int nnum)
{
  int tmp;
  int iter;
  int flag = 1;
  if(cc[2 * nh + 1] != nt) {
    tmp = nt;
    nt = nh;
    nh = tmp;
  }
  cc[2 * nh + 1] = -1;
  cc[2 * nh] = nh;
  while(flag) {
    flag = 0;
    for(iter = 0; iter < nnum; ++iter) {
      if((iter != nh)&&(cc[2 * iter + 1] != -1)) {
        if((cc[ 2 * iter] != nh)&&(cc[2 * cc[2 * iter + 1]] == nh)) {
          cc[2 * iter] = nh;
          flag = 1;
        }
      }
    }
  }
}

/**
 * \brief Test for loop
 *
 * \internal
 * This test whether adding an %edge to the partial common tree results in a
 * loop or not; a loop situation is something to avoid.
 *
 * \param cc actual common components
 * \param nh head node of the %edge
 * \param nt tail node of the %edge
 * \result zero whether adding the %edge results in a loop, a positive value
 *   otherwise
 */
static int
testloop (const int* cc, const int nh, const int nt)
{
  return (cc[2*nh] == cc[2*nt]) ? 1 : 0;
}

/**
 * \brief List-of-nodes-to-expression-token converter
 *
 * \internal
 * This function adds a token to the expression and returns the head of a new
 * tight and sorted expression
 *
 */
static expr_t*
to_expr (const circ_t* crep, const node_t* nodes, int* mask, int maskmark, int* giimat, int* gvimat, expr_t* chain)
{
  int sdim;
  int iter;
  int actv;
  int offset;
  expr_t* eslice;
  expr_t** eiter;
  expr_t* elist;
  list_t** liter;
  list_t* lhook;
  void* hook;
  elist = chain;
  sdim = crep->nnum - 1 - crep->efnum - 1;
  for(iter = 0; iter < crep->ednum; ++iter)
    mask[iter] = 0;
  eslice = expr_new();
  for(iter = 0; iter < (crep->nnum * (crep->nnum - 1)); ++iter) {
    giimat[iter] = 0;
    gvimat[iter] = 0;
  }
  forced_handler(crep->flist, eslice, giimat, gvimat, (crep->nnum - 1));
  offset = crep->efnum;
  ++maskmark;
  for(iter = 0; iter < sdim; ++iter)
    mask[nodes[iter]] = maskmark;
  for(iter = 0; iter < crep->ednum; ++iter) {
    if(mask[iter] == maskmark) {
      giimat[(crep->nnum - 1) * crep->gi[2 * iter] + offset] = -1;
      giimat[(crep->nnum - 1) * crep->gi[2 * iter + 1] + offset] = 1;
      gvimat[(crep->nnum - 1) * crep->gv[2 * iter] + offset] = -1;
      gvimat[(crep->nnum - 1) * crep->gv[2 * iter + 1] + offset] = 1;
      ++offset;
    }
    if(((mask[iter] == maskmark) && (crep->edge[iter].type == Y)) ||	\
       ((mask[iter] != maskmark) && (crep->edge[iter].type == Z))) {
      if(crep->edge[iter].sym) {
	if(crep->edge[iter].name) {
	  // ordered insertion
	  liter = &(eslice->epart);
	  while((*liter != NULL) && (strcmp(list_data(const char, (*liter)), crep->edge[iter].name) < 0))
	    liter = &((*liter)->next);
	  *liter = list_add(list_new((void*) xstrdup(crep->edge[iter].name)), *liter);
	  // unordered insertion
	  // eslice->epart = list_add(list_new((void*) xstrdup(crep->edge[iter].name)), eslice->epart);
	  ++(eslice->etoken);
	}
	// sign-handler (generators' direction)
	// eslice->vpart *= crep->edge[iter].value;
      } else eslice->vpart *= crep->edge[iter].value;
      eslice->degree += crep->edge[iter].degree;
    }
  }
  // support edge reference
  if(crep->esupport != NULL) {
    giimat[(crep->nnum - 1) * crep->esupport->gitoken[0] + offset] = -1;
    giimat[(crep->nnum - 1) * crep->esupport->gitoken[1] + offset] = 1;
    gvimat[(crep->nnum - 1) * crep->esupport->gvtoken[0] + offset] = -1;
    gvimat[(crep->nnum - 1) * crep->esupport->gvtoken[1] + offset] = 1;
  }
  // sign computation
  eslice->vpart *= to_diagonal_matrix(giimat, crep->nnum, (crep->nnum - 1));
  eslice->vpart *= to_diagonal_matrix(gvimat, crep->nnum, (crep->nnum - 1));
  // shrink-step
  eiter = &elist;
  actv = 0;
  // find ...
  while((*eiter != NULL) && (eslice->degree <= (*eiter)->degree) && (!actv)) {
    if((eslice->degree == (*eiter)->degree) && (eslice->epart != NULL)) {
      lhook = eslice->epart;
      liter = &((*eiter)->epart);
      while(*liter != NULL) {
	if((lhook != NULL) && (strcmp(list_data(const char, lhook), list_data(const char, (*liter)))))
	  break;
	else {
	  lhook = list_next(lhook);
	  liter = &((*liter)->next);
	}
      }
      if((*liter == NULL) && (lhook == NULL))
	actv = !actv;
    }
    if(!actv)
      eiter = &((*eiter)->next);
  }
  // ... and shrink ...
  if(actv) {
    (*eiter)->vpart += eslice->vpart;
    while(eslice->epart != NULL) {
      lhook = eslice->epart;
      eslice->epart = list_next(lhook);
      hook = list_data(void, lhook);
      XFREE(hook);
      XFREE(lhook);
    }
    XFREE(eslice);
  } else
    // ... or insert, of course!
    *eiter = (expr_t*) list_add((list_t*) eslice, (list_t*) *eiter);
  return elist;
}

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
