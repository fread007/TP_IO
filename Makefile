CC = gcc
FLAGS = -Wall -Werror

all: eval-libc eval-syscall test-format test-rand test test-ecriture test-buffer test-lecture

eval-libc: eval-libc.c
	$(CC) $(FLAGS) -o eval-libc eval-libc.c stdes.c

eval-syscall: eval-syscall.c
	$(CC) $(FLAGS) -o eval-syscall eval-syscall.c stdes.c

test-format: test-format.c
	$(CC) $(FLAGS) -o test-format test-format.c stdes.c

test-rand: test-rand.c
	$(CC) $(FLAGS) -o test-rand test-rand.c stdes.c

test: test.c
	$(CC) $(FLAGS) -o test test.c stdes.c

test-ecriture: test_ecriture.c
	$(CC) $(FLAGS) -o test_ecriture test_ecriture.c stdes.c

test-lecture: test_lecture.c
	$(CC) $(FLAGS) -o test_lecture test_lecture.c stdes.c

test-buffer: test_buffer.c
	$(CC) $(FLAGS) -o test_buffer test_buffer.c stdes.c

clean:
	rm -f eval-libc eval-syscall test-format test-rand test test_ecriture test_lecture test_buffer

