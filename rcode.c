/* RCODE.C - Procedures to read parity check and generator matrices. */

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

#include "alloc.h"
#include "intio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"


/* VARIABLES DECLARED IN RCODE.H.  These global variables are set to
   representations of the parity check and generator matrices by read_pchk
   and read_gen. */

mod2sparse *H;		/* Parity check matrix */

int M;			/* Number of rows in parity check matrix */
int N;			/* Number of columns in parity check matrix */

char type;		/* Type of generator matrix representation (s/d/m) */
int *cols;		/* Ordering of columns in generator matrix */

mod2sparse *L, *U;	/* Sparse LU decomposition, if type=='s' */
int *rows;		/* Ordering of rows in generator matrix (type 's') */

mod2dense *G;		/* Dense or mixed representation of generator matrix,
			   if type=='d' or type=='m' */


/* READ PARITY CHECK MATRIX.  Sets the H, M, and N global variables.  If an
   error is encountered, a message is displayed on standard error, and the
   program is terminated. */

void read_pchk
( char *pchk_file
)
{
  FILE *f;

  f = open_file_std(pchk_file,"rb");
  if (f==NULL)
  { fprintf(stderr,"Can't open parity check file: %s\n",pchk_file);
    exit(1);
  }

  if (intio_read(f)!=('P'<<8)+0x80)
  { fprintf(stderr,"File %s doesn't contain a parity check matrix\n",pchk_file);
    exit(1);
  }

  H = mod2sparse_read(f);

  if (H==0)
  { fprintf(stderr,"Error reading parity check matrix from %s\n",pchk_file);
    exit(1);
  }

  M = mod2sparse_rows(H);
  N = mod2sparse_cols(H);

  fclose(f);
}


/* READ GENERATOR MATRIX.  The parity check matrix must have already been 
   read, unless the last argument is set to 1.  The generator matrix must be 
   compatible with the parity check matrix, if it has been read.  If the 
   second argument is 1, only the column ordering (the last N-M of which are 
   the indexes of the message bits) is read, into the 'cols' global variable.  
   Otherwise, everything is read, into the global variables appropriate
   to the representation.  The 'type' global variable is set to a letter
   indicating which represention is used. 

   If an error is encountered, a message is displayed on standard error,
   and the program is terminated. */

void read_gen
( char *gen_file,	/* Name of generator matrix file */
  int cols_only,	/* Read only column ordering? */
  int no_pchk_file	/* No parity check file used? */
)
{
  int M2, N2;
  FILE *f;
  int i;

  f = open_file_std(gen_file,"rb");
  if (f==NULL)
  { fprintf(stderr,"Can't open generator matrix file: %s\n",gen_file);
    exit(1);
  }

  if (intio_read(f)!=('G'<<8)+0x80)
  { fprintf(stderr,"File %s doesn't contain a generator matrix\n",gen_file);
    exit(1);
  }

  if (fread (&type, 1, 1, f) != 1) goto error;

  M2 = intio_read(f);
  N2 = intio_read(f);

  if (feof(f) || ferror(f)) goto error;

  if (no_pchk_file)
  { M = M2;
    N = N2;
  }
  else 
  { if (M2!=M || N2!=N)
    { fprintf(stderr,
              "Generator matrix and parity-check matrix are incompatible\n");
      exit(1);
    }
  }

  cols = chk_alloc (N, sizeof *cols);
  rows = chk_alloc (M, sizeof *rows);

  for (i = 0; i<N; i++)
  { cols[i] = intio_read(f);
    if (feof(f) || ferror(f)) goto error;
  }

  if (!cols_only)
  {
    switch (type)
    {
      case 's':
      { 
        for (i = 0; i<M; i++)
        { rows[i] = intio_read(f);
          if (feof(f) || ferror(f)) goto error;
        }

        if ((L = mod2sparse_read(f)) == 0) goto error;
        if ((U = mod2sparse_read(f)) == 0) goto error;
  
        if (mod2sparse_rows(L)!=M || mod2sparse_cols(L)!=M) goto garbled;
        if (mod2sparse_rows(U)!=M || mod2sparse_cols(U)<M) goto garbled;
       
        break;
      }
  
      case 'd':
      {
        if ((G = mod2dense_read(f)) == 0) goto error;
  
        if (mod2dense_rows(G)!=M || mod2dense_cols(G)!=N-M) goto garbled;
  
        break;
      }
  
      case 'm':
      {
        if ((G = mod2dense_read(f)) == 0) goto error;
  
        if (mod2dense_rows(G)!=M || mod2dense_cols(G)!=M) goto garbled;
  
        break;
      }
  
      default: 
      { fprintf(stderr,
         "Unknown type of generator matrix in file %s\n",gen_file);
        exit(1);
      }
    }
  }
  
  fclose(f);

  return;

error:
  fprintf(stderr,"Error reading generator matrix from file %s\n",gen_file);
  exit(1);

garbled:
  fprintf(stderr,"Garbled generator matrix in file %s\n",gen_file);
  exit(1);
}
