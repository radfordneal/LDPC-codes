/* MAKE-GEN.C - Make generator matrix from parity-check matrix. */

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "alloc.h"
#include "intio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"

typedef enum { Sparse, Dense, Mixed } make_method;      /* Ways of making it */

void make_dense_mixed (FILE *, make_method, char *);     /* Procs to make it */
void make_sparse (FILE *, mod2sparse_strategy, int, int);
void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  char *pchk_file, *gen_file, *other_gen_file;
  mod2sparse_strategy strategy;
  int abandon_when, abandon_number;
  make_method method;
  char *meth;
  char junk;
  FILE *f;

  /* Look at arguments. */

  if (!(pchk_file = argv[1])
   || !(gen_file = argv[2])
   || !(meth = argv[3]))
  { usage();
  }
  
  if (strcmp(meth,"sparse")==0)     
  { method = Sparse;
    strategy = Mod2sparse_minprod;
    abandon_number = 0;
    if (argv[4])
    { if (strcmp(argv[4],"first")==0)        strategy = Mod2sparse_first;
      else if (strcmp(argv[4],"mincol")==0)  strategy = Mod2sparse_mincol;
      else if (strcmp(argv[4],"minprod")==0) strategy = Mod2sparse_minprod;
      else 
      { usage();
      }
      if (argv[5])
      { if (sscanf(argv[5],"%d%c",&abandon_number,&junk)!=1 || abandon_number<=0
         || !argv[6] 
         || sscanf(argv[6],"%d%c",&abandon_when,&junk)!=1 || abandon_when<=0
         || argv[7])
        { usage();
        }
      }
    }
  }
  else if (strcmp(meth,"dense")==0) 
  { method = Dense;
    other_gen_file = argv[4];
    if (other_gen_file && argv[5])
    { usage();
    }
  }
  else if (strcmp(meth,"mixed")==0) 
  { method = Mixed;
    other_gen_file = argv[4];
    if (other_gen_file && argv[5])
    { usage();
    }
  }
  else 
  { usage();
  }

  /* Read parity check matrix. */

  read_pchk(pchk_file);

  if (N<=M)
  { fprintf(stderr,
     "Can't encode if number of bits (%d) isn't greater than number of checks (%d)\n",N,M);
    exit(1);
  }

  /* Create generator matrix file. */

  f = open_file_std(gen_file,"wb");
  if (f==NULL)
  { fprintf(stderr,"Can't create generator matrix file: %s\n",gen_file);
    exit(1);
  }

  /* Allocate space for row and column permutations. */

  cols = chk_alloc (N, sizeof *cols);
  rows = chk_alloc (M, sizeof *rows);

  /* Create generator matrix with specified method. */

  switch (method)
  { case Sparse: 
    { make_sparse(f,strategy,abandon_number,abandon_when); 
      break;
    }
    case Dense: case Mixed:
    { make_dense_mixed(f,method,other_gen_file);
      break;
    }
    default: abort();
  }

  /* Check for error writing file. */

  if (ferror(f) || fclose(f)!=0)
  { fprintf(stderr,"Error writing to generator matrix file\n");
    exit(1);
  }

  return 0;
}


/* MAKE DENSE OR MIXED REPRESENTATION OF GENERATOR MATRIX. */

