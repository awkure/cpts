D := cpts.h
F := -s -g -O3 -Os -Wno-sizeof-array-argument # -Wall -Wextra -Wshadow -Wno-missing-field-initializers

.DEFAULT_GOAL := a

a : b c 

help : ; @-echo "make [ a | b | c | r ]"

c :   ; $(RM) *.o *~ -f 
r : c ; $(RM) ${basename $D} 

%.o: %.c $D

b : $(D:.h=.o) ; $(CC) -o ${basename $<} $^ $F

.PHONY : a b c r help
