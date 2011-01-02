CC = cc
LD = cc
CFLAGS := -Wall -O3
LDFLAGS := -lm 
CPPFLAGS =
OBJS := colormap.o
TARGET = colormap

all: ${TARGET}

${TARGET}: ${OBJS}
	${LD} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ ${OBJS} 

$%.o: %.c
	${CC} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -c -o $@ $< 

install: ${TARGET}
	cp ${TARGET} ~tsbischof/bin/.

clean:
	rm -f ${OBJS} ${TARGET}
