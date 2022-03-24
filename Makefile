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

$(BIN): $(BINDIR) runner main fault-local trace
	$(CC) $(OBJS) -fsanitize=address -g -o $(BIN)

$(BINDIR):
	mkdir -p $(BINDIR)

runner : src/runner.c
	$(CC) -g -c -o bin/runner.o src/runner.c
  
main : src/main.c
	$(CC) -g -c -o bin/main.o src/main.c

fault-local: src/fault-local
	$(CC) -g -c -o bin/fault-local.o src/fault-local.c

trace : src/trace-pc.c
	$(CC) -fsanitize=address -g -o bin/trace-pc.o -c src/trace-pc.c 

clean:
	rm -rf $(BINDIR)

test: $(BIN)
	$(CC) -g -fsanitize=address -fsanitize-coverage=trace-pc-guard -o bin/jsondump #test/test_c_with_three_input/triangle.c bin/trace-pc.o
