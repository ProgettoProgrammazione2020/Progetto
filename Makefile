test: test_main_priv.o ip_lib.o bmp.o
	gcc test_main_priv.o ip_lib.o bmp.o -o test -lm

test_main_priv.o: test_bmp.c ip_lib.h bmp.h
	gcc test_main_priv.c -o test_main_priv.o -c -lm

ip_lib.o: ip_lib.c bmp.h
	gcc ip_lib.c -o ip_lib.o -lm -c

bmp.o: bmp.c
	gcc bmp.c -o bmp.o -lm -c

clean_test:
	rm *.o
	rm test
