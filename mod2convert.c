/* MOD2CONVERT.C - Routines converting between sparse and dense mod2 matrices.*/

/* Copyright (c) 1995-2012 by Radford M. Neal.
 *
 * Permission is granted for anyone to copy, use, modify, and distribute
 * these programs and accompanying documents for any purpose, provided
 * this copyright notice is retained and prominently displayed, and note
 * is made of any changes made to these programs.  These programs and
 * documents are distributed without any warranty, express or implied.
 * As the programs were written for research purposes only, they have not
 * been tested to the degree that would be advisable in any important
 * application.  All use of these programs is entirely at the user's own
 * risk.
 */


/* NOTE:  See mod2convert.html for documentation on these procedures. */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mod2dense.h"
#include "mod2sparse.h"
#include "mod2convert.h"


/* CONVERT A MOD2 MATRIX FROM SPARSE TO DENSE FORM.  */

void mod2sparse_to_dense 
( mod2sparse *m, 	/* Sparse matrix to convert */
  mod2dense *r		/* Place to store result */
)
{
  mod2entry *e;
  int i;

  if (mod2sparse_rows(m)>mod2dense_rows(r) 
   || mod2sparse_cols(m)>mod2dense_cols(r))
  { fprintf(stderr,
      "mod2sparse_to_dense: Dimension of result matrix is less than source\n");
    exit(1);
  }

  mod2dense_clear(r);

  for (i = 0; i<mod2sparse_rows(m); i++)
  { e = mod2sparse_first_in_row(m,i);
    while (!mod2sparse_at_end(e))
    { mod2dense_set(r,i,mod2sparse_col(e),1);
      e = mod2sparse_next_in_row(e);
    }
  }
}


/* CONVERT A MOD2 MATRIX FROM DENSE TO SPARSE FORM.  */

void mod2dense_to_sparse 
( mod2dense *m, 	/* Dense matrix to convert */
  mod2sparse *r		/* Place to store result */
)
{
  int i, j;

  if (mod2dense_rows(m)>mod2sparse_rows(r) 
   || mod2dense_cols(m)>mod2sparse_cols(r))
  { fprintf(stderr,
      "mod2dense_to_sparse: Dimension of result matrix is less than source\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (i = 0; i<mod2dense_rows(m); i++)
  { for (j = 0; j<mod2dense_cols(m); j++)
    { if (mod2dense_get(m,i,j))
      { mod2sparse_insert(r,i,j);
      }
    }
  }
}
