/* MOD2CONVERT-TEST. C - Program to test mod2convert module. */

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


/* Correct output for this program is saved in the file mod2convert-test-out */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mod2dense.h"
#include "mod2sparse.h"
#include "mod2convert.h"
#include "rand.h"

#define Rows 40		/* Dimensions of matrix to use in test */
#define Cols 13

#define N 100		/* Number of bits to set in test matrix (some may be
                           duplicates, leading to fewer 1's in matrix */

main(void)
{
  mod2sparse *sm1, *sm2;
  mod2dense *dm1, *dm2;
  int i;

  sm1 = mod2sparse_allocate(Rows,Cols);
  sm2 = mod2sparse_allocate(Rows,Cols);

  dm1 = mod2dense_allocate(Rows,Cols);
  dm2 = mod2dense_allocate(Rows,Cols);

  printf("\nCreating sparse matrix.\n"); 
  fflush(stdout);

  for (i = 0; i<N; i++)
  { mod2sparse_insert(sm1,rand_int(Rows),rand_int(Cols));
  }

  printf("Converting from sparse to dense.\n");
  fflush(stdout);

  mod2sparse_to_dense(sm1,dm1);

  printf("Converting back to dense again.\n");
  fflush(stdout);

  mod2dense_to_sparse(dm1,sm2);

  printf("Testing for equality of two sparse matrices: %s.\n",
    mod2sparse_equal(sm1,sm2) ? "OK" : "NOT OK");
  fflush(stdout);

  printf("Converting to dense once again.\n");
  fflush(stdout);
 
  mod2sparse_to_dense(sm2,dm2);

  printf("Testing for equality of two dense matrices: %s.\n",
    mod2dense_equal(dm1,dm2) ? "OK" : "NOT OK");
  fflush(stdout);

  printf("\nDONE WITH TESTS.\n");

  exit(0);
}
