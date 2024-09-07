CC=gcc -lm

.traps: traps.c
	$(CC) -o traps traps.c

.trapt: trapt.c
	$(CC) -g -Wall -pthread -lpthread -o trapt trapt.c

all: .traps .trapt

clean:
	rm -fv traps trapt
