/* TRANSMIT.C - Simulate transmission of bits through a channel. */

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

#include "channel.h"
#include "open.h"
#include "rand.h"

void usage(void);


/* MAIN PROGRAM. */

int main
( int argc,
  char **argv
)
{
  char *tfile, *rfile;
  FILE *tf, *rf;
  int block_size, n_bits;
  char junk;
  int seed;
  int cnt;
  int n, b;

  /* Look at arguments.  The arguments specifying the channel are looked
     at by channel_parse in channel.c */

  if (!(tfile = argv[1])
   || !(rfile = argv[2])
   || !argv[3] || sscanf(argv[3],"%d%c",&seed,&junk)!=1)
  { usage();
  }

  n = channel_parse(argv+4,argc-4);
  if (n<=0 || argc-4-n!=0) 
  { usage();
  }

  /* See if the source is all zeros or a file. */

  if (sscanf(tfile,"%d%c",&n_bits,&junk)==1 && n_bits>0)
  { block_size = 1;
    tf = NULL;
  }
  else if (sscanf(tfile,"%dx%d%c",&block_size,&n_bits,&junk)==2 
            && block_size>0 && n_bits>0)
  { n_bits *= block_size;
    tf = NULL;
  }
  else
  { tf = open_file_std(tfile,"r");
    if (tf==NULL)
    { fprintf(stderr,"Can't open encoded file to transmit: %s\n",tfile);
      exit(1);
    }
  }

  /* Open output file. */

  rf = open_file_std(rfile,"w");
  if (rf==NULL)
  { fprintf(stderr,"Can't create file for received data: %s\n",rfile);
    exit(1);
  }

  /* Set random seed to avoid duplications with other programs. */

  rand_seed(10*seed+3);

  /* Transmit bits. */

  for (cnt = 0; ; cnt++)
  { 
    /* Get next bit to transmit. */

    if (tf) /* Data comes from a file */
    { 
      for (;;)
      { b = getc(tf);
        if (b!=' ' && b!='\t' && b!='\n' && b!='\r')
        { break;
        }
        putc(b,rf);
      }

      if (b==EOF) break;

      if (b!='0' && b!='1')
      { fprintf(stderr,"Bad character (code %d) file being transmitted\n",b);
        exit(1);
      }
    }

    else /* Data is all zeros */
    { 
      if (cnt>0 && cnt%block_size==0) 
      { putc('\n',rf);
      }

      if (cnt==n_bits) break;

      b = '0';
    }

    b = b=='1';

    /* Produce the channel output for this transmitted bit. */
    
    switch (channel)
    { case BSC:
      { int bsc_noise;
        bsc_noise = rand_uniform() < error_prob;
        fprintf (rf, "%d", b^bsc_noise);
        break;
      }
      case AWGN:
      { double awgn_noise;
        awgn_noise = std_dev * rand_gaussian();
        fprintf (rf, " %+5.2f", b ? 1+awgn_noise : -1+awgn_noise);
        break;
      }
      case AWLN:
      { double awln_noise;
        awln_noise = lwidth * rand_logistic();
        fprintf (rf, " %+5.2f", b ? 1+awln_noise : -1+awln_noise);
        break;
      }
      default:
      { abort();
      }
    }
  }

  fprintf(stderr,"Transmitted %d bits\n",cnt);

  if (ferror(rf) || fclose(rf)!=0)
  { fprintf(stderr,"Error writing received bits to %s\n",rfile);
    exit(1);
  }

  return 0;
}


/* PRINT USAGE MESSAGE AND EXIT. */

void usage(void)
{ fprintf(stderr,
    "Usage:   transmit encoded-file|n-zeros received-file seed channel\n");
  channel_usage();
  exit(1);
}
