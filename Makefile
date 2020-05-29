LIBRARIES = -lm
STANDARDS = --ansi --pedantic -std=gnu89 -g3 -O3
WARNINGS = -Wall -Wextra
SANITIZER = -fsanitize=address -fsanitize=undefined
TEST = $(WARNINGS) $(STANDARDS) $(LIBRARIES) $(SANITIZER)
VALGRIND = -Wall --ansi --pedantic -ggdb -g -O1 $(LIBRARIES)
NAME =  main_iplib

main: main.o ip_lib.o bmp.o
	gcc main.o ip_lib.o bmp.o -o $(NAME) -omain_iplib $(TEST)

main.o: main_iplib.c ip_lib.h
	gcc main_iplib.c -omain.o -c -Wall --ansi --pedantic -ggdb

ip_lib.o: ip_lib.c ip_lib.h
	gcc ip_lib.c -oip_lib.o -c $(TEST)

bmp.o: bmp.c bmp.h
	gcc bmp.c -obmp.o -Wall -c

valgrind: test.o ip_lib.o bmp.o
	gcc main.o ip_lib.o bmp.o  $(VALGRIND)
	valgrind -v --leak-check=full --track-origins=yes ./test

clear_bmp:
	rm *.bmp

clear:
	rm *.o
	rm $(NAME)
