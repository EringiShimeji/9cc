BASE_FLAGS		=	-std=c11 -g
ifneq ($(shell uname), Darwin)
RELEASE_FLAGS	=	-static
endif
DEBUG_FLAGS		=	-fsanitize=address
CC				=	gcc
SRCS			=	$(wildcard *.c)
OBJS			=	$(SRCS:.c=.o)
TEST_SRCS		=	$(wildcard test/*.c)
TEST_OBJS		=	$(TEST_SRCS:.c=.o)

ifdef DEBUG
	CFLAGS = $(BASE_FLAGS) $(DEBUG_FLAGS)
else
	CFLAGS = $(BASE_FLAGS) $(RELEASE_FLAGS)
endif

9cc: $(OBJS) $(TEST_OBJS)
		$(CC) -o 9cc $(OBJS) $(LDFLAGS) $(CFLAGS)

$(OBJS): 9cc.h

test: 9cc
		./test.sh

clean:
		rm -f 9cc *.o $(TEST_OBJS) *~ tmp*

.PHONY: test clean
