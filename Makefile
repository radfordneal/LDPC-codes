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

all: progs tests

progs = make-pchk alist-to-pchk pchk-to-alist make-ldpc print-pchk	\
 make-gen print-gen rand-src encode transmit decode extract verify	\
 extract_systematic

progs: $(progs)

tests = mod2dense-test mod2sparse-test mod2convert-test rand-test

tests: $(tests)

CFLAGS += -Wall
CFLAGS += -O2
CFLAGS += -g
LOADLIBES += -lm

ofiles = alloc.o blockio.o channel.o check.o dec.o distrib.o enc.o	\
  intio.o mod2convert.o mod2dense.o mod2sparse.o open.o rand.o rcode.o

RAND_FILE = $$(pwd)/randfile
rand.o: CPPFLAGS+=-DRAND_FILE=\"$(RAND_FILE)\"

$(progs) $(tests): $(ofiles)

# CLEAN UP ALL PROGRAMS AND REMOVE ALL FILES PRODUCED BY TESTS AND EXAMPLES.

clean:
	rm -f	core *.o *.exe ex-*.* test-file \
		$(progs) $(tests)

.PHONY: all progs tests clean
