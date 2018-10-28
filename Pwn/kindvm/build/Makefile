kindvm:main.o kindvm.o util.o
	gcc -m32 -o kindvm main.o kindvm.o util.o -Wl,-z,relro

main.o:main.c
	gcc -m32 -c main.c

kindvm.o:kindvm.c
	gcc -m32 -c kindvm.c

util.o:util.c
	gcc -m32 -c util.c

clean:
	rm -f main.o kindvm.o util.o
