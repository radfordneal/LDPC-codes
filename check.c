/* CHECK.C - Compute parity checks and other stats on decodings. */

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
#include <math.h>

#include "mod2sparse.h"
#include "check.h"



/* COMPUTE PARITY CHECKS.  Returns the number of parity checks violated by
   dblk.  The results of all the parity checks are stored in pchk. */

int check
( mod2sparse *H,	/* Parity check matrix */
  char *dblk,		/* Guess for codeword */
  char *pchk		/* Place to store parity checks */
)
{
  int M, i, c;

  M = mod2sparse_rows(H);

  mod2sparse_mulvec (H, dblk, pchk);

  c = 0;
  for (i = 0; i<M; i++) 
  { c += pchk[i];
  }

  return c;
}


/* COUNT HOW MANY BITS HAVED CHANGED FROM BIT INDICATED BY LIKELIHOOD.  The
   simple decoding based on likelihood ratio is compared to the given decoding.
   A bit for which the likelihood ratio is exactly one counts as half a 
   change, which explains why the result is a double rather than an int.
 */

double changed
( double *lratio,	/* Likelihood ratios for bits */
  char *dblk,		/* Candidate decoding */
  int N			/* Number of bits */
)
{ 
  double changed;
  int j;

  changed = 0;
  for (j = 0; j<N; j++)
  { changed += lratio[j]==1 ? 0.5 : dblk[j] != (lratio[j]>1); 
  }

  return changed;
}


/* COMPUTE THE EXPECTED NUMBER OF PARITY CHECK ERRORS.   Computes the
   expected number of parity check errors with respect to the distribution
   given by the bit probabilities passed, with bits assumed to be independent. 
 */

double expected_parity_errors
( mod2sparse *H,	/* Parity check matrix */
  double *bpr		/* Bit probabilities */
)
{ 
  mod2entry *f;
  double ee, p;
  int M, i, j;

  M = mod2sparse_rows(H);

  ee = 0;

  for (i = 0; i<M; i++)
  { p = 0;
    for (f = mod2sparse_first_in_row(H,i);
         !mod2sparse_at_end(f);
         f = mod2sparse_next_in_row(f))
    { j = mod2sparse_col(f);
      p = p * (1-bpr[j]) + (1-p) * bpr[j];
    }
    ee += p;
  }

  return ee;
}


/* COMPUTE LOG LIKELIHOOD OF A DECODING. */

double loglikelihood 
( double *lratio,	/* Likelihood ratios for bits */
  char *bits,		/* Bits in decoding */
  int N			/* Length of codeword */
)
{ 
  double ll;
  int j;
  
  ll = 0;

  for (j = 0; j<N; j++)
  { ll -= bits[j] ? log(1+1/lratio[j]) : log(1+lratio[j]);
  }

  return ll;
}


/* COMPUTE THE EXPECTED LOG LIKELIHOOD BASED ON BIT PROBABILITIES.  Computes
   the expected value of the log likelihood with respect to the distribution
   given by the bit probabilities passed, with bits assumed to be independent. 
 */

double expected_loglikelihood 
( double *lratio,	/* Likelihood ratios for bits */
  double *bpr,		/* Bit probabilities */
  int N			/* Length of codeword */
)
{ 
  double ll;
  int j;
  
  ll = 0;

  for (j = 0; j<N; j++)
  { if (bpr[j]>0) 
    { ll -= bpr[j]*log(1+1/lratio[j]);
    }
    if (bpr[j]<1) 
    { ll -= (1-bpr[j])*log(1+lratio[j]);
    }
  }

  return ll;
}


/* COMPUTE ENTROPY FROM BIT PROBABILITIES.  Computes the entropy of the
   distribution given by the bit probabilities, on the assumption that
   bits are independent.
 */

double entropy 
( double *bpr,		/* Bit probabilities */
  int N			/* Length of codeword */
)
{
  double e;
  int j;

  e = 0;
  for (j = 0; j<N; j++)
  { if (bpr[j]>0 && bpr[j]<1)
    { e -= bpr[j]*log(bpr[j]) + (1-bpr[j])*log(1-bpr[j]);
    }
  }

  return e/log(2.0);
}
