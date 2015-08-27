# Leave in the -O blast you!

CFLAGS= -O2 -Wall -Wextra
LDLIBS= -lncurses
FILES= airstrike.c etc.c midway.c movebombs.c moveships.c screen.c
OBJS= airstrike.o etc.o midway.o movebombs.o moveships.o screen.o
PROG= midway
JUNKFILES= ${PROG} fluff junk tags
PUB= /usr/public

all: ${PROG}

${PROG}: ${OBJS}
	${CC} ${OBJS} -o ${PROG} ${LDLIBS}

${OBJS}: midway.h

install: ${PUB}/${PROG} ${PUB}/${PROG}.txt

${PUB}/${PROG}: ${PROG}
	cp ${PROG} ${PUB}
	chmod 755 ${PUB}/${PROG}
	cp /dev/null ${PUB}/${PROG}.log
	chmod 666 ${PUB}/${PROG}.log

${PUB}/${PROG}.txt: README
	cp README ${PUB}/${PROG}.txt
	chmod 644 ${PUB}/${PROG}.txt

clean:
	${RM} ${OBJS} ${JUNKFILES}

.PHONY: all install clean
