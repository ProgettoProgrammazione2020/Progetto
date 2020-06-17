LIBRARIES = -lm
STANDARDS = --ansi --pedantic -std=gnu89 -g3 -O3
WARNINGS = -Wall -Wextra
SANITIZER = -fsanitize=address -fsanitize=undefined
TEST = $(WARNINGS) $(STANDARDS) $(LIBRARIES) $(SANITIZER)
VALGRIND = -Wall -g -O1 $(LIBRARIES)
NAME =  main_iplib
PROVA = img/gondola.bmp img/gondola.bmp gray gray_gondola.bmp 1

main: main.o ip_lib.o bmp.o
	gcc main.o ip_lib.o bmp.o -o $(NAME) -omain_iplib $(TEST)

main.o: main_iplib.c ip_lib.h
	gcc main_iplib.c -omain.o -c -Wall --ansi --pedantic -ggdb

ip_lib.o: ip_lib.c ip_lib.h
	gcc ip_lib.c -oip_lib.o -c $(TEST)

bmp.o: bmp.c bmp.h
	gcc bmp.c -obmp.o -Wall -c

valgrind: $(NAME).c ip_lib.c bmp.c
	gcc bmp.c -obmp.o -Wall -c
	gcc ip_lib.c -oip_lib.o -c $(VALGRIND)
	gcc main_iplib.c -omain.o -c $(VALGRIND)
	gcc main.o ip_lib.o bmp.o -o $(NAME) -omain_iplib $(VALGRIND)
	valgrind -v --leak-check=full --track-origins=yes ./$(NAME) $(PROVA)

clear_bmp:
	rm *.bmp

clear:
	rm *.o
	rm $(NAME)
