N  = cpts

A ?= $(if $(filter $(shell uname -m),x86_64), amd64, i386)
O := $(A)/platform.o $(A)/start.s

F := -s -g -O3 -Os -Wno-sizeof-array-argument -Wall -Wextra -Wshadow -Wno-missing-field-initializers -fno-asynchronous-unwind-tables -fno-unwind-tables -Wl,--gc-sections -Wa,--noexecstack -fdata-sections -fno-builtin -nostdlib

.DEFAULT_GOAL := a

a : b s c 

h : ; @-echo "make [ a | b | c | r ]"

c :   ; $(RM) *.o $A/*.o *~
r : c ; $(RM) $N

%.o: %.c $N.h

b : $O ; $(CC) -o ${basename $<} $^ $F
s : $O ; strip -R .comment $N && strip -R .note.gnu.build-id $N;

.PHONY : a b c r help
