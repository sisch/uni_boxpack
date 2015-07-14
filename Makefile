all: compile
debug:
	gcc -ggdb -DDEBUG -Wall -Werror -Wshadow -std=c99 -o boxpack boxpack.c

testDebug: debug
	valgrind --tool=callgrind --dump-instr=yes ./boxpack test out
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./boxpack test out

test: compile
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./boxpack test out	

compile:
	gcc -m32 -Wall -Werror -Wshadow -std=c99 -o boxpack boxpack.c
