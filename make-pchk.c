/* MAKE-PCHK.C - Make a parity check matrix explicitly. */

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
#include "intio.h"
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
  char **bit_specs;
  char *file;
  FILE *f;
  int i, j, k;
  char junk;

  if (!(file = argv[1]) 
   || !argv[2] || sscanf(argv[2],"%d%c",&M,&junk)!=1 || M<=0
   || !argv[3] || sscanf(argv[3],"%d%c",&N,&junk)!=1 || N<=0)
  { usage();
  }

  bit_specs = argv+4;

  if (bit_specs[0]==0) 
  { usage();
  }

  H = mod2sparse_allocate(M,N);

  for (k = 0; bit_specs[k]!=0; k++)
  { if (sscanf(bit_specs[k],"%d:%d%c",&i,&j,&junk)!=2 || i<0 || j<0) 
    { usage();
    }
    if (i>=M || j>=N)
    { fprintf(stderr,"Bit %d:%d is out of range\n",i,j);
      exit(1);
    }
    mod2sparse_insert(H,i,j);
  }

  f = open_file_std(file,"wb");
  if (f==NULL) 
  { fprintf(stderr,"Can't create parity check file: %s\n",file);
    exit(1);
  }

  intio_write(f,('P'<<8)+0x80);
  
  if (ferror(f) || !mod2sparse_write(f,H) || fclose(f)!=0)
  { fprintf(stderr,"Error writing to parity check file %s\n",file);
    exit(1);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,"Usage: make-pchk pchk-file n-checks n-bits row:col ...\n");
  exit(1);
}
