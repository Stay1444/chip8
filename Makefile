CC=gcc

CFLAGS= -Wall -Wextra -Wswitch-enum -Wmissing-prototypes -Wconversion -Isrc -Ivendor/SDL2/include -Lvendor/SDL2/lib -lmingw32 -lSDL2main -lSDL2 

all: m

m:
	${CC} ./src/main.c ${CFLAGS} -o ./target/chip8.exe