/* RAND.C - Random number generation module. */

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

/* Random generation routines at the end of this file are taken from the
   GNU C library, see the copyright notice there. */


/* NOTE:  See rand.html for documentation on these procedures. */


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "rand.h"


static long int this_nrand48 (unsigned short int [3]);
				       	     /* Local version of nrand48 */


/* This module uses the 'this_nrand48' pseudo-random number generator from the
   GNU C library, included below, but renamed to 'this_nrand48'.  The
   output of this generator is combined with a file of real random numbers.

   Many of the methods used in this module may be found in the following
   reference:

      Devroye, L. (1986) Non-Uniform Random Variate Generation, 
        New York: Springer-Verlag.

   The methods used here are not necessarily the fastest available.  They're
   selected to be reasonably fast while also being easy to write.
*/


/* CONSTANT PI.  Defined here if not in <math.h>. */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/* TABLES OF REAL RANDOM NUMBERS.  A file of 100000 real random numbers
   (NOT pseudo-random) is used in conjunction with pseudo-random numbers
   for extra insurance.  These are employed in the form of five tables
   of 5000 32-bit integers.  

   The file must be located at the path given by RAND_FILE, which should
   be defined on the "cc" command line. */

#define Table_size 5000			/* Number of words in each table */

static int rn[N_tables][Table_size];	/* Random number tables */


/* STATE OF RANDOM NUMBER GENERATOR. */

static int initialized = 0;		/* Has module been initialized? */

static rand_state state0;		/* Default state structure */

static rand_state *state;		/* Pointer to current state */


/* INITIALIZE MODULE.  Sets things up using the default state structure,
   set as if rand_seed had been called with a seed of one. */

static void initialize (void)
{
  int i, j, k, w;
  char b;
  FILE *f;

  if (!initialized)
  {
    f = fopen(RAND_FILE,"rb");
    
    if (f==NULL)
    { fprintf(stderr,"Can't open file of random numbers (%s)\n",RAND_FILE);
      exit(1);
    }

    for (i = 0; i<N_tables; i++)
    { for (j = 0; j<Table_size; j++)
      { w = 0;
        for (k = 0; k<4; k++)
        { if (fread(&b,1,1,f)!=1)
          { fprintf(stderr,"Error reading file of random numbers (%s)\n",
                            RAND_FILE);
            exit(1);
          }
          w = (w<<8) | (b&0xff);
        }
        rn[i][j] = w;
      }
    }

    state = &state0;

    initialized = 1;

    rand_seed(1);
  }
}


/* SET CURRENT STATE ACCORDING TO SEED. */

void rand_seed
( int seed
)
{ 
  int j;

  if (!initialized) initialize();

  state->seed = seed;

  state->state48[0] = seed>>16;
  state->state48[1] = seed&0xffff;
  state->state48[2] = rn[0][(seed&0x7fffffff)%Table_size];

  for (j = 0; j<N_tables; j++) 
  { state->ptr[j] = seed%Table_size;
    seed /= Table_size;
  }
}


/* SET STATE STRUCTURE TO USE. */

void rand_use_state
( rand_state *st
)
{ 
  if (!initialized) initialize();

  state = st;
}


/* RETURN POINTER TO CURRENT STATE. */

rand_state *rand_get_state (void)
{ 
  if (!initialized) initialize();

  return state;
}


/* GENERATE RANDOM 31-BIT INTEGER.  Not really meant for use outside this
   module. */

int rand_word(void)
{
  int v;
  int j;

  if (!initialized) initialize();

  v = this_nrand48(state->state48);

  for (j = 0; j<N_tables; j++)
  { v ^= rn[j][state->ptr[j]];
  }

  for (j = 0; j<N_tables && state->ptr[j]==Table_size-1; j++) 
  { state->ptr[j] = 0;
  }

  if (j<N_tables) 
  { state->ptr[j] += 1;
  }

  return v & 0x7fffffff;
}


/* GENERATE UNIFORMLY FROM [0,1). */

double rand_uniform (void)
{
  return (double)rand_word() / (1.0+(double)0x7fffffff);
}


/* GENERATE UNIFORMLY FORM (0,1). */

double rand_uniopen (void)
{
  return (0.5+(double)rand_word()) / (1.0+(double)0x7fffffff);
}


