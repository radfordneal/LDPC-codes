/* BLOCKIO.C - Routines to read/write blocks of bits from/to a text file. */

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

#include <stdlib.h>
#include <stdio.h>

#include "blockio.h"


int blockio_flush = 0;	/* Should blocks written be immediately flushed? */


/* READ A BLOCK OF BITS.  The bits must be given as '0' or '1' characters,
   with whitespace allowed (but not required) between bits.  Returns 0 if
   a block is read successfully, and EOF if eof or an error occurs.  If
   EOF is returned, a warning will be printed if a partial block had already
   been read. */

int blockio_read
( FILE *f,    /* File to read from */
  char *b,    /* Place to store bits read */
  int l       /* Length of block */
)
{
  int i, c;

  for (i = 0; i<l; i++)
  { 
    do
    { c = getc(f);
      if (c==EOF) 
      { if (i>0)
        { fprintf(stderr,
           "Warning: Short block (%d long) at end of input file ignored\n",i);
        }
        return EOF;
      }
    } while (c==' ' || c=='\t' || c=='\n' || c=='\r');

    if (c!='0' && c!='1')
    { fprintf(stderr,"Bad character in binary file (not '0' or '1')\n");
      exit(1);
    }

    b[i] = c=='1';
  }

  return 0;
}


/* WRITE A BLOCK OF BITS.  Bits are written as '0' and '1' characters, with
   no spaces between them, followed by a newline. */

void blockio_write
( FILE *f,     /* File to write to */
  char *b,     /* Block of bits to write */
  int l        /* Length of block */
)
{ 
  int i;

  for (i = 0; i<l; i++)
  { if (b[i]!=0 && b[i]!=1) abort();
    putc("01"[b[i]],f);
  }

  putc('\n',f);

  if (blockio_flush)
  { fflush(f);
  }
}
