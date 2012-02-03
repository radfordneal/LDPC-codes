/* RCODE.H - Parity chk and gen matrix storage, and procedures to read them.*/

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


/* VARIABLES HOLDING DATA READ.  These are declared for real in rcode.c. */

extern mod2sparse *H;	/* Parity check matrix */

extern int M;		/* Number of rows in parity check matrix */
extern int N;		/* Number of columns in parity check matrix */

extern char type;	/* Type of generator matrix representation */
extern int *cols;	/* Ordering of columns in generator matrix */

extern mod2sparse *L, *U; /* Sparse LU decomposition, if type=='s' */
extern int *rows;	  /* Ordering of rows in generator matrix (type 's') */

extern mod2dense *G;	/* Dense or mixed representation of generator matrix,
			   if type=='d' or type=='m' */


/* PROCEDURES FOR READING DATA. */

void read_pchk (char *);
void read_gen  (char *, int, int);
