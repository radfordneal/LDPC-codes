/* ALIST-TO-PCHK.C - Convert a parity check matrix to alist format. */

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
#include <string.h>
#include <math.h>

#include "alloc.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"


void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  char *alist_file, *pchk_file;
  FILE *af, *pf;
  int mxrw, mxcw;
  int *rw, *cw;
  int i, j, k;
  mod2entry *e;
  int trans;
  int nozeros;
  int last;

  trans = 0;
  nozeros = 0;

  for (;;)
  {
    if (argc>1 && strcmp(argv[1],"-t")==0)
    { trans = 1;
      argc -= 1;
      argv += 1;
    }
    else if (argc>1 && strcmp(argv[1],"-z")==0)
    { nozeros = 1;
      argc -= 1;
      argv += 1;
    }
    else
    { break;
    }
  }

  if (argc!=3)
  { usage();
  }

  pchk_file = argv[1];
  alist_file = argv[2];

  read_pchk(pchk_file);

  if (trans)
  { mod2sparse *HT;
    HT = H;
    H = mod2sparse_allocate(N,M);
    mod2sparse_transpose(HT,H);
    M = mod2sparse_rows(H);
    N = mod2sparse_cols(H);
  }

  af = open_file_std(alist_file,"wb");

  if (af==NULL) 
  { fprintf(stderr,"Can't create alist file: %s\n",alist_file);
    exit(1);
  }

  fprintf(af,"%d %d\n",M,N);

  rw = (int *) chk_alloc (M, sizeof *rw);
  mxrw = 0;

  for (i = 0; i<M; i++)
  { rw[i] = mod2sparse_count_row(H,i);
    if (rw[i]>mxrw)
    { mxrw = rw[i];
    }
  }

  cw = (int *) chk_alloc (N, sizeof *cw);
  mxcw = 0;

  for (j = 0; j<N; j++)
  { cw[j] = mod2sparse_count_col(H,j);
    if (cw[j]>mxcw)
    { mxcw = cw[j];
    }
  }

  fprintf(af,"%d %d\n",mxrw,mxcw);

  for (i = 0; i<M; i++)
  { fprintf(af,"%d%c",rw[i],i==M-1?'\n':' ');
  }

  for (j = 0; j<N; j++)
  { fprintf(af,"%d%c",cw[j],j==N-1?'\n':' ');
  }

  for (i = 0; i<M; i++)
  { e = mod2sparse_first_in_row(H,i);
    last = 0;
    for (k = 0; !last; k++)
    { last = nozeros ? k==rw[i]-1 : k==mxrw-1;
      fprintf (af, "%d%c", mod2sparse_at_end(e)?0:mod2sparse_col(e)+1,
                           last?'\n':' ');
      if (!mod2sparse_at_end(e)) 
      { e = mod2sparse_next_in_row(e);
      }
    }
  }

  for (j = 0; j<N; j++)
  { e = mod2sparse_first_in_col(H,j);
    last = 0;
    for (k = 0; !last; k++)
    { last = nozeros ? k==cw[j]-1 : k==mxcw-1;
      fprintf (af, "%d%c", mod2sparse_at_end(e)?0:mod2sparse_row(e)+1,
                           last?'\n':' ');
      if (!mod2sparse_at_end(e)) 
      { e = mod2sparse_next_in_col(e);
      }
    }
  }

  if (ferror(af) || fclose(af)!=0)
  { fprintf(stderr,"Error writing to alist file %s\n",alist_file);
    exit(1);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,"Usage: pchk-to-alist [ -t ] [ -z ] pchk-file alist-file\n");
  exit(1);
}