void make_dense_mixed
( FILE *f,
  make_method method,
  char *other_gen_file
)
{ 
  mod2dense *DH, *A, *A2, *AI, *B;
  int i, j, c, c2, n;
  int *rows_inv;

  DH = mod2dense_allocate(M,N);
  AI = mod2dense_allocate(M,M);
  B  = mod2dense_allocate(M,N-M);
  G  = mod2dense_allocate(M,N-M);

  mod2sparse_to_dense(H,DH);

  /* If another generator matrix was specified, invert using the set of
     columns it specifies. */

  if (other_gen_file)
  { 
    read_gen(other_gen_file,1,0);

    A = mod2dense_allocate(M,M);
    mod2dense_copycols(DH,A,cols);

    if (!mod2dense_invert(A,AI))
    { fprintf(stderr,
       "Couldn't invert sub-matrix with column order given in other file\n");
      exit(1);
    }

    mod2dense_copycols(DH,B,cols+M);
  }

  /* If no other generator matrix was specified, invert using whatever 
     selection of rows/columns is needed to get a non-singular sub-matrix. */

  if (!other_gen_file)
  {
    A  = mod2dense_allocate(M,N);
    A2 = mod2dense_allocate(M,N);

    n = mod2dense_invert_selected(DH,A2,rows,cols);
    mod2sparse_to_dense(H,DH);  /* DH was destroyed by invert_selected */

    if (n>0)
    { fprintf(stderr,"Note: Parity check matrix has %d redundant checks\n",n);
    }

    rows_inv = chk_alloc (M, sizeof *rows_inv);

    for (i = 0; i<M; i++)
    { rows_inv[rows[i]] = i;
    }

    mod2dense_copyrows(A2,A,rows);
    mod2dense_copycols(A,A2,cols);
    mod2dense_copycols(A2,AI,rows_inv);

    mod2dense_copycols(DH,B,cols+M);
  }

  /* Form final generator matrix. */

  if (method==Dense) 
  { mod2dense_multiply(AI,B,G);
  }
  else if (method==Mixed)
  { G = AI;
  }
  else
  { abort();
  }

  /* Compute and print number of 1s. */

  if (method==Dense)  
  { c = 0;
    for (i = 0; i<M; i++)
    { for (j = 0; j<N-M; j++)
      { c += mod2dense_get(G,i,j);
      }
    }
    fprintf(stderr,
      "Number of 1s per check in Inv(A) X B is %.1f\n", (double)c/M);
  }

  if (method==Mixed)
  { c = 0;
    for (i = 0; i<M; i++)
    { for (j = 0; j<M; j++)
      { c += mod2dense_get(G,i,j);
      }
    }
    c2 = 0;
    for (i = M; i<N; i++) 
    { c2 += mod2sparse_count_col(H,cols[i]);
    }
    fprintf(stderr,
     "Number of 1s per check in Inv(A) is %.1f, in B is %.1f, total is %.1f\n",
     (double)c/M, (double)c2/M, (double)(c+c2)/M);
  }

  /* Write the represention of the generator matrix to the file. */

  intio_write(f,('G'<<8)+0x80);

  if (method==Dense)      
  { fwrite ("d", 1, 1, f);
  }
  if (method==Mixed) 
  { fwrite ("m", 1, 1, f);
  }

  intio_write(f,M);
  intio_write(f,N);

  for (i = 0; i<N; i++) 
  { intio_write(f,cols[i]);
  }

  mod2dense_write (f, G);
}


/* MAKE SPARSE REPRESENTATION OF GENERATOR MATRIX. */

void make_sparse
( FILE *f,
  mod2sparse_strategy strategy,
  int abandon_number,
  int abandon_when
)
{
  int n, cL, cU, cB;
  int i;

  /* Find LU decomposition. */

  L = mod2sparse_allocate(M,M);
  U = mod2sparse_allocate(M,N);

  n = mod2sparse_decomp(H,M,L,U,rows,cols,strategy,abandon_number,abandon_when);

  if (n!=0 && abandon_number==0)
  { fprintf(stderr,"Note: Parity check matrix has %d redundant checks\n",n);
  }
  if (n!=0 && abandon_number>0)
  { fprintf(stderr,
  "Note: Have %d dependent columns, but this could be due to abandonment.\n",n);
    fprintf(stderr,
  "      Try again with lower abandonment number.\n");
    exit(1);
  }

  /* Compute and print number of 1s. */

  cL = cU = cB = 0;

  for (i = 0; i<M; i++) cL += mod2sparse_count_row(L,i);
  for (i = 0; i<M; i++) cU += mod2sparse_count_row(U,i);
  for (i = M; i<N; i++) cB += mod2sparse_count_col(H,cols[i]);

  fprintf(stderr,
   "Number of 1s per check in L is %.1f, U is %.1f, B is %.1f, total is %.1f\n",
    (double)cU/M, (double)cL/M, (double)cB/M, (double)(cL+cU+cB)/M);

  /* Write it all to the generator matrix file. */

  intio_write(f,('G'<<8)+0x80);

  fwrite ("s", 1, 1, f);

  intio_write(f,M);
  intio_write(f,N);

  for (i = 0; i<N; i++) 
  { intio_write(f,cols[i]);
  }

  for (i = 0; i<M; i++) 
  { intio_write(f,rows[i]);
  }

  mod2sparse_write (f, L);
  mod2sparse_write (f, U);
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf (stderr, 
   "Usage:  make-gen pchk-file gen-file method\n");
  fprintf (stderr, 
   "Method: sparse [ \"first\" | \"mincol\" | \"minprod\" ] [ abandon_num abandon_when ]\n");
  fprintf (stderr, 
   "    or: dense [ other-gen-file ]\n");
  fprintf (stderr, 
   "    or: mixed [ other-gen-file ]\n");
  exit(1);
}
