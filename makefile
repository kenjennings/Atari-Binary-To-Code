
EXE=bin2code

all: $(EXE) 

# linux CFLAGS=-Wall -I.
# solaris CFLAGS=-I.
CFLAGS=-Wall -I.

BIN2CODE_OBJS=code.o io.o myglobals.o globals.o main.o 


.c.o: $*.c $*.h
	@echo $*.c
	$(CC) $(CFLAGS) -c $*.c

$(EXE): $(BIN2CODE_OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $(BIN2CODE_OBJS)

clean: 
	rm -f $(EXE) *.o *~

install:
	cp $(EXE) ~/bin


