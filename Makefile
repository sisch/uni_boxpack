all:
	gcc -m32 -Wall -Werror -Wshadow -std=c99 -o boxpack boxpack.c
debug:
	gcc -ggdb -DDEBUG -Wall -Werror -Wshadow -std=c99 -o boxpack boxpack.c
test: debug
	valgrind --tool=callgrind --dump-instr=yes ./boxpack test out
