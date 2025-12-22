CFLAGS = -Wall #-Wextra -Wpedantic

proto: proto.c
	gcc $(CFLAGS) `sdl2-config --cflags --libs` -g -o $@ $<

fmt:
	astyle --project=.astylerc proto.c data.c

clean:
	rm -f proto *.o

.PHONY:	fmt clean

