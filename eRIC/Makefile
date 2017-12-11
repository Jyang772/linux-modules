CC=gcc
objects = test receive

all: $(objects)

$(objects): %: %.c
	$(CC) -o $@ $< 

#sudo chown root ericTest
#sudo chmod u+s ericTest

clean:
	rm receive test
