# Leave in the -O blast you!

CC= cc
CFLAGS= -O
FILES= airstrike.c etc.c midway.c movebombs.c moveships.c screen.c
OBJS= airstrike.o etc.o midway.o movebombs.o moveships.o screen.o
OTHERFILES= makefile externs.h globals.h Midway.doc
JUNKFILES= Midway fluff junk arch tags
LIBS= -lcurses -ltermlib 
PUB= /usr/public

.c.o:; ${CC} ${CFLAGS} -c $<

Midway: ${OBJS}
	cc ${OBJS} ${CFLAGS} ${LIBS} -o Midway

Cory: ${OBJS}
	cc ${OBJS} ${CFLAGS} ${LIBS} -i -o Cory

${OBJS}: externs.h

midway.o: globals.h

arch:
	ar uv MID.a ${FILES} ${OTHERFILES} 
	touch arch

profile: ${OBJS}
	cc -p -i ${OBJS} ${CFLAGS} ${LIBS} -o Midway.pro

install: ${PUB}/Midway ${PUB}/Midway.doc
${PUB}/Midway: Midway
	strip Midway
	cp Midway ${PUB}/Midway
	chmod 755 ${PUB}/Midway
	cp /dev/null ${PUB}/.midwaylog
	chmod 666 ${PUB}/.midwaylog
${PUB}/Midway.doc: Midway.doc
	cp Midway.doc ${PUB}/Midway.doc
	chmod 644 ${PUB}/Midway.doc

clean: arch
	rm -f ${OBJS} ${FILES} ${OTHERFILES} ${JUNKFILES}
	pack MID.a
