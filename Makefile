PROF=
CC=gcc
#DBG = -g
CFLAGS=-O2 -Wall -DLSB_FIRST $(PROF) $(DBG)
LDFLAGS=-lm $(PROF) $(DBG)
LDLIBS=-lreadline

LDFLAGS += $(shell sdl2-config --libs)
XCFLAGS = $(shell sdl2-config --cflags)


all: em gv

gv: gv.c
	gcc gv.c -o gv -Wall -O2 $(shell sdl-config --cflags) $(shell sdl-config --libs)

em: em.o cpufunc.o opcode0.o opcode1.o opcode2.o opcode3.o\
	opcode4.o opcode5.o opcode6.o opcode7.o opcode8.o opcode9.o\
	opcodeb.o opcodec.o opcoded.o opcodee.o video.o x.o expr.o \
	command.o dis68k.o z80.o fm.o z80dis.o gdb.o
	$(CC) -o $@ $^ $(LDFLAGS)   $(LDLIBS)   	
gdb.o: gdb.c em.h

cpufunc.o: cpufunc.c cpudefs.h readcpu.h cputbl.h 68kint.h

opcode1.o: opcode1.c cpudefs.h 68kint.h

opcode4.o: opcode4.c 68kint.h

opcode7.o: opcode7.c cpudefs.h 68kint.h

opcodeb.o: opcodeb.c cpudefs.h 68kint.h

opcodee.o: opcodee.c cpudefs.h 68kint.h

opcode2.o: opcode2.c cpudefs.h 68kint.h

opcode5.o: opcode5.c cpudefs.h 68kint.h

opcode8.o: opcode8.c cpudefs.h 68kint.h

opcodec.o: opcodec.c cpudefs.h 68kint.h

opcode0.o: opcode0.c cpudefs.h 68kint.h

opcode3.o: opcode3.c cpudefs.h 68kint.h

opcode6.o: opcode6.c cpudefs.h 68kint.h

opcode9.o: opcode9.c cpudefs.h 68kint.h

opcoded.o: opcoded.c cpudefs.h 68kint.h

em.o: em.c cpudefs.h M68000.h readcpu.h expr.h command.h em.h \
	z80dis.h 68kint.h

video.o: video.c em.h

x.o: x.c em.h
	$(CC) -c x.c -o x.o $(CFLAGS) $(XCFLAGS)

expr.o: expr.c expr.h

command.o: command.c command.h expr.h em.h cpudefs.h dis68k.h

dis68k.o: dis68k.c em.h dis68k.h

z80dis.o: z80dis.c z80dis.h

z80.o:      z80.c z80.h z80codes.h z80io.h z80daa.h

fm.o:	fm.c fm.h

clean:
	rm -f *.o em

test: em
	./em r/roadrash.bin
