CC=g++
CFLAGS=-Wall -Werror -g

DEPS=texture.h font.h
OBJ=survival.o texture.o font.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

survival: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lSDL2

clean:
	rm -rf *.o survival survival.dSYM