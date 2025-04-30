# a baby makefile...
all: main

main:
	rm main
	gcc main.c psu.c fieldOrder.c -lm -l wiringPi -o main

test:
	rm psutest
	rm fieldOrderTest
	gcc psutest.c -o psutest
	gcc fieldOrderTest.c -o fieldOrderTest
