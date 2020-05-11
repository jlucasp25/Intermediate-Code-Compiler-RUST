
OBJS=scanner.o parser.o ast.o code.o table.o
INTERM=scanner.c parser.c parser.h
PROGRAM=./test
CFLAGS=-g 

all: $(PROGRAM)

scanner.c: scanner.flex parser.h
	flex -o scanner.c scanner.flex

parser.c parser.h: parser.bison
	bison --defines=parser.h -o parser.c parser.bison

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS)
	
clean:
	rm -f $(PROGRAM) $(OBJS) $(INTERM)

