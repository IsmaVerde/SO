CC=gcc -Wall

shell: shell.c lista.o listap2.o listaestatica.o
	$(CC) -o shell lista.o listap2.o listaestatica.o shell.c 

lista.o: lista.c lista.h
	$(CC) -c lista.c

listap2.o: listap2.c listap2.h
	$(CC) -c listap2.c

listaestatica.o: listaestatica.c listaestatica.h
	$(CC) -c listaestatica.c

