CC = gcc
CFLAGS = -g -Wall

all: csim ctuner

csim: csim.c
	$(CC) $(CFLAGS) -o csim csim.c

ctuner: ctuner.c
	$(CC) $(CFLAGS) -o ctuner ctuner.c

test-csim: csim
	python3 test-csim.py

test-ctuner: ctuner
	python3 test-ctuner.py

test: test-csim test-ctuner

clean:
	rm -f csim ctuner

.PHONY: all test test-csim test-ctuner clean