/* GENERATE RANDOM INTEGER FROM 0, 1, ..., (n-1). */

int rand_int
( int n
)
{ 
  return (int) (n * rand_uniform());
}


/* GENERATE INTEGER FROM 0, 1, ..., (n-1), WITH GIVEN DISTRIBUTION. */

int rand_pickd
( double *p,
  int n
)
{ 
  double t, r;
  int i;

  t = 0;
  for (i = 0; i<n; i++)
  { if (p[i]<0) abort();
    t += p[i];
  }

  if (t<=0) abort();

  r = t * rand_uniform();

  for (i = 0; i<n; i++)
  { r -= p[i];
    if (r<0) return i;
  }

  /* Return value with non-zero probability if we get here due to roundoff. */

  for (i = 0; i<n; i++) 
  { if (p[i]>0) return i;
  }

  abort(); 
}


/* SAME PROCEDURE AS ABOVE, BUT WITH FLOAT ARGUMENT. */

int rand_pickf
( float *p,
  int n
)
{ 
  double t, r;
  int i;

  t = 0;
  for (i = 0; i<n; i++)
  { if (p[i]<=0) abort();
    t += p[i];
  }

  if (t<=0) abort();

  r = t * rand_uniform();

  for (i = 0; i<n; i++)
  { r -= p[i];
    if (r<0) return i;
  }

  /* Return value with non-zero probability if we get here due to roundoff. */

  for (i = 0; i<n; i++) 
  { if (p[i]>0) return i;
  }

  abort(); 
}


/* GENERATE RANDOM PERMUTATION OF INTEGERS FROM 1 TO N. */

void rand_permutation
( int *perm,		/* Place to store permutation */
  int n			/* Number of integers to permute */
)
{
  int i, j, t;

  for (i = 0; i<n; i++) 
  { perm[i] = i+1;
  }

  for (i = 0; i<n; i++)
  { t = perm[i];
    j = i + rand_int(n-i);
    perm[i] = perm[j];
    perm[j] = t;
  }
}


/* POISSON GENERATOR.  The method used is simple, but not very fast.  See
   Devroye, p. 503.  Very large means are done using Gaussian approximation. */

int rand_poisson 
( double lambda
)
{ int v;
  if (lambda>10000)
  { v = (int) (lambda + rand_gaussian()*sqrt(lambda) + 0.5);
  }
  else
  { v = 0;
    for (;;)
    { lambda -= rand_exp();
      if (lambda<=0) break;
      v += 1;
    }
  }
  return v;
}


/* GAUSSIAN GENERATOR.  Done by using the Box-Muller method, but only one
   of the variates is retained (using both would require saving more state).
   See Devroye, p. 235. 

   As written, should never deliver exactly zero, which may sometimes be
   helpful. */

double rand_gaussian (void)
{
  double a, b;

  a = rand_uniform();
  b = rand_uniopen();

  return cos(2.0*M_PI*a) * sqrt(-2.0*log(b));
}


/* EXPONENTIAL GENERATOR.  See Devroye, p. 29.  Written so as to never
   return exactly zero. */

double rand_exp (void)
{
  return -log(rand_uniopen());
}


/* LOGISTIC GENERATOR.  Just inverts the CDF. */

double rand_logistic (void)
{ double u;
  u = rand_uniopen();
  return log(u/(1-u));
}


/* CAUCHY GENERATOR.  See Devroye, p. 29. */

double rand_cauchy (void)
{
  return tan (M_PI * (rand_uniopen()-0.5));
}


/* GAMMA GENERATOR.  Generates a positive real number, r, with density
   proportional to r^(a-1) * exp(-r).  See Devroye, p. 410 and p. 420. 
   Things are fiddled to avoid ever returning a value that is very near 
   zero. */

