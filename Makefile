CC      = clang
INCLDIR = include/
BINDIR  = bin/
SRCDIR  = src/

_BIN    = main
BIN     = $(addprefix $(BINDIR), $(_BIN))

SRC     = $(wildcard src/*.c)
INCL    = $(wildcard include/*.h)
_OBJS   = $(patsubst src/%.c, %.o, $(SRC))
OBJS    = $(addprefix $(BINDIR), $(_OBJS))


all: $(BIN)

$(BIN): $(BINDIR) runner main trace fault-local tarantula sbi
	$(CC) $(OBJS) -fsanitize=address -g -o $(BIN)

$(BINDIR):
	mkdir -p $(BINDIR)

runner : src/runner.c
	$(CC) -g -c -o bin/runner.o src/runner.c
  
main : src/main.c
	$(CC) -g -c -o bin/main.o src/main.c

fault-local: src/fault-local.c
	$(CC) -g -c -o bin/fault-local.o src/fault-local.c

trace : src/trace-pc.c
	$(CC) -fsanitize=address -g -o bin/trace-pc.o -c src/trace-pc.c 

tarantula : src/tarantula.c
	$(CC) -fsanitize=address -g -o bin/tarantula.o -c src/tarantula.c 

sbi : src/sbi.c
	$(CC) -fsanitize=address -g -o bin/sbi.o -c src/sbi.c 

clean:
	rm -rf $(BINDIR)

