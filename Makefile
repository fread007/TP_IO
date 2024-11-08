CC = gcc
FLAGS = -Wall -Werror

all: eval-libc eval-syscall test-format test-rand test

eval-libc: eval-libc.c
	$(CC) $(FLAGS) -o eval-libc eval-libc.c stdes.c

eval-syscall: eval-syscall.c
	$(CC) $(FLAGS) -o eval-syscall eval-syscall.c stdes.c

test-format: test-format.c
	$(CC) $(FLAGS) -o test-format test-format.c stdes.c

test-rand: test-rand.c
	$(CC) $(FLAGS) -o test-rand test-rand.c stdes.c

test: test.c
	$(CC) $(FLAGS) -o test test.c stdes.h

clean:
	rm -f eval-libc eval-syscall eval-test-format test-rand test

