/* RAND-SRC.C - Generate random message bits. */

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

#include "open.h"
#include "rand.h"

void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  int seed, bs, nb;
  char *file, *n_bits;
  char junk;
  int i, j;
  FILE *f;

  if (!(file = argv[1])
   || !argv[2] || sscanf(argv[2],"%d%c",&seed,&junk)!=1
   || !(n_bits = argv[3])
   || argv[4])
  { usage();
  }

  if (sscanf(n_bits,"%d%c",&nb,&junk)==1)
  { if (nb<=0) usage();
    bs = 1;
  }
  else if (sscanf(n_bits,"%dx%d%c",&bs,&nb,&junk)==2)
  { if (nb<=0 || bs<=0) usage();
  }
  else
  { usage();
  }

  f = open_file_std(file,"w");
  if (f==NULL)
  { fprintf(stderr,"Can't create source file: %s\n",file);
    exit(1);
  }

  rand_seed(10*seed+2);

  for (i = 0; i<nb; i++)
  { for (j = 0; j<bs; j++)
    { fprintf(f,"%d",rand_int(2));
    }
    fprintf(f,"\n");
  }

  if (ferror(f) || fclose(f)!=0) 
  { fprintf(stderr,"Error writing random source blocks to %s\n",file);
    exit(1);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,"Usage: rand-src source-file seed n-bits\n");
  exit(1);
}
