/* VERIFY.C - Verify encoded or decoded blocks. */

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
#include "check.h"

void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  char *coded_file, *source_file;
  char *pchk_file, *gen_file;
  int table;

  char *sblk, *cblk, *chks;
  int seof, ceof;
  int srcerr, chkerr, bit_errs;
  int i, n;
  FILE *srcf, *codef;

  int tot_srcerrs, tot_chkerrs, tot_botherrs;

  /* Look at arguments. */

  table = 0;
  if (argc>1 && strcmp(argv[1],"-t")==0)
  { table = 1;
    argc -= 1;
    argv += 1;
  }

  if (argc<3 || argc>5) usage();

  if (!(pchk_file = argv[1])
   || !(coded_file = argv[2]))
  { usage();
  }
 
  gen_file = 0;
  source_file = 0;

  if (argv[3])
  { gen_file = argv[3];
    if (argv[4])
    { source_file = argv[4];
      if (argv[5])
      { usage();
      }
    }
  }

  if ((strcmp(pchk_file,"-")==0) 
    + (strcmp(coded_file,"-")==0) 
    + (source_file!=0 && strcmp(source_file,"-")==0)
    + (gen_file!=0 && strcmp(gen_file,"-")==0) > 1)
  { fprintf(stderr,"Can't read more than one stream from standard input\n");
    exit(1);
  }

  /* Read parity check file. */

  read_pchk(pchk_file);

  if (N<=M)
  { fprintf(stderr,
     "Number of bits (%d) should be greater than number of checks (%d)\n",N,M);
    exit(1);
  }

  /* Read generator matrix file, if given, up to the point of finding
     out which are the message bits. */

  if (gen_file!=0)
  { read_gen(gen_file,1,0);
  }

  /* Open coded file to check. */

  codef = open_file_std(coded_file,"r");
  if (codef==NULL)
  { fprintf(stderr,"Can't open coded file: %s\n",coded_file);
    exit(1);
  }

  /* Open source file to verify against, if given. */

  if (source_file!=0)
  { 
    srcf = open_file_std(source_file,"r");
    if (srcf==NULL)
    { fprintf(stderr,"Can't open source file: %s\n",source_file);
      exit(1);
    }
  }

  sblk = chk_alloc (N-M, sizeof *sblk);
  cblk = chk_alloc (N, sizeof *cblk);
  chks = chk_alloc (M, sizeof *chks);

  /* Print header for table. */

  if (table)
  { if (gen_file!=0)
    { printf("  block chkerrs srcerrs\n");
    }
    else
    { printf("  block chkerrs\n");
    }
  }

  /* Verify successive blocks. */

  tot_srcerrs = 0;
  tot_chkerrs = 0;
  tot_botherrs = 0;

  bit_errs = 0;

  seof = 0; 
  ceof = 0;

  for (n = 0; ; n++)
  { 
    /* Read block from coded file. */
    
    if (blockio_read(codef,cblk,N)==EOF) 
    { ceof = 1;
    }

    /* Read block from source file, if given. */

    if (source_file!=0 && !ceof && !seof)
    { if (blockio_read(srcf,sblk,N-M)==EOF) 
      { fprintf(stderr,"Warning: Not enough source blocks (only %d)\n",n);
        seof = 1;
      }
    }

    /* Stop if end of received file. */

    if (ceof) break;

    /* Check that received block is a code word, and if not find the number of
       parity check errors. */

    chkerr = check(H,cblk,chks);

    /* Check against source block, if provided, or against zeros, if
       the generator matrix was provided but no source file. */

    if (gen_file!=0)
    { srcerr = 0;
      if (source_file!=0 && !seof)
      { for (i = M; i<N; i++)
        { if (cblk[cols[i]]!=sblk[i-M])
          { srcerr += 1;
          }
        }
      }
      if (source_file==0)
      { for (i = M; i<N; i++)
        { if (cblk[cols[i]]!=0)
          { srcerr += 1;
          }
        }
      }
      bit_errs += srcerr;
    }

    /* Print table entry. */

    if (table)
    { if (gen_file!=0)
      { printf("%6d %7d %7d\n",n,chkerr,srcerr);
      }
      else
      { printf("%6d %7d\n",n,chkerr);
      }
    }

    /* Increment totals. */

    if (chkerr) tot_chkerrs += 1;
    if (gen_file!=0 && (source_file==0 || !seof))
    { if (srcerr) tot_srcerrs += 1;
      if (srcerr && chkerr) tot_botherrs += 1;
    }
  }

  /* Write final statistics to standard error.  Flush standard output 
     first to avoid mixing of output. */

  fflush(stdout);

  if (gen_file!=0)
  { fprintf(stderr,
     "Block counts: tot %d, with chk errs %d, with src errs %d, both %d\n",
      n, tot_chkerrs, tot_srcerrs, tot_botherrs);
    fprintf(stderr,
     "Bit error rate (on message bits only): %.3e\n", 
      (double)bit_errs/(n*(N-M)));
  }
  else
  { fprintf (stderr, 
     "Block counts: tot %d, with chk errs %d\n", n, tot_chkerrs);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,
 "Usage: verify [ -t ] pchk-file decoded-file [ gen-file [ source-file ] ]\n");
  exit(1);
}
