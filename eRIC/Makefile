CC=gcc

all: ericTest.o
	$(CC) ericTest.c -o calcTest
	sudo chown root ericTest
	sudo chmod u+s ericTest

clean:
	rm *.o ericTest
