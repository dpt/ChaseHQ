CFLAGS = -Wall -Wno-unused-function #-Wextra -Wpedantic

ChaseHQ: ChaseHQ.c ChaseHQ-Data.c ChaseHQ-StageData.c Main.c
	gcc $(CFLAGS) `sdl2-config --cflags --libs` -g -o $@ $^

fmt:
	astyle --project=.astylerc *.[ch]

clean:
	rm -f ChaseHQ *.o

.PHONY:	fmt clean

