CFLAGS = -Wall -Wno-unused-function `sdl2-config --cflags` -g
#-Wextra -Wpedantic

SRCS = Main.c ChaseHQ.c ChaseHQ-Data.c ChaseHQ-StageData.c
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

ChaseHQ: $(OBJS)
	$(CC) `sdl2-config --libs` -o $@ $^

.PHONY: clean
clean:
	$(RM) ChaseHQ $(OBJS) $(DEPS)

.PHONY: fmt
fmt:
	astyle --project=.astylerc *.[ch]

%.d: %.c
	@set -e; rm -f $@; \
	 $(CC) -M $(CFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

-include $(DEPS)

