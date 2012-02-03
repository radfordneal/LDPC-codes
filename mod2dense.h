/* MOD2DENSE.H - Interface to module for handling dense mod2 matrices. */

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


/* This module implements operations on matrices of mod2 elements (bits,
   with addition and multiplication being done modulo 2).  The matrices
   are stored with consecutive bits of a column packed into words, and 
   the procedures are implemented where possible using bit operations 
   on these words.  This is an appropriate representation when the matrices 
   are dense (ie, 0s and 1s are about equally frequent). 
  
   All procedures in this module display an error message on standard 
   error and terminate the program if passed an invalid argument (indicative
   of a programming error), or if memory cannot be allocated.  Errors from 
   invalid contents of a file result in an error code being returned to the 
   caller, with no message being printed by this module. 
*/


#include <stdint.h>		/* Has the definition of uint32_t used below */

/* PACKING OF BITS INTO WORDS.  Bits are packed into 32-bit words, with
   the low-order bit coming first. */

typedef uint32_t mod2word;	/* Data type that holds packed bits. If uint32_t
		 		   doesn't exist, change it to unsigned long */

#define mod2_wordsize 32	/* Number of bits that fit in a mod2word. Can't
				   be increased without changing intio module */

#define mod2_wordsize_shift 5	/* Amount to shift by to divide by wordsize */
#define mod2_wordsize_mask 0x1f /* What to AND with to produce mod wordsize */

/* Extract the i'th bit of a mod2word. */

#define mod2_getbit(w,i) (((w)>>(i))&1) 

/* Make a word like w, but with the i'th bit set to 1 (if it wasn't already). */

#define mod2_setbit1(w,i) ((w)|(1<<(i))) 

/* Make a word like w, but with the i'th bit set to 0 (if it wasn't already). */

#define mod2_setbit0(w,i) ((w)&(~(1<<(i)))) 


/* STRUCTURE REPRESENTING A DENSE MATRIX.  These structures are dynamically
   allocated using mod2dense_allocate (or by other procedures that call
   mod2dense_allocate).  They should be freed with mod2dense_free when no 
   longer required. 

   Direct access to this structure should be avoided except in low-level
   routines.  Use the macros and procedures defined below instead. */

typedef struct 
{
  int n_rows;		/* Number of rows in the matrix */
  int n_cols;		/* Number of columns in the matrix */

  int n_words;		/* Number of words used to store a column of bits */

  mod2word **col;	/* Pointer to array of pointers to columns */

  mod2word *bits;	/* Pointer to storage block for bits in this matrix 
                           (pieces of this block are pointed to from col) */
} mod2dense;


/* MACROS. */

#define mod2dense_rows(m) ((m)->n_rows)  /* Get the number of rows or columns */
#define mod2dense_cols(m) ((m)->n_cols)  /* in a matrix                       */


/* PROCEDURES. */

mod2dense *mod2dense_allocate (int, int);
void mod2dense_free           (mod2dense *);

void mod2dense_clear    (mod2dense *);
void mod2dense_copy     (mod2dense *, mod2dense *);
void mod2dense_copyrows (mod2dense*, mod2dense *, int *);
void mod2dense_copycols (mod2dense*, mod2dense *, int *);

void mod2dense_print      (FILE *, mod2dense *);
int  mod2dense_write      (FILE *, mod2dense *);
mod2dense *mod2dense_read (FILE *);

int  mod2dense_get (mod2dense *, int, int);
void mod2dense_set (mod2dense *, int, int, int);
int  mod2dense_flip(mod2dense *, int, int);

void mod2dense_transpose (mod2dense *, mod2dense *);
void mod2dense_add       (mod2dense *, mod2dense *, mod2dense *);
void mod2dense_multiply  (mod2dense *, mod2dense *, mod2dense *);

int mod2dense_equal (mod2dense *, mod2dense *);

int mod2dense_invert          (mod2dense *, mod2dense *);
int mod2dense_forcibly_invert (mod2dense *, mod2dense *, int *, int *);
int mod2dense_invert_selected (mod2dense *, mod2dense *, int *, int *);
