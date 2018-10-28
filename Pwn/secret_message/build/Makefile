all: secret_message
secret_message: cipher.o cylib.o main.o
	g++ -o secret_message main.o cylib.o cipher.o
	strip -s secret_message
main.o: main.cpp
	g++ -c -o main.o main.cpp
cylib.o: cylib.cpp cylib.h
	g++ -c -o cylib.o cylib.cpp
cipher.o: cipher.cpp cipher.h
	g++ -c -o cipher.o cipher.cpp
clean:
	rm secret_message *.o
