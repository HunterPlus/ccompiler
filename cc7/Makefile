CFLAGS=-std=c11 -g -static

mycc: 	main.o
		$(CC) -o $@ $? $(LDFLAGS)
test:	mycc
			./test.sh
clean:
		rm -f mycc *.o *~ tmp*
.PHONY:	test clean