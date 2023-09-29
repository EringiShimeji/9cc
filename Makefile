BASE_FLAGS		=	-std=c11 -g
RELEASE_FLAGS	=	-static
DEBUG_FLAGS		=	-fsanitize=address

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