double rand_gamma
( double a
)
{
  double b, c, X, Y, Z, U, V, W;

  if (a<0.00001)
  { X = a;
  }

  else if (a<=1) 
  { 
    U = rand_uniopen();
    X = rand_gamma(1+a) * pow(U,1/a);
  }

  else if (a<1.00001)
  { X = rand_exp();
  }

  else
  {
    b = a-1;
    c = 3*a - 0.75;
  
    for (;;)
    {
      U = rand_uniopen();
      V = rand_uniopen();
    
      W = U*(1-U);
      Y = sqrt(c/W) * (U-0.5);
      X = b+Y;
  
      if (X>=0)
      { 
        Z = 64*W*W*W*V*V;
  
        if (Z <= 1 - 2*Y*Y/X || log(Z) <= 2 * (b*log(X/b) - Y)) break;
      }
    }
  }

  return X<1e-30 && X<a ? (a<1e-30 ? a : 1e-30) : X;
}


/* BETA GENERATOR. Generates a real number, r, in (0,1), with density
   proportional to r^(a-1) * (1-r)^(b-1).  Things are fiddled to avoid
   the end-points, and to make the procedure symmetric between a and b. */

double rand_beta 
( double a, 
  double b
)
{
  double x, y, r;

  do
  { x = rand_gamma(a);
    y = rand_gamma(b);
    r = 1.0 + x/(x+y);
    r = r - 1.0;
  } while (r<=0.0 || r>=1.0);

  return r;
}


/* ROUTINES FROM THE GNU C LIBRARY.  These were modified to extract 
   only the routines used here, and to allow them to be included in 
   this module without any possible name conflict with other modules.
   Inclusion here ensures that these routines are always available, and 
   operate in exactly the same way on all systems.  The routines as copied
   below are still easily useable by other programs by simply inserting 
   this source code into an appropriate source file.
   
   The following is the copyright notice for these routines:

     Copyright (C) 1995, 1996, 1997, 2002 Free Software Foundation, Inc.
     This file is part of the GNU C Library.
     Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, August 1995.
  
     The GNU C Library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.
  
     The GNU C Library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.
  
     You should have received a copy of the GNU Lesser General Public
     License along with the GNU C Library; if not, write to the Free
     Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
     02111-1307 USA.  

   The GNU Lesser General Public License is included with these source
   files in the file LGPL. */

#include <errno.h>
#include <limits.h>
#include <sys/types.h>

struct this_drand48_data
  {
    unsigned short int x[3];	/* Current state.  */
    unsigned short int old_x[3]; /* Old state.  */
    unsigned short int c;	/* Additive const. in congruential formula.  */
    unsigned short int init;	/* Flag for initializing.  */
    unsigned long long int a;	/* Factor in congruential formula.  */
  };

/* Global state for non-reentrant functions.  */

struct this_drand48_data libc_this_drand48_data;

static int this_nrand48_r (unsigned short int xsubi[3],
                      struct this_drand48_data *buffer,
                      long int *result);

/* Internal function to compute next state of the generator.  */

static int this_drand48_iterate (unsigned short int xsubi[3],
                            struct this_drand48_data *buffer);

static long int this_nrand48 (xsubi)
     unsigned short int xsubi[3];
{
  long int result;

  (void) this_nrand48_r (xsubi, &libc_this_drand48_data, &result);

  return result;
}

static int this_nrand48_r (xsubi, buffer, result)
     unsigned short int xsubi[3];
     struct this_drand48_data *buffer;
     long int *result;
{
  /* Compute next state.  */
  if (this_drand48_iterate (xsubi, buffer) < 0)
    return -1;

  /* Store the result.  */
  if (sizeof (unsigned short int) == 2)
    *result = xsubi[2] << 15 | xsubi[1] >> 1;
  else
    *result = xsubi[2] >> 1;

  return 0;
}

static int this_drand48_iterate (xsubi, buffer)
     unsigned short int xsubi[3];
     struct this_drand48_data *buffer;
{
  uint64_t X;
  uint64_t result;

  /* Initialize buffer, if not yet done.  */
  if (!buffer->init)
    {
      buffer->a = 0x5deece66dull;
      buffer->c = 0xb;
      buffer->init = 1;
    }

  /* Do the real work.  We choose a data type which contains at least
     48 bits.  Because we compute the modulus it does not care how
     many bits really are computed.  */

  X = (uint64_t) xsubi[2] << 32 | (uint32_t) xsubi[1] << 16 | xsubi[0];

  result = X * buffer->a + buffer->c;

  xsubi[0] = result & 0xffff;
  xsubi[1] = (result >> 16) & 0xffff;
  xsubi[2] = (result >> 32) & 0xffff;

  return 0;
}
