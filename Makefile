# MAKEFILE FOR LDPC PROGRAMS & ASSOCIATED UTILITIES.

# Copyright (c) 1995-2012 by Radford M. Neal.
#
# Permission is granted for anyone to copy, use, modify, and distribute
# these programs and accompanying documents for any purpose, provided
# this copyright notice is retained and prominently displayed, and note
# is made of any changes made to these programs.  These programs and
# documents are distributed without any warranty, express or implied.
# As the programs were written for research purposes only, they have not
# been tested to the degree that would be advisable in any important
# application.  All use of these programs is entirely at the user's own
# risk.


# NOTE:  The natural random numbers in "randfile" are accessed by the
# 'rand' module via a path to this directory.  Change the definition of
# RAND_FILE in the compilation command for rand.c below if this is not
# appropriate.

# NOTE:  This makefile is trivial, simply recompiling everything from
# scratch every time.  Since this takes only about 5 seconds on a modern
# PC, there's no point in putting in dependency-based rules, which just 
# make things more complex and error-prone.


COMPILE = cc -c -O    # Command to compile a module from .c to .o
LINK =    cc          # Command to link a program


# MAKE ALL THE MAIN PROGRAMS.  First makes the modules used.

progs:	modules
	$(COMPILE) make-pchk.c
	$(LINK) make-pchk.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o alloc.o intio.o open.o -lm -o make-pchk
	$(COMPILE) alist-to-pchk.c
	$(LINK) alist-to-pchk.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o alloc.o intio.o open.o -lm -o alist-to-pchk
	$(COMPILE) pchk-to-alist.c
	$(LINK) pchk-to-alist.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o alloc.o intio.o open.o -lm -o pchk-to-alist
	$(COMPILE) make-ldpc.c
	$(LINK) make-ldpc.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o rand.o alloc.o intio.o open.o distrib.o -lm -o make-ldpc
	$(COMPILE) print-pchk.c
	$(LINK) print-pchk.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o rand.o alloc.o intio.o open.o -lm -o print-pchk
	$(COMPILE) make-gen.c
	$(LINK) make-gen.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o alloc.o intio.o open.o -lm -o make-gen
	$(COMPILE) print-gen.c
	$(LINK) print-gen.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o rand.o alloc.o intio.o open.o -lm -o print-gen
	$(COMPILE) rand-src.c
	$(LINK) rand-src.o rand.o open.o -lm -o rand-src
	$(COMPILE) encode.c
	$(LINK) encode.o mod2sparse.o mod2dense.o mod2convert.o \
	   enc.o rcode.o rand.o alloc.o intio.o blockio.o open.o -lm -o encode
	$(COMPILE) transmit.c
	$(LINK) transmit.o channel.o rand.o open.o -lm -o transmit
	$(COMPILE) decode.c
	$(LINK) decode.o channel.o mod2sparse.o mod2dense.o mod2convert.o \
	   enc.o check.o \
	   rcode.o rand.o alloc.o intio.o blockio.o dec.o open.o -lm -o decode
	$(COMPILE) extract.c
	$(LINK) extract.o mod2sparse.o mod2dense.o mod2convert.o \
	   rcode.o alloc.o intio.o blockio.o open.o -lm -o extract
	$(COMPILE) verify.c
	$(LINK) verify.o mod2sparse.o mod2dense.o mod2convert.o check.o \
	   rcode.o alloc.o intio.o blockio.o open.o -lm -o verify


# MAKE THE TEST PROGRAMS.  First makes the modules used.

tests:	modules
	$(COMPILE) mod2dense-test.c
	$(LINK) mod2dense-test.o mod2dense.o alloc.o intio.o \
	  -lm -o mod2dense-test
	$(COMPILE) mod2sparse-test.c
	$(LINK) mod2sparse-test.o mod2sparse.o alloc.o intio.o \
	  -lm -o mod2sparse-test
	$(COMPILE) mod2convert-test.c
	$(LINK) mod2convert-test.o mod2convert.o mod2dense.o mod2sparse.o \
	  alloc.o intio.o rand.o open.o -lm -o mod2convert-test
	$(COMPILE) rand-test.c
	$(LINK) rand-test.o rand.o -lm -o rand-test


# MAKE THE MODULES USED BY THE PROGRAMS.

modules:
	$(COMPILE) rcode.c
	$(COMPILE) channel.c
	$(COMPILE) dec.c
	$(COMPILE) enc.c
	$(COMPILE) alloc.c
	$(COMPILE) intio.c
	$(COMPILE) blockio.c
	$(COMPILE) check.c
	$(COMPILE) open.c
	$(COMPILE) mod2dense.c
	$(COMPILE) mod2sparse.c
	$(COMPILE) mod2convert.c
	$(COMPILE) distrib.c
	$(COMPILE) -DRAND_FILE=\"`pwd`/randfile\" rand.c


# CLEAN UP ALL PROGRAMS AND REMOVE ALL FILES PRODUCED BY TESTS AND EXAMPLES.

clean:
	rm -f	core *.o *.exe ex-*.* test-file \
		make-pchk alist-to-pchk pchk-to-alist \
		make-ldpc print-pchk make-gen print-gen \
		rand-src encode transmit decode extract verify \
		mod2dense-test mod2sparse-test mod2convert-test rand-test
