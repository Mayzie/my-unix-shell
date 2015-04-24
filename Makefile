# Makefile for MyShell
# Daniel Collis - 430133523

CC=gcc					# Compiler to use
DEBUG=-DEBUG -g -Wall			# Debug compile options
RELEASE=-O3				# Release compile options
CFLAGS=$(RELEASE)			# Which mode to compile in? $(DEBUG) or $(RELEASE)?

DEPS = myshell.h utlity.h intrnlcmd.h
OBJ = myshell.o utility.o intrnlcmd.o

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

myshell:	$(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/myshell $^

# Remove all object and binary files
clean:
	rm -f *.o
	rm -f bin/myshell
