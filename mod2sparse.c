/* MOD2SPARSE.C - Procedures for handling sparse mod2 matrices. */

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


/* NOTE:  See mod2sparse.html for documentation on these procedures. */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "alloc.h"
#include "intio.h"
#include "mod2sparse.h"


/* ALLOCATE AN ENTRY WITHIN A MATRIX.  This local procedure is used to
   allocate a new entry, representing a non-zero element, within a given
   matrix.  Entries in this matrix that were previously allocated and
   then freed are re-used.  If there are no such entries, a new block
   of entries is allocated. */

static mod2entry *alloc_entry
( mod2sparse *m
)
{ 
  mod2block *b;
  mod2entry *e;
  int k;

  if (m->next_free==0)
  { 
    b = chk_alloc (1, sizeof *b);

    b->next = m->blocks;
    m->blocks = b;

    for (k = 0; k<Mod2sparse_block; k++)
    { b->entry[k].left = m->next_free;
      m->next_free = &b->entry[k];
    }
  }

  e = m->next_free;
  m->next_free = e->left;

  e->pr = 0;
  e->lr = 0;

  return e;
}


/* ALLOCATE SPACE FOR A SPARSE MOD2 MATRIX.  */

mod2sparse *mod2sparse_allocate
( int n_rows, 		/* Number of rows in matrix */
  int n_cols		/* Number of columns in matrix */
)
{
  mod2sparse *m;
  mod2entry *e;
  int i, j;

  if (n_rows<=0 || n_cols<=0)
  { fprintf(stderr,"mod2sparse_allocate: Invalid number of rows or columns\n");
    exit(1);
  }

  m = chk_alloc (1, sizeof *m);

  m->n_rows = n_rows;
  m->n_cols = n_cols;

  m->rows = chk_alloc (n_rows, sizeof *m->rows);
  m->cols = chk_alloc (n_cols, sizeof *m->cols);

  m->blocks = 0;
  m->next_free = 0;

  for (i = 0; i<n_rows; i++)
  { e = &m->rows[i];
    e->left = e->right = e->up = e->down = e;
    e->row = e->col = -1;
  }

  for (j = 0; j<n_cols; j++)
  { e = &m->cols[j];
    e->left = e->right = e->up = e->down = e;
    e->row = e->col = -1;
  }

  return m;
}


/* FREE SPACE OCCUPIED BY A SPARSE MOD2 MATRIX. */

void mod2sparse_free
( mod2sparse *m		/* Matrix to free */
)
{ 
  mod2block *b;

  free(m->rows);
  free(m->cols);

  while (m->blocks!=0)
  { b = m->blocks;
    m->blocks = b->next;
    free(b);
  }
}


/* CLEAR A SPARSE MATRIX TO ALL ZEROS. */

void mod2sparse_clear
( mod2sparse *r
)
{
  mod2block *b;
  mod2entry *e;
  int i, j;

  for (i = 0; i<mod2sparse_rows(r); i++)
  { e = &r->rows[i];
    e->left = e->right = e->up = e->down = e;
  }

  for (j = 0; j<mod2sparse_cols(r); j++)
  { e = &r->cols[j];
    e->left = e->right = e->up = e->down = e;
  }

  while (r->blocks!=0)
  { b = r->blocks;
    r->blocks = b->next;
    free(b);
  }
}


/* COPY A SPARSE MATRIX. */

