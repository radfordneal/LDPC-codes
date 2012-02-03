/* INTIO.C - Routines to read and write integers one byte at a time. */

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

#include "intio.h"


/* READ AN INTEGER ONE BYTE AT A TIME.  Four bytes are read, ordered from
   low to high order.  These are considered to represent a signed integer,
   in two's complement form.  The value returned is this integer, converted 
   to whatever a C "int" is.  The conversion should work as long as an "int" 
   is at least four bytes, even if it's not in two's complement representation 
   (except for the largest two's complement negative integer).

   If an error or eof is encountered, zero is returned.  The caller can
   check for these events using feof and ferror.  

   The file read from should have been opened as "binary".
*/

int intio_read
( FILE *f   /* File to read from */
)
{ 
  unsigned char b[4];
  int top;
  int i;

  for (i = 0; i<4; i++)
  { if (fread(&b[i],1,1,f) != 1) return 0;
  }

  top = b[3]>127 ? (int)b[3] - 256 : b[3];
  
  return (top<<24) + (b[2]<<16) + (b[1]<<8) + b[0];
}


/* WRITE AN INTEGER ONE BYTE AT A TIME.  Four bytes are written, ordered from
   low to high order.  These are considered to represent a signed integer,
   in two's complement form.  This should work as long as the integer passed
   can be represented in four bytes, even if a C "int" is longer than this.

   The file written to should have been opened as "binary".
*/

void intio_write
( FILE *f,  /* File to write to */
  int v     /* Value to write to file */
)
{ 
  unsigned char b;
  int i;

  for (i = 0; i<3; i++)
  { b = v&0xff;
    fwrite(&b,1,1,f);
    v >>= 8;
  }

  b = v>0 ? v : v+256;
  fwrite(&b,1,1,f);
}


/* TEST PROGRAM. */

#ifdef TEST_INTIO

main(void)
{ 
  FILE *f;
  f = fopen("test","wb");
  intio_write(f,334);
  intio_write(f,-40000);
  intio_write(f,0x8fffffff);
  intio_write(f,-0x8fffffff);
  fclose(f);
  f = fopen("test","rb");
  if (intio_read(f)!=334
   || intio_read(f)!=-40000
   || intio_read(f)!=0x8fffffff
   || intio_read(f)!=-0x8fffffff)
  { fprintf(stderr,"got back bad data\n");
    exit(1);
  }
  if (intio_read(f)!=0 || !feof(f) || ferror(f))
  { fprintf(stderr,"eof not handled correctly\n");
    exit(1);
  }
  fclose(f);
  fprintf(stderr,"OK\n");
  exit(0);
}

#endif
