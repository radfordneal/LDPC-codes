/* ALIST-TO-PCHK.C - Convert a parity check matrix from alist format. */

/* Copyright (c) 2006 by Radford M. Neal 
 *
 * Permission is granted for anyone to copy, use, modify, or distribute this
 * program and accompanying programs and documents for any purpose, provided 
 * this copyright notice is retained and prominently displayed, along with
 * a note saying that the original programs are available from Radford Neal's
 * web page, and note is made of any changes made to the programs.  The
 * programs and documents are distributed without any warranty, express or
 * implied.  As the programs were written for research purposes only, they have
 * not been tested to the degree that would be advisable in any important
 * application.  All use of these programs is entirely at the user's own risk.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "alloc.h"
#include "intio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"


void bad_alist_file(void);
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
  int tot, trans;

  trans = 0;

  for (;;)
  {
    if (argc>1 && strcmp(argv[1],"-t")==0)
    { trans = 1;
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

  pchk_file = argv[2];
  alist_file = argv[1];

  af = open_file_std(alist_file,"r");

  if (af==NULL) 
  { fprintf(stderr,"Can't open alist file: %s\n",alist_file);
    exit(1);
  }

  if (fscanf(af,"%d",&M)!=1 || M<1
   || fscanf(af,"%d",&N)!=1 || N<1
   || fscanf(af,"%d",&mxrw)!=1 || mxrw<0 || mxrw>N 
   || fscanf(af,"%d",&mxcw)!=1 || mxcw<0 || mxcw>M)
  { bad_alist_file();
  }

  rw = (int *) chk_alloc (M, sizeof *rw);

  for (i = 0; i<M; i++)
  { if (fscanf(af,"%d",&rw[i])!=1 || rw[i]<0 || rw[i]>N)
    { bad_alist_file();
    }
  }

  cw = (int *) chk_alloc (N, sizeof *cw);

  for (j = 0; j<N; j++)
  { if (fscanf(af,"%d",&cw[j])!=1 || cw[j]<0 || cw[j]>M)
    { bad_alist_file();
    }
  }

  H = mod2sparse_allocate(M,N);

  tot = 0;

  for (i = 0; i<M; i++)
  { for (k = 0; k<mxrw; k++)
    { if (fscanf(af,"%d",&j)!=1 || j<0 || j>N
       || k>=rw[i] && j!=0 || k<rw[i] && j==0)
      { bad_alist_file();
      }
      if (j==0) continue;
      if (mod2sparse_find(H,i,j-1))
      { bad_alist_file();
      }
      mod2sparse_insert(H,i,j-1);
      tot += 1;
    }
  }

  for (j = 0; j<N; j++)
  { for (k = 0; k<mxcw; k++)
    { if (fscanf(af,"%d",&i)!=1 || i<0 || i>M
       || k>=cw[j] && i!=0 || k<cw[j] && i==0)
      { bad_alist_file();
      }
      if (i==0) continue;
      if (!mod2sparse_find(H,i-1,j))
      { bad_alist_file();
      }
      tot -= 1;
    }
  }

  if (tot!=0)
  { bad_alist_file();
  }

  if (fscanf(af,"%d",&i)==1 || !feof(af))
  { bad_alist_file();
  }

  if (trans)
  { mod2sparse *HT;
    HT = H;
    H = mod2sparse_allocate(N,M);
    mod2sparse_transpose(HT,H);
  }
  
  pf = open_file_std(pchk_file,"wb");
  if (pf==NULL) 
  { fprintf(stderr,"Can't create parity check file: %s\n",pchk_file);
    exit(1);
  }

  intio_write(pf,('P'<<8)+0x80);
  
  if (ferror(pf) || !mod2sparse_write(pf,H) || fclose(pf)!=0)
  { fprintf(stderr,"Error writing to parity check file %s\n",pchk_file);
    exit(1);
  }

  return 0;
}


/* COMPLAIN THAT ALIST FILE IS BAD. */

void bad_alist_file()
{ fprintf(stderr,"Alist file doesn't have the right format\n");
  exit(1);
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,"Usage: alist-to-pchk [ -t ] alist-file pchk-file\n");
  exit(1);
}
