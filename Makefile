run_main: test
	./test

valgrind: test.o ip_lib.o bmp.o
	gcc test.o ip_lib.o bmp.o -otest -Wall --ansi --pedantic -ggdb -lm -g -O1
	valgrind -v --leak-check=full --track-origins=yes ./test

test: test.o ip_lib.o bmp.o
	gcc test.o ip_lib.o bmp.o -otest -Wall --ansi --pedantic -lm -g3 -O3 -fsanitize=address -std=gnu89 -Wextra

test.o: test_main_priv.c ip_lib.h
	gcc test_main_priv.c -otest.o -c -Wall --ansi --pedantic -ggdb
	
ip_lib.o: ip_lib.c ip_lib.h
	gcc ip_lib.c -oip_lib.o -c -Wall --ansi --pedantic -ggdb
	
run_test_bmp: test_bmp
	./test_bmp

test_bmp: test_bmp.o bmp.o
	gcc test_bmp.o bmp.o -otest_bmp -Wall -lm
	
test_bmp.o: test_bmp.c
	gcc test_bmp.c -otest_bmp.o -Wall -c
	
bmp.o: bmp.c bmp.h
	gcc bmp.c -obmp.o -Wall -c

clear:
	rm *.o
	rm test
	rm test_bmp
