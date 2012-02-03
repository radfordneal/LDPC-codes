/* MOD2SPARSE-TEST. C - Program to test mod2sparse module. */

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


/* Correct output for this program is saved in the file mod2sparse-test-out */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mod2sparse.h"


main(void)
{
  mod2sparse *m1, *m2, *m3, *m4;
  mod2sparse *s0, *s1, *s2, *s3, *s4;
  mod2sparse *L, *U;
  mod2entry *e;
  int rows[5], cols[7];
  int i, j;
  FILE *f;


  printf("\nPART 1:\n\n");

  /* Set up m1 with bits on a diagonal plus a few more set to 1. */

  m1 = mod2sparse_allocate(35,40);

  mod2sparse_clear(m1);

  for (i = 0; i<35; i++) mod2sparse_insert(m1,i,i);

  mod2sparse_insert(m1,2,3);
  mod2sparse_insert(m1,34,4);
  mod2sparse_insert(m1,10,38);

  /* Print m1. */

  printf("Matrix m1:\n\n");
  mod2sparse_print(stdout,m1);
  printf("\n"); fflush(stdout);

  /* Store m1 in a file. */

  f = fopen("test-file","wb");
  if (f==0)
  { fprintf(stderr,"Can't create test-file\n");
    exit(1);
  }

  if (!mod2sparse_write(f,m1))
  { printf("Error from mod2sparse_write\n");
  }

  fclose(f);

  /* Read matrix written above back into m2. */

  f = fopen("test-file","rb");
  if (f==0)
  { fprintf(stderr,"Can't open test-file\n");
    exit(1);
  }

  m2 = mod2sparse_read(f);

  if (m2==0)
  { printf("Error from mod2sparse_read\n");
    exit(1);
  }

  /* Print m2, along with result of equality test. */

  printf("Matrix m2, as read from file.  Should be same as m1 above.\n\n");
  mod2sparse_print(stdout,m2);
  printf("\n"); fflush(stdout);

  printf("Test of equality of m1 & m2 (should be 1): %d\n\n",
    mod2sparse_equal(m1,m2));

  /* Copy m1 to m3. */

  m3 = mod2sparse_allocate(mod2sparse_rows(m1),mod2sparse_cols(m1));

  mod2sparse_copy(m1,m3);

  /* Print m3, along with result of equality test. */

  printf("Matrix m3, copied from m1 above.\n\n");
  mod2sparse_print(stdout,m3);
  printf("\n"); fflush(stdout);

  printf("Test of equality of m1 & m3 (should be 1): %d\n\n",
    mod2sparse_equal(m1,m3));

  /* Clear m3. */

  mod2sparse_clear(m3);

  /* Print m3 again. */

  printf("Matrix m3 again, should now be all zeros.\n\n");
  mod2sparse_print(stdout,m3);
  printf("\n"); fflush(stdout);

  printf("Test of equality of m1 & m3 (should be 0): %d\n\n",
    mod2sparse_equal(m1,m3));


  printf("\nPART 2:\n\n");

  /* Compute transpose of m1. */

  m4 = mod2sparse_allocate(mod2sparse_cols(m1),mod2sparse_rows(m1));

  mod2sparse_transpose(m1,m4);

  /* Print transpose. */

  printf("Transpose of m1.\n\n");
  mod2sparse_print(stdout,m4);
  printf("\n"); fflush(stdout);

  /* Add rows and columns in m1. */

  mod2sparse_add_row(m1,10,m1,2); 
  mod2sparse_add_row(m1,10,m1,12);
  mod2sparse_add_row(m1,10,m1,3); 
  printf("Matrix m1 after adding rows 2 and 12 and 3 to 10.\n\n");
  mod2sparse_print(stdout,m1);
  printf("\n"); fflush(stdout);
  printf("Matrix m1 after further adding column 34 to 0.\n\n");
  mod2sparse_add_col(m1,0,m1,34);
  mod2sparse_print(stdout,m1);
  printf("\n"); fflush(stdout);

  /* Free space for m1, m2, and m3. */

  mod2sparse_free(m1);
  mod2sparse_free(m2);
  mod2sparse_free(m3);


  printf("\nPART 3:\n\n");
  
  /* Allocate some small matrices. */

  s0 = mod2sparse_allocate(5,7);
  s1 = mod2sparse_allocate(5,7);
  s2 = mod2sparse_allocate(7,4);
  s3 = mod2sparse_allocate(5,4);
  s4 = mod2sparse_allocate(5,7);

  /* Set up the contents of s0, s1, and s2. */

  mod2sparse_clear(s0);
  mod2sparse_clear(s1);
  mod2sparse_clear(s2);

  mod2sparse_insert(s0,1,3);
  mod2sparse_insert(s0,1,4);
  mod2sparse_insert(s0,2,0);
  mod2sparse_insert(s0,3,1);

  mod2sparse_insert(s1,1,3);
  mod2sparse_insert(s1,1,5);
  mod2sparse_insert(s1,3,0);
  mod2sparse_insert(s1,3,1);
  mod2sparse_insert(s1,3,6);

  mod2sparse_insert(s2,5,1);
  mod2sparse_insert(s2,5,2);
  mod2sparse_insert(s2,5,3);
  mod2sparse_insert(s2,0,0);
  mod2sparse_insert(s2,1,1);

  /* Print s0, s1, and s2. */

  printf("Matrix s0.\n\n");
  mod2sparse_print(stdout,s0);
  printf("\nMatrix s1.\n\n");
  mod2sparse_print(stdout,s1);
  printf("\nMatrix s2.\n\n");
  mod2sparse_print(stdout,s2);
  printf("\n"); fflush(stdout);

  /* Multiply s1 by vector (1 1 0 1 0 1 0). */

  { char u[7] = { 1, 0, 0, 1, 0, 1, 0 };
    char v[5];
    int i;
    printf("Maxtrix s1 times unpacked vector ( 1 0 0 1 0 1 0 ).\n\n(");
    mod2sparse_mulvec(s1,u,v);
    for (i = 0; i<5; i++) printf(" %d",v[i]);
    printf(" )\n\n");
  }
   
  /* Add s0 and s1, storing the result in s4, then print s4. */

  mod2sparse_add(s0,s1,s4);

  printf("Sum of s0 and s1.\n\n");
  mod2sparse_print(stdout,s4);
  printf("\n"); fflush(stdout);

  /* Multiply s1 and s2, storing the product in s3, and then print s3. */

  mod2sparse_multiply(s1,s2,s3);

  printf("Product of s1 and s2.\n\n");
  mod2sparse_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Try clearing a bit in s3, then printing the result. */

  e = mod2sparse_find(s3,1,2);
  printf("Tried to find (1,2), actually found: (%d,%d)\n\n",
    mod2sparse_row(e), mod2sparse_col(e));

  mod2sparse_delete(s3,e);
  
  printf("Above matrix with (1,2) cleared.\n\n");
  mod2sparse_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Try clearing another bit in s3, then printing the result. */

  e = mod2sparse_find(s3,1,1);
  printf("Tried to find (1,1), actually found: (%d,%d)\n\n",
    mod2sparse_row(e), mod2sparse_col(e));

  mod2sparse_delete(s3,e);
  
  printf("Matrix with (1,1) cleared as well.\n\n");
  mod2sparse_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Free space for s0, s1, s2, s3, and s4. */

  mod2sparse_free(s0);
  mod2sparse_free(s1);
  mod2sparse_free(s2);
  mod2sparse_free(s3);
  mod2sparse_free(s4);


  printf("\nPART 4:\n\n");

  /* Set up a small rectangular matrix, s1. */

  s1 = mod2sparse_allocate(6,7);

  mod2sparse_clear(s1);

  mod2sparse_insert(s1,0,3);
  mod2sparse_insert(s1,0,5);
  mod2sparse_insert(s1,1,6);
  mod2sparse_insert(s1,1,1);
  mod2sparse_insert(s1,2,0);
  mod2sparse_insert(s1,3,1);
  mod2sparse_insert(s1,3,2);
  mod2sparse_insert(s1,4,2);
  mod2sparse_insert(s1,4,0);
  mod2sparse_insert(s1,5,6);

  /* Print s1. */

  printf("Matrix s1.\n\n");
  mod2sparse_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Compute and print LU decomposition. */

  L = mod2sparse_allocate(6,5);
  U = mod2sparse_allocate(5,7);

  i = mod2sparse_decomp(s1,5,L,U,rows,cols,Mod2sparse_first,0,0);

  printf("LU decomposition (returned value was %d).\n\n",i);
  printf("L=\n");
  mod2sparse_print(stdout,L);
  printf("\nU=\n");
  mod2sparse_print(stdout,U);
  printf("\n");

  printf("cols:");
  for (j = 0; j<7; j++) printf(" %d",cols[j]);
  printf("\n");
  printf("rows:");
  for (i = 0; i<6; i++) printf(" %d",rows[i]);
  printf("\n\n");
  fflush(stdout);

  /* Compute and print product of L and U. Should match s1 for the
     sub-matrix found. */

  s2 = mod2sparse_allocate(6,7);
  mod2sparse_multiply(L,U,s2);

  printf("Product of L and U.\n\n");
  mod2sparse_print(stdout,s2);
  printf("\n");
  fflush(stdout);

  /* Solve system by forward and backward substitution. */

  { char x[6] = { 0, 1, 1, 0, 1, 0 };
    static char y[5], z[7];
    int i, r;

    r = mod2sparse_forward_sub (L, rows, x, y);
    printf(
"Solution of Ly=x with x from ( 0 1 1 0 1 0 ) according to rows selected.\n\n");
    for (i = 0; i<5; i++) printf(" %d",y[i]);
    printf("\n\nReturned value from forward_sub was %d\n\n",r);
    fflush(stdout);

    r = mod2sparse_backward_sub (U, cols, y, z);
    printf("Solution of Uz=y.\n\n");
    for (i = 0; i<7; i++) printf(" %d",z[i]);
    printf("\n\nReturned value from backward_sub was %d\n\n",r);
    fflush(stdout);
  }

  printf("\nPART 5:\n\n");

  m1 = mod2sparse_allocate(4,4);
  m2 = mod2sparse_allocate(4,4);
  m3 = mod2sparse_allocate(4,4);

  mod2sparse_insert(m1,0,3);
  mod2sparse_insert(m1,1,1);
  mod2sparse_insert(m1,2,2);
  mod2sparse_insert(m1,3,0);

  printf("Matrix m1:\n\n");

  mod2sparse_print(stdout,m1);
  printf("\n"); fflush(stdout);

  printf("Matrix m2, copyrows of m1 in order 3,1,2,0 (should be identity)\n\n");

  { int rows[] = { 3, 1, 2, 0 };
    mod2sparse_copyrows(m1,m2,rows);
  }

  mod2sparse_print(stdout,m2);
  printf("\n"); fflush(stdout);

  printf("Matrix m3, copycols of m1 in order 3,1,2,0 (should be identity)\n\n");

  { int cols[] = { 3, 1, 2, 0 };
    mod2sparse_copycols(m1,m3,cols);
  }

  mod2sparse_print(stdout,m3);
  printf("\n"); fflush(stdout);


  printf("\nDONE WITH TESTS.\n");

  exit(0);
}
