/* EXTRACT_SYSTEMATIC.C - Extract systematic positions. */

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
#include "blockio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"

_Noreturn void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  char *gen_file, *systematic_file;
  FILE *systematicf;
  int i;

  /* Look at arguments. */

  if (!(gen_file = argv[1])
   || !(systematic_file = argv[2])
   || argv[3])
  { usage();
  }

  if ((strcmp(gen_file,"-")==0) + (strcmp(systematic_file,"-")==0) > 1)
  { fprintf(stderr,"Can't read more than one stream from standard input\n");
    exit(1);
  }

  /* Read generator matrix file, up to the point of finding out which
     are the message bits. */
  
  read_gen(gen_file,1,1);


  /* Open file to write systematic positions to. */

  systematicf = open_file_std(systematic_file,"w");
  if (systematicf==NULL)
  { fprintf(stderr,"Can't create file for systematic positions: %s\n",systematic_file);
    exit(1);
  }

   for (i = M; i<N; i++)
   { fprintf(systematicf,"%d\n",cols[i]);
   }

  if (ferror(systematicf) || fclose(systematicf)!=0)
  { fprintf(stderr,"Error writing systematic positions to %s\n",systematic_file);
    exit(1);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

_Noreturn void usage(void)
{ fprintf(stderr,
    "Usage: extract gen-file systematic_file\n");
  exit(1);
}
