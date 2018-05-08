CC=g++
CFLAGS=-Wall -Werror -g

OBJ=survival.o

%.o: %.cpps
	$(CC) -c -o $@ $< $(CFLAGS)

survival: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lSDL2

clean:
	rm -rf *.o survival survival.dSYM