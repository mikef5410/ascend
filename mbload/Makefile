#
# Makefile modified by Mike Ferrara x74479 for HP-UX
#

SHELL=	/bin/sh
CC=	cc -DSYSV -Aa -D_HPUX_SOURCE -O
CFLAGS=	

SRC=	mbload.c newsend.c getput.c
HDR=	xmodem.h
OBJ=	${SRC:.c=.o}

PROG=	mbload
BIN=	/usr/local/bin

${PROG}: ${OBJ}
	${CC} ${CFLAGS} -o ${PROG} ${OBJ}

clean:
	rm ${PROG} ${OBJ} *~

install:
	install ${PROG} ${BIN}

shar:
	shar -v README Makefile ${HDR} ${SRC} getput.sysv.c >mbload.shar







