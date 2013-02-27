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
 make-gen print-gen rand-src encode transmit decode extract verify 

progs: $(progs)

tests = mod2dense-test mod2sparse-test mod2convert-test rand-test

tests: $(tests)

CFLAGS += -Wall -Wno-maybe-uninitialized
CFLAGS += -O2
CFLAGS += -g
LOADLIBES += -lm
LDFLAGS += -g

RAND_FILE = $$(pwd)/randfile
rand.o: CPPFLAGS+=-DRAND_FILE=\"$(RAND_FILE)\"

make-pchk: make-pchk.o mod2sparse.o mod2dense.o mod2convert.o rcode.o	\
  alloc.o intio.o open.o
alist-to-pchk: alist-to-pchk.o mod2sparse.o mod2dense.o mod2convert.o	\
  rcode.o alloc.o intio.o open.o
pchk-to-alist: pchk-to-alist.o mod2sparse.o mod2dense.o mod2convert.o	\
  rcode.o alloc.o intio.o open.o
make-ldpc: make-ldpc.o mod2sparse.o mod2dense.o mod2convert.o rcode.o	\
  rand.o alloc.o intio.o open.o distrib.o
print-pchk: print-pchk.o mod2sparse.o mod2dense.o mod2convert.o rcode.o	\
  rand.o alloc.o intio.o open.o
make-gen: make-gen.o mod2sparse.o mod2dense.o mod2convert.o rcode.o	\
  alloc.o intio.o open.o
print-gen: print-gen.o mod2sparse.o mod2dense.o mod2convert.o rcode.o	\
  rand.o alloc.o intio.o open.o
rand-src: rand-src.o rand.o open.o
encode: encode.o mod2sparse.o mod2dense.o mod2convert.o enc.o rcode.o	\
  rand.o alloc.o intio.o blockio.o open.o
transmit: transmit.o channel.o rand.o open.o
decode: decode.o channel.o mod2sparse.o mod2dense.o mod2convert.o	\
  enc.o check.o rcode.o rand.o alloc.o intio.o blockio.o dec.o open.o
extract: extract.o mod2sparse.o mod2dense.o mod2convert.o rcode.o	\
  alloc.o intio.o blockio.o open.o
verify: verify.o mod2sparse.o mod2dense.o mod2convert.o check.o rcode.o	\
  alloc.o intio.o blockio.o open.o

mod2dense-test: mod2dense-test.o mod2dense.o alloc.o intio.o
mod2sparse-test: mod2sparse-test.o mod2sparse.o alloc.o intio.o
mod2convert-test: mod2convert-test.o mod2convert.o mod2dense.o		\
  mod2sparse.o alloc.o intio.o rand.o open.o
rand-test: rand-test.o rand.o

# CLEAN UP ALL PROGRAMS AND REMOVE ALL FILES PRODUCED BY TESTS AND EXAMPLES.

clean:
	rm -f	core *.o *.exe ex-*.* test-file \
		$(progs) $(tests)

.PHONY: all progs tests clean