void mod2sparse_copy
( mod2sparse *m,	/* Matrix to copy */
  mod2sparse *r		/* Place to store copy of matrix */
)
{
  mod2entry *e, *f;
  int i;

  if (mod2sparse_rows(m)>mod2sparse_rows(r) 
   || mod2sparse_cols(m)>mod2sparse_cols(r))
  { fprintf(stderr,"mod2sparse_copy: Destination matrix is too small\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (i = 0; i<mod2sparse_rows(m); i++)
  {
    e = mod2sparse_first_in_row(m,i); 

    while (!mod2sparse_at_end(e))
    { f = mod2sparse_insert(r,e->row,e->col);
      f->lr = e->lr;
      f->pr = e->pr;
      e = mod2sparse_next_in_row(e);
    }
  }
}


/* COPY ROWS OF A SPARSE MOD2 MATRIX. */

void mod2sparse_copyrows
( mod2sparse *m,	/* Matrix to copy */
  mod2sparse *r,	/* Place to store copy of matrix */
  int *rows		/* Indexes of rows to copy, from 0 */
)
{ 
  mod2entry *e;
  int i;

  if (mod2sparse_cols(m)>mod2sparse_cols(r))
  { fprintf(stderr,
     "mod2sparse_copyrows: Destination matrix has fewer columns than source\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (i = 0; i<mod2sparse_rows(r); i++)
  { if (rows[i]<0 || rows[i]>=mod2sparse_rows(m))
    { fprintf(stderr,"mod2sparse_copyrows: Row index out of range\n");
      exit(1);
    }
    e = mod2sparse_first_in_row(m,rows[i]);
    while (!mod2sparse_at_end(e))
    { mod2sparse_insert(r,i,e->col);
      e = mod2sparse_next_in_row(e);
    }
  }
}


/* COPY COLUMNS OF A SPARSE MOD2 MATRIX. */

void mod2sparse_copycols
( mod2sparse *m,	/* Matrix to copy */
  mod2sparse *r,	/* Place to store copy of matrix */
  int *cols		/* Indexes of columns to copy, from 0 */
)
{ 
  mod2entry *e;
  int j;

  if (mod2sparse_rows(m)>mod2sparse_rows(r))
  { fprintf(stderr,
      "mod2sparse_copycols: Destination matrix has fewer rows than source\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (j = 0; j<mod2sparse_cols(r); j++)
  { if (cols[j]<0 || cols[j]>=mod2sparse_cols(m))
    { fprintf(stderr,"mod2sparse_copycols: Column index out of range\n");
      exit(1);
    }
    e = mod2sparse_first_in_col(m,cols[j]);
    while (!mod2sparse_at_end(e))
    { mod2sparse_insert(r,e->row,j);
      e = mod2sparse_next_in_col(e);
    }
  }
}


/* PRINT A SPARSE MOD2 MATRIX IN HUMAN-READABLE FORM. */

void mod2sparse_print
( FILE *f,
  mod2sparse *m
)
{ 
  int rdigits, cdigits;
  mod2entry *e;
  int i;

  rdigits = mod2sparse_rows(m)<=10 ? 1 
          : mod2sparse_rows(m)<=100 ? 2
          : mod2sparse_rows(m)<=1000 ? 3
          : mod2sparse_rows(m)<=10000 ? 4
          : mod2sparse_rows(m)<=100000 ? 5
          : 6;

  cdigits = mod2sparse_cols(m)<=10 ? 1 
          : mod2sparse_cols(m)<=100 ? 2
          : mod2sparse_cols(m)<=1000 ? 3
          : mod2sparse_cols(m)<=10000 ? 4
          : mod2sparse_cols(m)<=100000 ? 5
          : 6;

  for (i = 0; i<mod2sparse_rows(m); i++)
  { 
    fprintf(f,"%*d:",rdigits,i);

    e = mod2sparse_first_in_row(m,i);
    while (!mod2sparse_at_end(e))
    { fprintf(f," %*d",cdigits,mod2sparse_col(e));
      e = mod2sparse_next_in_row(e);
    }

    fprintf(f,"\n");
  }
}


/* WRITE A SPARSE MOD2 MATRIX TO A FILE IN MACHINE-READABLE FORM. */

int mod2sparse_write
( FILE *f,
  mod2sparse *m
)
{
  mod2entry *e;
  int i;

  intio_write(f,m->n_rows);
  if (ferror(f)) return 0;

  intio_write(f,m->n_cols);
  if (ferror(f)) return 0;

  for (i = 0; i<mod2sparse_rows(m); i++)
  { 
    e = mod2sparse_first_in_row(m,i);

    if (!mod2sparse_at_end(e))
    {
      intio_write (f, -(i+1));
      if (ferror(f)) return 0;

      while (!mod2sparse_at_end(e))
      { 
        intio_write (f, mod2sparse_col(e)+1);
        if (ferror(f)) return 0;

        e = mod2sparse_next_in_row(e);
      }
    }
  }

  intio_write(f,0);
  if (ferror(f)) return 0;

  return 1;
}


/* READ A SPARSE MOD2 MATRIX STORED IN MACHINE-READABLE FORM FROM A FILE. */

mod2sparse *mod2sparse_read
( FILE *f
)
{
  int n_rows, n_cols;
  mod2sparse *m;
  int v, row, col;

  n_rows = intio_read(f);
  if (feof(f) || ferror(f) || n_rows<=0) return 0;

  n_cols = intio_read(f);
  if (feof(f) || ferror(f) || n_cols<=0) return 0;

  m = mod2sparse_allocate(n_rows,n_cols);

  row = -1;

  for (;;)
  {
    v = intio_read(f);
    if (feof(f) || ferror(f)) break;

    if (v==0)
    { return m;
    }
    else if (v<0) 
    { row = -v-1;
      if (row>=n_rows) break;
    }
    else 
    { col = v-1;
      if (col>=n_cols) break;
      if (row==-1) break;
      mod2sparse_insert(m,row,col);
    }
  }

  /* Error if we get here. */

  mod2sparse_free(m);
  return 0;   
}


/* LOOK FOR AN ENTRY WITH GIVEN ROW AND COLUMN. */

mod2entry *mod2sparse_find
( mod2sparse *m,
  int row,
  int col
)
{ 
  mod2entry *re, *ce;

  if (row<0 || row>=mod2sparse_rows(m) || col<0 || col>=mod2sparse_cols(m))
  { fprintf(stderr,"mod2sparse_find: row or column index out of bounds\n");
    exit(1);
  }

  /* Check last entries in row and column. */

  re = mod2sparse_last_in_row(m,row);
  if (mod2sparse_at_end(re) || mod2sparse_col(re)<col) 
  { return 0;
  }
  if (mod2sparse_col(re)==col) 
  { return re;
  }

  ce = mod2sparse_last_in_col(m,col);
  if (mod2sparse_at_end(ce) || mod2sparse_row(ce)<row)
  { return 0;
  }
  if (mod2sparse_row(ce)==row)
  { return ce;
  }

  /* Search row and column in parallel, from the front. */

  re = mod2sparse_first_in_row(m,row);
  ce = mod2sparse_first_in_col(m,col);

  for (;;)
  { 
    if (mod2sparse_at_end(re) || mod2sparse_col(re)>col)
    { return 0;
    } 
    if (mod2sparse_col(re)==col) 
    { return re;
    }

    if (mod2sparse_at_end(ce) || mod2sparse_row(ce)>row)
    { return 0;
    } 
    if (mod2sparse_row(ce)==row)
    { return ce;
    }

    re = mod2sparse_next_in_row(re);
    ce = mod2sparse_next_in_col(ce);
  }
}


/* INSERT AN ENTRY WITH GIVEN ROW AND COLUMN. */

mod2entry *mod2sparse_insert
( mod2sparse *m,
  int row,
  int col
)
{
  mod2entry *re, *ce, *ne;

  if (row<0 || row>=mod2sparse_rows(m) || col<0 || col>=mod2sparse_cols(m))
  { fprintf(stderr,"mod2sparse_insert: row or column index out of bounds\n");
    exit(1);
  }

  /* Find old entry and return it, or allocate new entry and insert into row. */

  re = mod2sparse_last_in_row(m,row);

  if (!mod2sparse_at_end(re) && mod2sparse_col(re)==col) 
  { return re;
  }

  if (mod2sparse_at_end(re) || mod2sparse_col(re)<col) 
  { re = re->right;
  }
  else
  {
    re = mod2sparse_first_in_row(m,row);

    for (;;)
    { 
      if (!mod2sparse_at_end(re) && mod2sparse_col(re)==col) 
      { return re;
      }

      if (mod2sparse_at_end(re) || mod2sparse_col(re)>col)
      { break;
      } 

      re = mod2sparse_next_in_row(re);
    }
  }

  ne = alloc_entry(m);

  ne->row = row;
  ne->col = col;

  ne->left = re->left;
  ne->right = re;
  ne->left->right = ne;
  ne->right->left = ne;

  /* Insert new entry into column.  If we find an existing entry here,
     the matrix must be garbled, since we didn't find it in the row. */

  ce = mod2sparse_last_in_col(m,col);

  if (!mod2sparse_at_end(ce) && mod2sparse_row(ce)==row) 
  { fprintf(stderr,"mod2sparse_insert: Garbled matrix\n");
    exit(1);
  }

  if (mod2sparse_at_end(ce) || mod2sparse_row(ce)<row) 
  { ce = ce->down;
  }
  else
  {
    ce = mod2sparse_first_in_col(m,col);

    for (;;)
    { 
      if (!mod2sparse_at_end(ce) && mod2sparse_row(ce)==row) 
      { fprintf(stderr,"mod2sparse_insert: Garbled matrix\n");
        exit(1);
      }

      if (mod2sparse_at_end(ce) || mod2sparse_row(ce)>row)
      { break;
      } 

      ce = mod2sparse_next_in_col(ce);
    }
  }
    
  ne->up = ce->up;
  ne->down = ce;
  ne->up->down = ne;
  ne->down->up = ne;

  /* Return the new entry. */

  return ne;
}


/* DELETE AN ENTRY FROM A SPARSE MATRIX. */

void mod2sparse_delete
( mod2sparse *m,
  mod2entry *e
)
{ 
  if (e==0)
  { fprintf(stderr,"mod2sparse_delete: Trying to delete a null entry\n");
    exit(1);
  }

  if (e->row<0 || e->col<0)
  { fprintf(stderr,"mod2sparse_delete: Trying to delete a header entry\n");
    exit(1);
  }

  e->left->right = e->right;
  e->right->left = e->left;

  e->up->down = e->down;
  e->down->up = e->up;
 
  e->left = m->next_free;
  m->next_free = e;
}


/* TEST WHETHER TWO SPARSE MATRICES ARE EQUAL. */

int mod2sparse_equal
( mod2sparse *m1,
  mod2sparse *m2
)
{
  mod2entry *e1, *e2;
  int i;

  if (mod2sparse_rows(m1)!=mod2sparse_rows(m2) 
   || mod2sparse_cols(m1)!=mod2sparse_cols(m2))
  { fprintf(stderr,"mod2sparse_equal: Matrices have different dimensions\n");
    exit(1);
  }
  
  for (i = 0; i<mod2sparse_rows(m1); i++)
  { 
    e1 = mod2sparse_first_in_row(m1,i);
    e2 = mod2sparse_first_in_row(m2,i);

    while (!mod2sparse_at_end(e1) && !mod2sparse_at_end(e2))
    {  
      if (mod2sparse_col(e1)!=mod2sparse_col(e2))
      { return 0;
      }

      e1 = mod2sparse_next_in_row(e1);
      e2 = mod2sparse_next_in_row(e2);
    }

    if (!mod2sparse_at_end(e1) || !mod2sparse_at_end(e2)) 
    { return 0;
    }
  }

  return 1;
}


/* COMPUTE THE TRANSPOSE OF A SPARSE MOD2 MATRIX. */

void mod2sparse_transpose
( mod2sparse *m,	/* Matrix to compute transpose of (left unchanged) */
  mod2sparse *r		/* Result of transpose operation */
)
{
  mod2entry *e;
  int i;

  if (mod2sparse_rows(m)!=mod2sparse_cols(r) 
   || mod2sparse_cols(m)!=mod2sparse_rows(r))
  { fprintf(stderr,
     "mod2sparse_transpose: Matrices have incompatible dimensions\n");
    exit(1);
  }

  if (r==m)
  { fprintf(stderr, 
     "mod2sparse_transpose: Result matrix is the same as the operand\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (i = 0; i<mod2sparse_rows(m); i++)
  {
    e = mod2sparse_first_in_row(m,i);

    while (!mod2sparse_at_end(e))
    { mod2sparse_insert(r,mod2sparse_col(e),i);
      e = mod2sparse_next_in_row(e);
    }
  }
}


/* ADD TWO SPARSE MOD2 MATRICES. */

void mod2sparse_add
( mod2sparse *m1,	/* Left operand of add */
  mod2sparse *m2,	/* Right operand of add */
  mod2sparse *r		/* Place to store result of add */
)
{
  mod2entry *e1, *e2;
  int i;

  if (mod2sparse_rows(m1)!=mod2sparse_rows(r) 
   || mod2sparse_cols(m1)!=mod2sparse_cols(r) 
   || mod2sparse_rows(m2)!=mod2sparse_rows(r)
   || mod2sparse_cols(m2)!=mod2sparse_cols(r)) 
  { fprintf(stderr,"mod2sparse_add: Matrices have different dimensions\n");
    exit(1);
  }

  if (r==m1 || r==m2)
  { fprintf(stderr,
     "mod2sparse_add: Result matrix is the same as one of the operands\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (i = 0; i<mod2sparse_rows(r); i++)
  { 
    e1 = mod2sparse_first_in_row(m1,i);
    e2 = mod2sparse_first_in_row(m2,i);

    while (!mod2sparse_at_end(e1) && !mod2sparse_at_end(e2))
    { 
      if (mod2sparse_col(e1)==mod2sparse_col(e2))
      { e1 = mod2sparse_next_in_row(e1);
        e2 = mod2sparse_next_in_row(e2); 
      }

      else if (mod2sparse_col(e1)<mod2sparse_col(e2))
      { mod2sparse_insert(r,i,mod2sparse_col(e1));
        e1 = mod2sparse_next_in_row(e1);
      }

      else
      { mod2sparse_insert(r,i,mod2sparse_col(e2));
        e2 = mod2sparse_next_in_row(e2);       
      }
    }

    while (!mod2sparse_at_end(e1))
    { mod2sparse_insert(r,i,mod2sparse_col(e1));
      e1 = mod2sparse_next_in_row(e1);
    }

    while (!mod2sparse_at_end(e2))
    { mod2sparse_insert(r,i,mod2sparse_col(e2));
      e2 = mod2sparse_next_in_row(e2);
    }
  }
}


/* MULTIPLY TWO SPARSE MOD2 MATRICES. */

void mod2sparse_multiply 
( mod2sparse *m1, 	/* Left operand of multiply */
  mod2sparse *m2,	/* Right operand of multiply */
  mod2sparse *r		/* Place to store result of multiply */
)
{
  mod2entry *e1, *e2;
  int i, j, b;

  if (mod2sparse_cols(m1)!=mod2sparse_rows(m2) 
   || mod2sparse_rows(m1)!=mod2sparse_rows(r) 
   || mod2sparse_cols(m2)!=mod2sparse_cols(r))
  { fprintf (stderr,
      "mod2sparse_multiply: Matrices have incompatible dimensions\n");
    exit(1);
  }

  if (r==m1 || r==m2)
  { fprintf(stderr,
     "mod2sparse_multiply: Result matrix is the same as one of the operands\n");
    exit(1);
  }

  mod2sparse_clear(r);

  for (i = 0; i<mod2sparse_rows(m1); i++)
  { 
    if (mod2sparse_at_end(mod2sparse_first_in_row(m1,i))) 
    { continue;
    }

    for (j = 0; j<mod2sparse_cols(m2); j++)
    { 
      b = 0;

      e1 = mod2sparse_first_in_row(m1,i);
      e2 = mod2sparse_first_in_col(m2,j);

      while (!mod2sparse_at_end(e1) && !mod2sparse_at_end(e2))
      { 
        if (mod2sparse_col(e1)==mod2sparse_row(e2))
        { b ^= 1;
          e1 = mod2sparse_next_in_row(e1);
          e2 = mod2sparse_next_in_col(e2); 
        }

        else if (mod2sparse_col(e1)<mod2sparse_row(e2))
        { e1 = mod2sparse_next_in_row(e1);
        }

        else
        { e2 = mod2sparse_next_in_col(e2);       
        }
      }

      if (b)
      { mod2sparse_insert(r,i,j);
      }
    }
  }
}


/* MULTIPLY VECTOR BY SPARSE MATRIX. */

void mod2sparse_mulvec
( mod2sparse *m,	/* The sparse matrix, with M rows and N columns */
  char *u,		/* The input vector, N long */
  char *v		/* Place to store the result, M long */
)
{
  mod2entry *e;
  int M, N;
  int i, j;

  M = mod2sparse_rows(m);
  N = mod2sparse_cols(m);

  for (i = 0; i<M; i++) v[i] = 0;

  for (j = 0; j<N; j++)
  { if (u[j])
    { for (e = mod2sparse_first_in_col(m,j);
           !mod2sparse_at_end(e);
           e = mod2sparse_next_in_col(e))
      { v[mod2sparse_row(e)] ^= 1;
      }
    }
  }
}


/* COUNT ENTRIES IN A ROW. */

int mod2sparse_count_row
( mod2sparse *m,
  int row
)
{
  mod2entry *e;
  int count;

  if (row<0 || row>=mod2sparse_rows(m))
  { fprintf(stderr,"mod2sparse_count_row: row index out of bounds\n");
    exit(1);
  }

  count = 0;

  for (e = mod2sparse_first_in_row(m,row);
       !mod2sparse_at_end(e);
       e = mod2sparse_next_in_row(e))
  { count += 1;
  }

  return count;
}


/* COUNT ENTRIES IN A COLUMN. */

int mod2sparse_count_col
( mod2sparse *m,
  int col
)
{
  mod2entry *e;
  int count;

  if (col<0 || col>=mod2sparse_cols(m))
  { fprintf(stderr,"mod2sparse_count_col: column index out of bounds\n");
    exit(1);
  }

  count = 0;

  for (e = mod2sparse_first_in_col(m,col);
       !mod2sparse_at_end(e);
       e = mod2sparse_next_in_col(e))
  { count += 1;
  }

  return count;
}


/* ADD TO A ROW. */

void mod2sparse_add_row
( mod2sparse *m1,	/* Matrix containing row to add to */
  int row1,		/* Index in this matrix of row to add to */
  mod2sparse *m2,	/* Matrix containing row to add from */
  int row2		/* Index in this matrix of row to add from */
)
{
  mod2entry *f1, *f2, *ft;

  if (mod2sparse_cols(m1)<mod2sparse_cols(m2))
  { fprintf (stderr,
     "mod2sparse_add_row: row added to is shorter than row added from\n");
    exit(1);
  }

  if (row1<0 || row1>=mod2sparse_rows(m1) 
   || row2<0 || row2>=mod2sparse_rows(m2))
  { fprintf (stderr,"mod2sparse_add_row: row index out of range\n");
    exit(1);
  }

  f1 = mod2sparse_first_in_row(m1,row1);
  f2 = mod2sparse_first_in_row(m2,row2);

  while (!mod2sparse_at_end(f1) && !mod2sparse_at_end(f2))
  { if (mod2sparse_col(f1)>mod2sparse_col(f2))
    { mod2sparse_insert(m1,row1,mod2sparse_col(f2));
      f2 = mod2sparse_next_in_row(f2);
    }
    else
    { ft = mod2sparse_next_in_row(f1);  
      if (mod2sparse_col(f1)==mod2sparse_col(f2))
      { mod2sparse_delete(m1,f1);
        f2 = mod2sparse_next_in_row(f2);
      }
      f1 = ft;
    }
  }

  while (!mod2sparse_at_end(f2))
  { mod2sparse_insert(m1,row1,mod2sparse_col(f2));
    f2 = mod2sparse_next_in_row(f2);
  }
}


/* ADD TO A COLUMN. */

void mod2sparse_add_col
( mod2sparse *m1,	/* Matrix containing column to add to */
  int col1,		/* Index in this matrix of column to add to */
  mod2sparse *m2,	/* Matrix containing column to add from */
  int col2		/* Index in this matrix of column to add from */
)
{
  mod2entry *f1, *f2, *ft;

  if (mod2sparse_rows(m1)<mod2sparse_rows(m2))
  { fprintf (stderr,
     "mod2sparse_add_col: Column added to is shorter than column added from\n");
    exit(1);
  }

  if (col1<0 || col1>=mod2sparse_cols(m1) 
   || col2<0 || col2>=mod2sparse_cols(m2))
  { fprintf (stderr,"mod2sparse_add_col: Column index out of range\n");
    exit(1);
  }

  f1 = mod2sparse_first_in_col(m1,col1);
  f2 = mod2sparse_first_in_col(m2,col2);

  while (!mod2sparse_at_end(f1) && !mod2sparse_at_end(f2))
  { if (mod2sparse_row(f1)>mod2sparse_row(f2))
    { mod2sparse_insert(m1,mod2sparse_row(f2),col1);
      f2 = mod2sparse_next_in_col(f2);
    }
    else
    { ft = mod2sparse_next_in_col(f1);
      if (mod2sparse_row(f1)==mod2sparse_row(f2))
      { mod2sparse_delete(m1,f1);
        f2 = mod2sparse_next_in_col(f2);
      }
      f1 = ft;
    }
  }

  while (!mod2sparse_at_end(f2))
  { mod2sparse_insert(m1,mod2sparse_row(f2),col1);
    f2 = mod2sparse_next_in_col(f2);
  }
}


/* FIND AN LU DECOMPOSITION OF A SPARSE MATRIX. */

int mod2sparse_decomp
( mod2sparse *A,	/* Input matrix, M by N */
  int K,		/* Size of sub-matrix to find LU decomposition of */
  mod2sparse *L,	/* Matrix in which L is stored, M by K */
  mod2sparse *U,	/* Matrix in which U is stored, K by N */
  int *rows,		/* Array where row indexes are stored, M long */
  int *cols,		/* Array where column indexes are stored, N long */
  mod2sparse_strategy strategy, /* Strategy to follow in picking rows/columns */
  int abandon_number,	/* Number of columns to abandon at some point */
  int abandon_when	/* When to abandon these columns */
)
{  
  int *rinv, *cinv, *acnt, *rcnt;
  mod2sparse *B;
  int M, N;

  mod2entry *e, *f, *fn, *e2;
  int i, j, k, cc, cc2, cc3, cr2, pr;
  int found, nnf;

  M = mod2sparse_rows(A);
  N = mod2sparse_cols(A);

  if (mod2sparse_cols(L)!=K || mod2sparse_rows(L)!=M
   || mod2sparse_cols(U)!=N || mod2sparse_rows(U)!=K)
  { fprintf (stderr,
      "mod2sparse_decomp: Matrices have incompatible dimensions\n");
    exit(1);
  }

  if (abandon_number>N-K)
  { fprintf(stderr,"Trying to abandon more columns than allowed\n");
    exit(1);
  }

  rinv = chk_alloc (M, sizeof *rinv);
  cinv = chk_alloc (N, sizeof *cinv);

  if (abandon_number>0)
  { acnt = chk_alloc (M+1, sizeof *acnt);
  }

  if (strategy==Mod2sparse_minprod)
  { rcnt = chk_alloc (M, sizeof *rcnt);
  }

  mod2sparse_clear(L);
  mod2sparse_clear(U);

  /* Copy A to B.  B will be modified, then discarded. */

  B = mod2sparse_allocate(M,N);
  mod2sparse_copy(A,B);

  /* Count 1s in rows of B, if using minprod strategy. */

  if (strategy==Mod2sparse_minprod)
  { for (i = 0; i<M; i++) 
    { rcnt[i] = mod2sparse_count_row(B,i);
    }
  }

  /* Set up initial row and column choices. */

  for (i = 0; i<M; i++) rows[i] = rinv[i] = i;
  for (j = 0; j<N; j++) cols[j] = cinv[j] = j;
 
  /* Find L and U one column at a time. */

  nnf = 0;

  for (i = 0; i<K; i++)
  { 
    /* Choose the next row and column of B. */

    switch (strategy)
    {
      case Mod2sparse_first: 
      { 
        found = 0;

        for (k = i; k<N; k++)
        { e = mod2sparse_first_in_col(B,cols[k]);
          while (!mod2sparse_at_end(e))
          { if (rinv[mod2sparse_row(e)]>=i)
            { found = 1;
              goto out_first;
            }
            e = mod2sparse_next_in_col(e);
          }
        }

      out_first:
        break;
      }

      case Mod2sparse_mincol:
      { 
        found = 0;

        for (j = i; j<N; j++)
        { cc2 = mod2sparse_count_col(B,cols[j]);
          if (!found || cc2<cc)
          { e2 = mod2sparse_first_in_col(B,cols[j]);
            while (!mod2sparse_at_end(e2))
            { if (rinv[mod2sparse_row(e2)]>=i)
              { found = 1;
                cc = cc2;
                e = e2;
                k = j;
                break;
              }
              e2 = mod2sparse_next_in_col(e2);
            }
          }
        }

        break;
      }

      case Mod2sparse_minprod:
      { 
        found = 0;

        for (j = i; j<N; j++)
        { cc2 = mod2sparse_count_col(B,cols[j]);
          e2 = mod2sparse_first_in_col(B,cols[j]);
          while (!mod2sparse_at_end(e2))
          { if (rinv[mod2sparse_row(e2)]>=i)
            { cr2 = rcnt[mod2sparse_row(e2)];
              if (!found || cc2==1 || (cc2-1)*(cr2-1)<pr)
              { found = 1;
                pr = cc2==1 ? 0 : (cc2-1)*(cr2-1);
                e = e2;
                k = j;
              }
            }
            e2 = mod2sparse_next_in_col(e2);
          }
        }

        break;
      }

      default:
      { fprintf(stderr,"mod2sparse_decomp: Unknown stategy\n");
        exit(1);
      }
    }

    if (!found) 
    { nnf += 1;
    }

    /* Update 'rows' and 'cols'.  Looks at 'k' and 'e' found above. */

    if (found)
    { 
      if (cinv[mod2sparse_col(e)]!=k) abort();

      cols[k] = cols[i];
      cols[i] = mod2sparse_col(e);

      cinv[cols[k]] = k;
      cinv[cols[i]] = i;

      k = rinv[mod2sparse_row(e)];

      if (k<i) abort();

      rows[k] = rows[i];
      rows[i] = mod2sparse_row(e);

      rinv[rows[k]] = k;
      rinv[rows[i]] = i;
    }

    /* Update L, U, and B. */

    f = mod2sparse_first_in_col(B,cols[i]); 

    while (!mod2sparse_at_end(f))
    { 
      fn = mod2sparse_next_in_col(f);
      k = mod2sparse_row(f);

      if (rinv[k]>i)
      { mod2sparse_add_row(B,k,B,mod2sparse_row(e));
        if (strategy==Mod2sparse_minprod) 
        { rcnt[k] = mod2sparse_count_row(B,k);
        }
        mod2sparse_insert(L,k,i);
      }
      else if (rinv[k]<i)
      { mod2sparse_insert(U,rinv[k],cols[i]);
      }
      else
      { mod2sparse_insert(L,k,i);
        mod2sparse_insert(U,i,cols[i]);
      }

      f = fn;
    }

    /* Get rid of all entries in the current column of B, just to save space. */

    for (;;)
    { f = mod2sparse_first_in_col(B,cols[i]);
      if (mod2sparse_at_end(f)) break;
      mod2sparse_delete(B,f);
    }

    /* Abandon columns of B with lots of entries if it's time for that. */

    if (abandon_number>0 && i==abandon_when)
    { 
      for (k = 0; k<M+1; k++) 
      { acnt[k] = 0;
      }
      for (j = 0; j<N; j++) 
      { k = mod2sparse_count_col(B,j);
        acnt[k] += 1;
      }

      cc = abandon_number;
      k = M;
      while (acnt[k]<cc)
      { cc -= acnt[k];
        k -= 1;
        if (k<0) abort();
      }

      cc2 = 0;
      for (j = 0; j<N; j++)
      { cc3 = mod2sparse_count_col(B,j);
        if (cc3>k || cc3==k && cc>0)
        { if (cc3==k) cc -= 1;
          for (;;)
          { f = mod2sparse_first_in_col(B,j);
            if (mod2sparse_at_end(f)) break;
            mod2sparse_delete(B,f);
          }
          cc2 += 1;
        }
      }

      if (cc2!=abandon_number) abort();

      if (strategy==Mod2sparse_minprod)
      { for (j = 0; j<M; j++) 
        { rcnt[j] = mod2sparse_count_row(B,j);
        }
      }
    }
  }

  /* Get rid of all entries in the rows of L past row K, after reordering. */

  for (i = K; i<M; i++)
  { for (;;)
    { f = mod2sparse_first_in_row(L,rows[i]);
      if (mod2sparse_at_end(f)) break;
      mod2sparse_delete(L,f);
    }
  }

  mod2sparse_free(B);
  free(rinv);
  free(cinv);
  if (strategy==Mod2sparse_minprod) free(rcnt);
  if (abandon_number>0) free(acnt);

  return nnf;
}


/* SOLVE A LOWER-TRIANGULAR SYSTEM BY FORWARD SUBSTITUTION. */

int mod2sparse_forward_sub
( mod2sparse *L,	/* Matrix that is lower triangular after reordering */
  int *rows,		/* Array of indexes (from 0) of rows for new order */
  char *x,		/* Vector on right of equation, also reordered */
  char *y		/* Place to store solution */
)
{
  int K, i, j, ii, b, d;
  mod2entry *e;

  K = mod2sparse_cols(L);

  /* Make sure that L is lower-triangular, after row re-ordering. */

  for (i = 0; i<K; i++)
  { ii = rows ? rows[i] : i;
    e = mod2sparse_last_in_row(L,ii);
    if (!mod2sparse_at_end(e) && mod2sparse_col(e)>i)
    { fprintf(stderr,
        "mod2sparse_forward_sub: Matrix is not lower-triangular\n");
      exit(1);
    }
  }

  /* Solve system by forward substitution. */

  for (i = 0; i<K; i++)
  { 
    ii = rows ? rows[i] : i;

    /* Look at bits in this row, forming inner product with partial 
       solution, and seeing if the diagonal is 1. */

    d = 0;
    b = 0;

    for (e = mod2sparse_first_in_row(L,ii); 
         !mod2sparse_at_end(e);
         e = mod2sparse_next_in_row(e))
    { 
      j = mod2sparse_col(e);

      if (j==i)
      { d = 1;
      }
      else
      { b ^= y[j];
      }
    }

    /* Check for no solution if the diagonal isn't 1. */

    if (!d && b!=x[ii]) 
    { return 0;
    }

    /* Set bit of solution, zero if arbitrary. */

    y[i] = b^x[ii];
  }

  return 1;
}


/* SOLVE AN UPPER-TRIANGULAR SYSTEM BY BACKWARD SUBSTITUTION. */

int mod2sparse_backward_sub
( mod2sparse *U,	/* Matrix that is upper triangular after reordering */
  int *cols,		/* Array of indexes (from 0) of columns for new order */
  char *y,		/* Vector on right of equation */
  char *z		/* Place to store solution, also reordered */
)
{
  int K, i, j, ii, b, d;
  mod2entry *e;

  K = mod2sparse_rows(U);

  /* Make sure that U is upper-triangular, after column re-ordering. */

  for (i = 0; i<K; i++)
  { ii = cols ? cols[i] : i;
    e = mod2sparse_last_in_col(U,ii);
    if (!mod2sparse_at_end(e) && mod2sparse_row(e)>i)
    { fprintf(stderr,
        "mod2sparse_backward_sub: Matrix is not upper-triangular\n");
      exit(1);
    }
  }

  /* Solve system by backward substitution. */

  for (i = K-1; i>=0; i--)
  { 
    ii = cols ? cols[i] : i;

    /* Look at bits in this row, forming inner product with partial 
       solution, and seeing if the diagonal is 1. */

    d = 0;
    b = 0;

    for (e = mod2sparse_first_in_row(U,i); 
         !mod2sparse_at_end(e);
         e = mod2sparse_next_in_row(e))
    { 
      j = mod2sparse_col(e);

      if (j==ii)
      { d = 1;
      }
      else
      { b ^= z[j];
      }
    }

    /* Check for no solution if the diagonal isn't 1. */

    if (!d && b!=y[i]) 
    { return 0;
    }

    /* Set bit of solution, zero if arbitrary. */

    z[ii] = b^y[i];
  }

  return 1;
}
