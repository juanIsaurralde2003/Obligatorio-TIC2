CFLAGS=-Wall -Wextra -std=gnu99 -ggdb 
LDLIBS=-lubsan
minish:	minish.o minish_func.o wrappers.o
minish.o:	minish.h wrappers.h
minish_func.o:	wrappers.h minish.h
wrappers.o:	wrappers.h