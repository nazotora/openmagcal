# a baby makefile...
all: main

main:
	gcc main.c psu.c fieldOrder.c -lm -l wiringPi -o main

test:
	gcc psutest.c -o psutest
	gcc fieldOrderTest.c -o fieldOrderTest
