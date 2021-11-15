CC=gcc -Wall
all: shell listar info
shell: shell.c lista.o
	$(CC) -o shell lista.o shell.c 

listar: listar.c 
	$(CC) -o listar listar.c

info: info.c
	$(CC) -o info info.c


lista.o: lista.c lista.h
	$(CC) -c lista.c


clean:
	rm p0 lista.p
