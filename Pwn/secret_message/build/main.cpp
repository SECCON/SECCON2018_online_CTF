#include <stdio.h>
#include <iostream>
#include <time.h>
#include "cylib.h"
#include <stdlib.h>
#include <unistd.h>
using namespace std;
void help();
void init();

int main(int argc, char *argv[]) {
	init();
	char c;
	puts("welcome~");
	c = getchar();
	switch (c) {
	case 'e': {

		cyencode();

	}break;
	case 's': {

		cyshow();
	}break;
	default:
		help();
	}
	return 0;
}

void help() {
	puts(":(");
}
void init() {
    setlinebuf(stdin);
    setlinebuf(stdout);
	srand((unsigned int)time(NULL));
//    chdir("/home/secret_message/");
}