CC=g++
CFLAGS=-Wall -Werror -std=c++14 -g

DEPS=texture.h font.h dialog.h world.h entity.h behaviours.h entities.h point.h
OBJ=survival.o texture.o font.o dialog.o world.o entity.o behaviours.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

survival: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lSDL2

clean:
	rm -rf *.o survival survival.dSYM
