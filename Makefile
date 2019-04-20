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
CFLAGS=-Wall -Wextra -g -lgs -I /usr/local/Cellar/ghostscript/9.26_1/include/ghostscript -L$(LIBPATH) -l$(BARCODELIB) -I$(BARCODELIB)
ifeq ($(OS),Windows_NT)
	CC=bcc32x
else
	CC=clang
	CFLAGS:=`pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` $(CFLAGS)
endif

$(ODIR)/%.o: $(DEPS) $(SDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SDIR)/$*.c -o $@

main: ui $(MAINOBJ) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(MAINOBJ) $(OBJS) $(LIBS)

ui:
	glib-compile-resources $(SDIR)/$(UIDIR)/barcode.gresource.xml --target=$(SDIR)/resources.c --sourcedir=$(SDIR)/$(UIDIR) --generate-source

debug: clean
	docker run -v $(PWD):/home/ valgrind-docker bash -c "cd home; make clean; make main; valgrind --leak-check=yes --read-var-info=yes --track-origins=yes ./main"

all: main

.PHONY: clean

clean:
	-$(RM) $(ODIR)/*.o main $(LIBNAME)
