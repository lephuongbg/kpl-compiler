CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: kplc

kplc: main.o parser.o scanner.o reader.o charcode.o token.o error.o symtab.o semantics.o debug.o
	${CC} main.o parser.o scanner.o reader.o charcode.o token.o error.o symtab.o semantics.o debug.o -o kplc

main.o: main.c
	${CC} ${CFLAGS} main.c

scanner.o: scanner.c
	${CC} ${CFLAGS} scanner.c

parser.o: parser.c
	${CC} ${CFLAGS} parser.c

reader.o: reader.c
	${CC} ${CFLAGS} reader.c

charcode.o: charcode.c
	${CC} ${CFLAGS} charcode.c

token.o: token.c
	${CC} ${CFLAGS} token.c

error.o: error.c
	${CC} ${CFLAGS} error.c

symtab.o: symtab.c
	${CC} ${CFLAGS} symtab.c

semantics.o: semantics.c
	${CC} ${CFLAGS} semantics.c

debug.o: debug.c
	${CC} ${CFLAGS} debug.c

clean:
	rm -f *.o *~

