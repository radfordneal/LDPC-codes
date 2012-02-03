/* OPEN.C - Routine for opening file that might be stdin/stdout. */

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
#include <string.h>

#include "open.h"


/* OPEN A FILE THAT MIGHT BE STANDARD INPUT OR OUTPUT.  If the file name
   given is "-", this procedure just returns stdin or stdout, depending on
   whether the mode is for reading or writing.  Otherwise, fopen is called. 
*/

FILE *open_file_std
( char *fname,	/* Name of file to open, or "-" for stdin/stdout */
  char *mode	/* Mode for opening: eg, "r" or "w" */
)
{ 
  if (strcmp(fname,"-")==0)
  { switch (mode[0])
    { case 'r': 
      { return stdin;
      }
      case 'w': 
      { return stdout;
      }
      default:  
      { fprintf(stderr,"Bad mode passed to open_file_std: %s\n",mode);
        exit(1);
      }
    }
  }
  else
  { return fopen(fname,mode);
  }
}
