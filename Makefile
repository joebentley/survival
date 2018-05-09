CC=g++
CFLAGS=-Wall -Werror -g -std=c++11

DEPS=texture.h font.h dialog.h world.h
OBJ=survival.o texture.o font.o dialog.o world.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

survival: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lSDL2

clean:
	rm -rf *.o survival survival.dSYM