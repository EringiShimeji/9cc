BASE_FLAGS		=	-std=c11 -g
ifneq ($(shell uname), Darwin)
RELEASE_FLAGS	=	-static
endif
DEBUG_FLAGS		=	-fsanitize=address
CC				=	gcc

ifdef DEBUG
	CFLAGS = $(BASE_FLAGS) $(DEBUG_FLAGS)
else
	CFLAGS = $(BASE_FLAGS) $(RELEASE_FLAGS)
endif

9cc: 9cc.c

test: 9cc
		./test.sh

clean:
		rm -f 9cc *.o *~ tmp*

.PHONY: test clean
