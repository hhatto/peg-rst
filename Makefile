uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
ifneq (,$(findstring MINGW,$(uname_S)))
	X = .exe
endif

export X

LIBNAME=librst
PROGRAM=rst$(X)
CFLAGS ?= -Wall -O3 -ansi -D_GNU_SOURCE # -flto for newer GCC versions
OBJS=rst_parser.o rst_output.o rst_lib.o utility_functions.o parsing_functions.o odf.o
PEGDIR=peg-0.1.18
LEG=$(PEGDIR)/leg$(X)
PKG_CONFIG = pkg-config

ALL: $(PROGRAM) $(LIBNAME).a

$(LEG): $(PEGDIR)
	CC=gcc make -C $(PEGDIR)

%.o: %.c rst_peg.h
	$(CC) -c `$(PKG_CONFIG) --cflags glib-2.0` $(CFLAGS) -o $@ $<

$(PROGRAM): rst.c $(OBJS)
	$(CC) `$(PKG_CONFIG) --cflags glib-2.0` $(CFLAGS) -o $@ $< $(OBJS) `$(PKG_CONFIG) --libs glib-2.0`

$(LIBNAME).a: $(OBJS)
	ar rcs $(LIBNAME).a $(OBJS)

$(LIBNAME).so: $(OBJS)
	$(CC) -shared $(OBJS) -o $(LIBNAME).so

rst_parser.c: rst_parser.leg $(LEG) rst_peg.h parsing_functions.c utility_functions.c
	$(LEG) -o $@ $<

.PHONY: clean test

clean:
	rm -f rst_parser.c $(PROGRAM) $(OBJS) $(LIBNAME).*

distclean: clean
	make -C $(PEGDIR) spotless clean

leak-check: $(PROGRAM)
	valgrind --leak-check=full ./$(PROGRAM) README.rst

test: $(PROGRAM)
	./$(PROGRAM) README.rst
