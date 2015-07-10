all:
	gcc -m32 -Wall -Werror -Wshadow -std=c99 -o boxpack boxpack.c
debug:
	gcc -ggdb -Wall -Werror -Wshadow -std=c99 -o boxpack boxpack.c
