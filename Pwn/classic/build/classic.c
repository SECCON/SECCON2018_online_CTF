// gcc -fno-stack-protector classic.c -o classic
#include <stdio.h>

#define BUF_SIZE 64

char *gets(char *s);

__attribute__((constructor))
int init(){
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	return 0;
}

int main(void){
	char local[BUF_SIZE];

	puts("Classic Pwnable Challenge");

	printf("Local Buffer >> ");
	gets(local);

	puts("Have a nice pwn!!");

	return 0;
}
