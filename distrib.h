/* DISTRIB.H - Interface to module for handling distributions over numbers. */

/* Copyright (c) 2006 by Radford M. Neal and Peter Junteng Liu
 *
 * Permission is granted for anyone to copy, use, modify, or distribute this
 * program and accompanying programs and documents for any purpose, provided 
 * this copyright notice is retained and prominently displayed, along with
 * a note saying that the original programs are available from Radford Neal's
 * web page, and note is made of any changes made to the programs.  The
 * programs and documents are distributed without any warranty, express or
 * implied.  As the programs were written for research purposes only, they have
 * not been tested to the degree that would be advisable in any important
 * application.  All use of these programs is entirely at the user's own risk.
 */


/* DATA STRUCTURES USED TO STORE A DISTRIBUTION LIST.  Entries in the list 
 * (distrib_entry) are stored in an array (distrib->list[]). */

typedef struct distrib_entry
{ int num;			/* A positive number */
  double prop;			/* Proportion for this number */
} distrib_entry;

typedef struct distrib
{ struct distrib_entry *list;	/* The list of numbers and proportions */
  int size;			/* Number of entries in the list */
} distrib;


/* MACROS TO ACCESS ELEMENTS OF A DISTRIBUTION LIST.  Note that indexes for
   entries start at 0. */

#define distrib_num(d,i) \
  ((d)->list[i].num)		/* The number for the i'th entry */

#define distrib_prop(d,i) \
  ((d)->list[i].prop)		/* The i'th entry's proportion [probability] */

#define distrib_size(d) \
  ((d)->size)			/* The length of the list (integer) */


/* PROCEDURES FOR DISTRIBUTION LISTS. */

distrib *distrib_create	(char *);
void distrib_free (distrib *);

int distrib_max(distrib *);
