CC = gcc
OBJECTS = rbtree.o
CFLAGS = -w -Wall -s -pedantic-errors
EXECUTABLE = driver

.SUFFIXES: .c
.c.o: $<
	${CC} -c ${CFLAGS} $< -o $@


default: all

all: ${EXECUTABLE}

${EXECUTABLE}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o $@

clean:
	-@rm -f core > /dev/null 2>&1
	-@rm -f ${EXECUTABLE} > /dev/null 2>&1
	-@rm -f ${OBJECTS}  > /dev/null 2>&1

# Dependencies
rbtree.o: rbtree.c
