.PHONY: all clean

all: myshell

myshell: new.o linkedlist.o
	gcc -o myshell new.o linkedlist.o


myshell.o: new.c linkedlist.h
	gcc -c new.c


linkedlist.o: linkedlist.c
	gcc -c linkedlist.c

clean:
	rm -rf *.o myshell