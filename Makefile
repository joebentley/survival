CC=gcc
CFLAGS=-Wall -Werror -g

OBJ=survival.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

survival: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

clean:
	rm -rf *.o survival survival.dSYM