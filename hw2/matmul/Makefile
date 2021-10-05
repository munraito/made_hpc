CC=gcc

CFLAGS=-c -Wall -g -O3

all: matmul

matmul: matmul.o mtr_lib.a
	$(CC) matmul.o mtr_lib.a -o matmul.exe

matmul.o: matmul.c
	$(CC) $(CFLAGS) matmul.c

mtr_init.o: mtr_init.c
	$(CC) $(CFLAGS) mtr_init.c

mtr_calc.o: mtr_calc.c
	$(CC) $(CFLAGS) mtr_calc.c

mtr_lib.a: mtr_init.o mtr_calc.o
	ar rcs mtr_lib.a mtr_init.o mtr_calc.o

clean:
	rm -rf *.o *.a matmul.exe