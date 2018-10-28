#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>	//open
#include <sys/stat.h>	//open
#include <fcntl.h>	//open
#include <unistd.h>	//read
#include <signal.h>	//signal
#include <stdlib.h>	//exit

#include "util.h"

void __stack_chk_fail(void){
	open_read_write("hint1.txt");
	exit(-1);
}

void signal_handler_timeout(int sig){
	printf("\nOops! Sorry, it's TIME OUT !\n");
	exit(-1);
}


void ctf_setup(void){
	setvbuf(stdin,0x0,0x2,0x0);
	setvbuf(stdout,0x0,0x2,0x0);
	setvbuf(stderr,0x0,0x2,0x0);
	signal(SIGALRM,signal_handler_timeout);
	alarm(TIMEOUT);
}

void open_read_write(char *filename){
	int fd;
	unsigned int file_size;
	char *buf;
	
	fd = open(filename,'r');

	file_size = lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	buf = (char *)malloc(sizeof(char) * file_size);
	read(fd,buf,file_size);
	write(1,buf,file_size);
	close(fd);
}
