/* EXTRACT.C - Extract message bits from coded blocks. */

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

void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  char *gen_file, *coded_file, *ext_file;
  FILE *codef, *extf;
  char *cblk;
  int i;

  /* Look at arguments. */

  if (!(gen_file = argv[1])
   || !(coded_file = argv[2])
   || !(ext_file = argv[3])
   || argv[4])
  { usage();
  }

  if ((strcmp(gen_file,"-")==0) + (strcmp(coded_file,"-")==0) > 1)
  { fprintf(stderr,"Can't read more than one stream from standard input\n");
    exit(1);
  }

  /* Read generator matrix file, up to the point of finding out which
     are the message bits. */
  
  read_gen(gen_file,1,1);

  /* Open decoded file. */

  codef = open_file_std(coded_file,"r");
  if (codef==NULL)
  { fprintf(stderr,"Can't open coded file: %s\n",coded_file);
    exit(1);
  }

  /* Open file to write extracted message bits to. */

  extf = open_file_std(ext_file,"w");
  if (extf==NULL)
  { fprintf(stderr,"Can't create file for extracted bits: %s\n",ext_file);
    exit(1);
  }

  cblk = chk_alloc (N, sizeof *cblk);

  for (;;)
  { 
    /* Read block from coded file. */

    if (blockio_read(codef,cblk,N)==EOF) break;

    /* Extract message bits and write to file, followed by newline to mark
       block boundary. */

    for (i = M; i<N; i++)
    { putc("01"[cblk[cols[i]]],extf);
    }
   
    putc('\n',extf);
  }

  if (ferror(extf) || fclose(extf)!=0)
  { fprintf(stderr,"Error writing extracted data to %s\n",ext_file);
    exit(1);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,
    "Usage: extract gen-file decoded-file extracted-file\n");
  exit(1);
}
