main.out: key_event.o main.o
	clang -o main.out key_event.o main.o -lssl -lcrypto

key_event.o: key_event.c
	clang -c key_event.c

main.o: main.c key_event.h
	clang -c main.c

clean:
	rm -f *.o main.out
