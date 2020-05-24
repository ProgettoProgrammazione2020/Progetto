STANDARD_FLAGS = --ansi --pedantic -g3 -O3 -std=gnu89
WARNING_FLAGS = -Wall -Wextra
LIBRARIES_FLAGS = -lm
SANITIZER = -fsanitize=address -fsanitize=undefined
SAFE = $(STANDARD_FLAGS) $(WARNING_FLAGS) $(LIBRARIES_FLAGS)
UNSAFE = $(SAFE) $(SANITIZER)


test: test_main_priv.o ip_lib.o bmp.o
	gcc test_main_priv.o ip_lib.o bmp.o -o test -g -O1 -lm

main: main_iplib.o ip_lib.o bmp.o
	gcc main_iplib.o ip_lib.o bmp.o -o main -g -O1 -lm

main_iplib.o: main_iplib.c ip_lib.h bmp.h
	gcc main_iplib.c -o main_iplib.o -c -lm

test_main_priv.o: test_bmp.c ip_lib.h bmp.h
	gcc test_main_priv.c -o test_main_priv.o -c -lm

ip_lib.o: ip_lib.c bmp.h
	gcc ip_lib.c -o ip_lib.o -lm -c $(SAFE) -fsanitize=undefined

bmp.o: bmp.c
	gcc bmp.c -o bmp.o -lm -c

clean_test:
	rm *.o
	rm test

clean_main:
	rm *.o
	rm main
