all: makex

makex: main.o
	gcc main.o -o makex -lsense -lm

main.o: main.c
	gcc -c main.c

clean:
	rm -f makex *.o
