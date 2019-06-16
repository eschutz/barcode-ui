SHELL=/bin/sh

MAINFILE=main
MAINOBJ=$(ODIR)/$(MAINFILE).o
SDIR=src
UIDIR=ui
ODIR=build
_OBJS=ui.o win.o util.o backend.o resources.o
OBJS=$(patsubst %,$(ODIR)/%,$(_OBJS))
_DEPS=ui.h win.h util.h backend.h error.h
DEPS=$(patsubst %,$(SDIR)/%,$(_DEPS))
LIBPATH=lib
BARCODELIB=barcode
INCLUDE_PATH=include
CFLAGS=-Wall -Wextra -Wno-unused-command-line-argument -g -rdynamic -I$(INCLUDE_PATH)
LIBS= -L$(LIBPATH) -l$(BARCODELIB)
ifeq ($(OS),Windows_NT)
	CC=bcc32x
else
	CC=clang
	CFLAGS:=`pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` `pkg-config --cflags gtk+-unix-print-3.0` `pkg-config --libs gtk+-unix-print-3.0` $(CFLAGS)
endif

$(ODIR)/%.o: $(DEPS) $(SDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SDIR)/$*.c -o $@

main: ui $(MAINOBJ) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(MAINOBJ) $(OBJS) $(LIBS)

ui:
	glib-compile-resources $(SDIR)/$(UIDIR)/barcode.gresource.xml --target=$(SDIR)/resources.c --sourcedir=$(SDIR)/$(UIDIR) --generate-source

dev:
	git clone "https://github.com/eschutz/libbarcode.git" &&	\
	make -C libbarcode lib &&					\
	cp libbarcode/lib/* lib &&					\
	ln -sr libbarcode/include/* include &&				\
	printf -- "-I/usr/local/include\n-Iinclude\n$(pkg-config --cflags gtk+-3.0 | tr ' ' '\n')" > .clang_complete

debug:
	valgrind --leak-check=yes --read-var-info=yes --track-origins=yes ./main

all: main

.PHONY: clean

clean:
	-$(RM) $(ODIR)/*.o main $(LIBNAME)
