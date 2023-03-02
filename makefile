############################################################################
# Seth Cram
# 11/12/2021
# hw3: Kernel Simulator
# File: makefile
#
# notes:
#
# description:
#
############################################################################

cc = g++
cflags = -Wall -g
ofiles = ksim_cmds.o ksim.o ksim_step.o ksim_query.o
tgts = ksim $(ofiles) #ksim_cmds.o ksim.o ksim_step.o ksim_query.o
deps = ksim.h

all:
	make ksim

%.o: %.c $(deps)
	$(cc) -c -o $@ $< $(cflags)

ksim: $(ofiles)
	$(cc) -o ksim $(ofiles)

#included bc sometimes header not recompiled correctly
header: ksim.h
	g++ ksim.h

clean:
	rm $(tgts)
