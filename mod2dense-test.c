/* MOD2DENSE-TEST. C - Program to test mod2dense module. */

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


/* Correct output for this program is saved in the file mod2dense-test-out */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mod2dense.h"


main(void)
{
  mod2dense *m1, *m2, *m3, *m4;
  mod2dense *s0, *s1, *s2, *s3, *s4, *s5;
  int a_row[35], a_col[35];
  int code;
  int i, j;
  FILE *f;


  printf("\nPART 1:\n\n");

  /* Set up m1 with bits on a diagonal plus a few more set to 1. */

  m1 = mod2dense_allocate(35,40);

  mod2dense_clear(m1);

  for (i = 0; i<35; i++) mod2dense_set(m1,i,i,1);

  mod2dense_set(m1,2,3,1);
  mod2dense_set(m1,34,4,1);
  mod2dense_set(m1,10,38,1);

  /* Print m1. */

  printf("Matrix m1:\n\n");
  mod2dense_print(stdout,m1);
  printf("\n"); fflush(stdout);

  /* Store m1 in a file. */

  f = fopen("test-file","wb");
  if (f==0)
  { fprintf(stderr,"Can't create test-file\n");
    exit(1);
  }

  if (!mod2dense_write(f,m1))
  { printf("Error from mod2dense_write\n");
  }

  fclose(f);

  /* Read matrix written above back into m2. */

  f = fopen("test-file","rb");
  if (f==0)
  { fprintf(stderr,"Can't open test-file\n");
    exit(1);
  }

  m2 = mod2dense_read(f);

  if (m2==0)
  { printf("Error from mod2dense_read\n");
    exit(1);
  }

  /* Print m2, along with result of equality test. */

  printf("Matrix m2, as read from file.  Should be same as m1 above.\n\n");
  mod2dense_print(stdout,m2);
  printf("\n"); fflush(stdout);

  printf("Test of equality of m1 & m2 (should be 1): %d\n\n",
    mod2dense_equal(m1,m2));

  /* Copy m1 to m3. */

  m3 = mod2dense_allocate(mod2dense_rows(m1),mod2dense_cols(m1));

  mod2dense_copy(m1,m3);

  /* Print m3, along with result of equality test. */

  printf("Matrix m3, copied from m1 above.\n\n");
  mod2dense_print(stdout,m3);
  printf("\n"); fflush(stdout);

  printf("Test of equality of m1 & m3 (should be 1): %d\n\n",
    mod2dense_equal(m1,m3));

  /* Clear m3. */

  mod2dense_clear(m3);

  /* Print m3 again. */

  printf("Matrix m3 again, should now be all zeros.\n\n");
  mod2dense_print(stdout,m3);
  printf("\n"); fflush(stdout);

  printf("Test of equality of m1 & m3 (should be 0): %d\n\n",
    mod2dense_equal(m1,m3));


  printf("\nPART 2:\n\n");

  /* Compute transpose of m1. */

  m4 = mod2dense_allocate(mod2dense_cols(m1),mod2dense_rows(m1));

  mod2dense_transpose(m1,m4);

  /* Print transpose. */

  printf("Transpose of m1.\n\n");
  mod2dense_print(stdout,m4);
  printf("\n"); fflush(stdout);

  /* Free space for m1, m2, and m3. */

  mod2dense_free(m1);
  mod2dense_free(m2);
  mod2dense_free(m3);


  printf("\nPART 3:\n\n");
  
  /* Allocate some small matrices. */

  s0 = mod2dense_allocate(5,7);
  s1 = mod2dense_allocate(5,7);
  s2 = mod2dense_allocate(7,4);
  s3 = mod2dense_allocate(5,4);
  s4 = mod2dense_allocate(5,7);

  /* Set up the contents of s0, s1, and s2. */

  mod2dense_clear(s0);
  mod2dense_clear(s1);
  mod2dense_clear(s2);

  mod2dense_set(s0,1,3,1);
  mod2dense_set(s0,1,4,1);
  mod2dense_set(s0,2,0,1);
  mod2dense_set(s0,3,1,1);

  mod2dense_set(s1,1,3,1);
  mod2dense_set(s1,1,5,1);
  mod2dense_set(s1,3,0,1);
  mod2dense_set(s1,3,1,1);
  mod2dense_set(s1,3,6,1);

  mod2dense_set(s2,5,1,1);
  mod2dense_set(s2,5,2,1);
  mod2dense_set(s2,5,3,1);
  mod2dense_set(s2,0,0,1);
  mod2dense_set(s2,1,1,1);

  /* Print s0, s1, and s2. */

  printf("Matrix s0.\n\n");
  mod2dense_print(stdout,s0);
  printf("\nMatrix s1.\n\n");
  mod2dense_print(stdout,s1);
  printf("\nMatrix s2.\n\n");
  mod2dense_print(stdout,s2);
  printf("\n"); fflush(stdout);

  /* Add s0 and s1, storing the result in s4, then print s4. */

  mod2dense_add(s0,s1,s4);

  printf("Sum of s0 and s1.\n\n");
  mod2dense_print(stdout,s4);
  printf("\n"); fflush(stdout);

  /* Multiply s1 and s2, storing the product in s3, and then print s3. */

  mod2dense_multiply(s1,s2,s3);

  printf("Product of s1 and s2.\n\n");
  mod2dense_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Try clearing a bit in s3, then printing the result. */

  mod2dense_set(s3,1,2,0);
  
  printf("Above matrix with (1,2) cleared.\n\n");
  mod2dense_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Free space for s0, s1, s2, s3, and s4. */

  mod2dense_free(s0);
  mod2dense_free(s1);
  mod2dense_free(s2);
  mod2dense_free(s3);
  mod2dense_free(s4);


  printf("\nPART 4:\n\n");

  /* Set up a small square matrix, s1.  Also copy it to s2. */

  s1 = mod2dense_allocate(5,5);
  s2 = mod2dense_allocate(5,5);

  mod2dense_clear(s1);

  mod2dense_set(s1,0,3,1);
  mod2dense_set(s1,1,4,1);
  mod2dense_set(s1,1,1,1);
  mod2dense_set(s1,2,0,1);
  mod2dense_set(s1,3,1,1);
  mod2dense_set(s1,3,2,1);
  mod2dense_set(s1,4,2,1);
  mod2dense_set(s1,4,0,1);

  mod2dense_copy(s1,s2);

  /* Print s1. */

  printf("Matrix s1.\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Compute inverse of s1, storing it in s3. */

  s3 = mod2dense_allocate(5,5);
  
  code = mod2dense_invert(s1,s3);

  /* Print inverse (s3). */

  printf("Matrix s3, the inverse of s1 (return code %d).\n\n",code);
  mod2dense_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Compute and print product of inverse and original matrix, both ways. */

  mod2dense_multiply(s2,s3,s1);
  printf("Original matrix times inverse (should be identity).\n\n");
  mod2dense_print(stdout,s1);

  mod2dense_multiply(s3,s2,s1);
  printf("\nInverse times original matrix (should be identity).\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Compute and print inverse of inverse, and do equality check. */

  mod2dense_invert(s3,s1);
  
  printf("Inverse of inverse (should be same as original s1).\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  printf("Test of equality with original (should be 1): %d\n\n",
    mod2dense_equal(s1,s2));

  /* Free s1, s2, and s3. */

  mod2dense_free(s1);
  mod2dense_free(s2);
  mod2dense_free(s3);

  /* Set up a rectangular matrix like s1 above, but with two zero columns. 
     Copy to s4 as well. */

  s1 = mod2dense_allocate(5,7);

  mod2dense_clear(s1);

  mod2dense_set(s1,0,4,1);
  mod2dense_set(s1,1,6,1);
  mod2dense_set(s1,1,1,1);
  mod2dense_set(s1,2,0,1);
  mod2dense_set(s1,3,1,1);
  mod2dense_set(s1,3,2,1);
  mod2dense_set(s1,4,2,1);
  mod2dense_set(s1,4,0,1);

  s4 = mod2dense_allocate(5,7);
  mod2dense_copy(s1,s4);

  /* Print s1. */

  printf("Matrix s1.\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Compute inverse of sub-matrix of s1, storing it in s3.  Print results. */

  s3 = mod2dense_allocate(5,7);
  
  code = mod2dense_invert_selected(s1,s3,a_row,a_col);

  printf("Matrix s3, from invert_selected applied to s1 (return code %d).\n\n",
          code);

  mod2dense_print(stdout,s3);

  printf("\n row ordering returned:");
  for (i = 0; i<5; i++) printf(" %d",a_row[i]);
  printf("\n");

  printf("\n column ordering returned:");
  for (j = 0; j<7; j++) printf(" %d",a_col[j]);
  printf("\n");

  printf("\n"); fflush(stdout);

  printf("Columns extracted in order from original matrix.\n\n");

  s2 = mod2dense_allocate(5,5);
  mod2dense_copycols(s4,s2,a_col);
  mod2dense_print(stdout,s2);
  printf("\n"); fflush(stdout);

  s5 = mod2dense_allocate(5,5);
  code = mod2dense_invert(s2,s5);
  
  printf(
   "Inverse of above calculated using mod2dense_inverse (return code %d)\n\n",
   code);

  mod2dense_print(stdout,s5);
  printf("\n"); fflush(stdout);

  printf(
   "Columns extracted in order from s3 (should also be inverse of above).\n\n");
  mod2dense_copycols(s3,s2,a_col);
  mod2dense_print(stdout,s2);
  printf("\n"); fflush(stdout);

  /* Try out mod2dense_invert_selected again. */

  mod2dense_clear(s1);

  mod2dense_set(s1,0,0,1);
  mod2dense_set(s1,0,1,1);
  mod2dense_set(s1,1,1,1);
  mod2dense_set(s1,1,2,1);
  mod2dense_set(s1,2,0,1);
  mod2dense_set(s1,2,2,1);
  mod2dense_set(s1,3,3,1);
  mod2dense_set(s1,3,4,1);

  printf("Matrix s1.\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  code = mod2dense_invert_selected(s1,s3,a_row,a_col);

  printf("Matrix s3, from invert_selected applied to s1 (return code %d).\n\n",
          code);

  mod2dense_print(stdout,s3);

  printf("\n row ordering returned:");
  for (i = 0; i<5; i++) printf(" %d",a_row[i]);
  printf("\n");
  printf("\n column ordering returned:");
  for (j = 0; j<7; j++) printf(" %d",a_col[j]);
  printf("\n");

  printf("\n"); fflush(stdout);

  printf("Matrix s4, from copying rows in order from s3.\n\n");

  mod2dense_copyrows(s3,s4,a_row);

  mod2dense_print(stdout,s4);

  free(s1);
  free(s2);
  free(s3);
  free(s4);
  free(s5);


  printf("\nPART 5:\n\n");

  /* Set up a larger square matrix, s1.  Also copy it to s2. */

  s1 = mod2dense_allocate(35,35);
  s2 = mod2dense_allocate(35,35);

  mod2dense_clear(s1);

  for (i = 0; i<35; i++) mod2dense_set(s1,i,i,1);

  mod2dense_set(s1,10,3,1);
  mod2dense_set(s1,11,4,1);
  mod2dense_set(s1,11,11,1);
  mod2dense_set(s1,12,20,1);
  mod2dense_set(s1,13,31,1);
  mod2dense_set(s1,23,12,1);
  mod2dense_set(s1,24,12,1);
  mod2dense_set(s1,14,10,1);
  mod2dense_set(s1,2,20,1);
  mod2dense_set(s1,3,31,1);
  mod2dense_set(s1,3,12,1);
  mod2dense_set(s1,24,2,1);
  mod2dense_set(s1,24,0,1);
  mod2dense_set(s1,5,3,1);
  mod2dense_set(s1,18,3,1);
  mod2dense_set(s1,17,11,1);
  mod2dense_set(s1,32,23,1);
  mod2dense_set(s1,9,24,1);
  mod2dense_set(s1,19,11,1);
  mod2dense_set(s1,11,30,1);
  mod2dense_set(s1,21,27,1);
  mod2dense_set(s1,21,22,1);
  mod2dense_set(s1,23,33,1);
  mod2dense_set(s1,24,23,1);
  mod2dense_set(s1,24,25,1);
  mod2dense_set(s1,30,34,1);
  mod2dense_set(s1,31,10,1);
  mod2dense_set(s1,33,17,1);
  mod2dense_set(s1,33,18,1);
  mod2dense_set(s1,34,8,1);
  mod2dense_set(s1,34,11,1);
  mod2dense_set(s1,34,3,1);
  mod2dense_set(s1,34,24,1);
  mod2dense_set(s1,25,34,1);
  mod2dense_set(s1,13,34,1);

  mod2dense_set(s1,3,3,0);
  mod2dense_set(s1,11,11,0);
  mod2dense_set(s1,23,23,0);
  mod2dense_set(s1,24,24,0);

  mod2dense_copy(s1,s2);

  /* Print s1. */

  printf("Matrix s1.\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Compute inverse of s1, storing it in s3. */

  s3 = mod2dense_allocate(35,35);
  
  code = mod2dense_invert(s1,s3);

  /* Print inverse (s3). */

  printf("Matrix s3, the inverse of s1 (return code %d).\n\n",code);
  mod2dense_print(stdout,s3);
  printf("\n"); fflush(stdout);

  /* Compute and print product of inverse and original matrix, both ways. */

  mod2dense_multiply(s2,s3,s1);
  printf("Original matrix times inverse (should be identity).\n\n");
  mod2dense_print(stdout,s1);

  mod2dense_multiply(s3,s2,s1);
  printf("\nInverse times original matrix (should be identity).\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Compute and print inverse of inverse, and do equality check. */

  mod2dense_invert(s3,s1);
  
  printf("Inverse of inverse (should be same as original s1).\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  printf("Test of equality with original (should be 1): %d\n\n",
    mod2dense_equal(s1,s2));

  /* Free s1, s2, and s3. */

  mod2dense_free(s1);
  mod2dense_free(s2);
  mod2dense_free(s3);


  printf("\nPART 6:\n\n");

  /* Set up a largish square matrix, s1.  Also copy it to s2. */

  s1 = mod2dense_allocate(35,35);
  s2 = mod2dense_allocate(35,35);

  mod2dense_clear(s1);

  for (i = 0; i<10; i++)  
  { if (i!=3 && i!=7) 
    { mod2dense_set(s1,i,i,1);
    }
  }
  for (i = 10; i<35; i++) 
  { if (i!=15 && i!=21 && i!=32)
    { mod2dense_set(s1,i,34-(i-10),1);
    }
  }

  /* Print s1. */

  printf("Matrix s1.\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);

  /* Forcibly invert s1, storing inverse in s3. */

  s3 = mod2dense_allocate(35,35);
  
  code = mod2dense_forcibly_invert(s1,s3,a_row,a_col);

  /* Print inverse, and list of altered elements. */

  printf("Result of forcibly inverting s1 (needed to alter %d elements).\n\n",
    code);
  mod2dense_print(stdout,s3);
  printf("\n"); fflush(stdout);

  printf("Altered elements at these indexes:\n\n");

  for (i = 0; i<code; i++)
  { printf("%3d %3d\n",a_row[i],a_col[i]);
  }
  printf("\n"); fflush(stdout);

  /* Compute and print inverse of inverse. */

  mod2dense_invert(s3,s1);
  
  printf("Inverse of inverse of altered matrix.\n\n");
  mod2dense_print(stdout,s1);
  printf("\n"); fflush(stdout);
 
  printf("\nDONE WITH TESTS.\n");

  exit(0);
}
