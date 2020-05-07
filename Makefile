test: test_bmp.o ip_lib.o bmp.o
	gcc test_bmp.o ip_lib.o bmp.o -o test -lm

test_bmp.o: test_bmp.c ip_lib.h bmp.h
	gcc test_bmp.c -o test_bmp.o -c -lm

ip_lib.o: ip_lib.c bmp.h
	gcc ip_lib.c -o ip_lib.o -lm -c 

bmp.o: bmp.c 
	gcc bmp.c -o bmp.o -lm -c 

clean_test:
	rm *.o 
	rm test
