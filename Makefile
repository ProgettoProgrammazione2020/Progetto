LIBRARIES = -lm
STANDARDS = --ansi --pedantic -std=gnu89 -g3 -O3
WARNINGS = -Wall -Wextra
SANITIZER = -fsanitize=address -fsanitize=undefined
TEST = $(WARNINGS) $(STANDARDS) $(LIBRARIES) $(SANITIZER)
VALGRIND = -Wall --ansi --pedantic -ggdb -g -O1 $(LIBRARIES)

test: test.o ip_lib.o bmp.o
	gcc test.o ip_lib.o bmp.o -otest $(TEST)

test.o: test_main_priv.c ip_lib.h
	gcc test_main_priv.c -otest.o -c -Wall --ansi --pedantic -ggdb

ip_lib.o: ip_lib.c ip_lib.h
	gcc ip_lib.c -oip_lib.o -c $(TEST)

bmp.o: bmp.c bmp.h
	gcc bmp.c -obmp.o -Wall -c

valgrind: test.o ip_lib.o bmp.o
	gcc test.o ip_lib.o bmp.o  $(VALGRIND)
	valgrind -v --leak-check=full --track-origins=yes ./test

run_test_bmp: test_bmp
	./test_bmp

test_bmp: test_bmp.o bmp.o
	gcc test_bmp.o bmp.o -otest_bmp -Wall -lm

test_bmp.o: test_bmp.c
	gcc test_bmp.c -otest_bmp.o -Wall -c

clear_bmp:
	rm *.bmp

clear:
	rm *.o
	rm test

clear_test_bmp:
	rm *.o
	rm test_bmp
