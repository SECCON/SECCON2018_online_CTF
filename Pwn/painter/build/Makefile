# ASAN := -fsanitize=address  -fno-sanitize-recover
CXXFLAGS := -fPIC -g -O2 -std=c++11 ${ASAN}
LDFLAGS := -g -Wl,-z,relro,-z,now -pie ${ASAN}

all: painter

painter: bmp.o main.o commands.o commands.h bmp.h main.h Makefile
	$(CXX) $(LDFLAGS) -o painter bmp.o main.o commands.o
	strip ./painter

clean:
	rm -f painter *.o
