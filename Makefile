CC=emcc
SOURCES:=$(wildcard src/*.cpp)
LDFLAGS=-O2 --llvm-opts 2
NAME=native
OUTPUT=public/$(NAME).html
OUTPUT2=public/$(NAME).wsam
DIST:=$(wildcard public/$(NAME).*)

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES) 
	$(CC) $(SOURCES) --bind -s WASM=1 -s FULL_ES2=1 -std=c++11 $(LDFLAGS) -o $(OUTPUT)

clean:
	rm $(DIST)
