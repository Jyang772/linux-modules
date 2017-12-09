CC=gcc

all: calcTest.o
	$(CC) calcTest.c -o calcTest
	sudo chown root calcTest
	sudo chmod u+s calcTest

clean:
	rm *.o calcTest
