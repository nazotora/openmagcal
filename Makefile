# a baby makefile...
all: main

main:
	gcc main.c psu.c fieldOrder.c -lm -l wiringPi -o main

test:
	gcc psutest.c psu.c -o psutest
	gcc fieldOrderTest.c -o fieldOrderTest

psutest:
	gcc psutest_extended.c psu.c -lm -l wiringPi -o psutest_extended