
#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include "cystruct.h"
#include "cipher.h"



void fail() {
	puts(":(");
	exit(0);
}
void path_check(char *path,int l){
    for(int i=0;i<l;i++){
        if('.'==path[i]){
            fail();
        }
    }
}
int read_(char *ptr, int size){
	int cnt=0,b;
	for(int i=0;i<size;i++){
		b=read(0,ptr+i,1);
		if(b==0||b==-1||ptr[i]=='\n'){
			ptr[i]=0;
			return cnt;
		}
		cnt++;
	}
	return cnt;
}
char check_input(char input) {

	if ((input >= 0x61 && input <= 0x7a) || (input >= 0x41 && input <= 0x5a) || (input >= 0x30 && input <= 0x39))
		return input;
	else {
		return '0';
	}

}
cy_header* mk_header() {
	cy_header *ptr = new cy_header;
	if (ptr == NULL) {
		exit(0);
	}
	memcpy(ptr->signiture, signiture, sizeof(signiture));
	return ptr;
}

void write_cydata(FILE *fd, char c) {
	int i, j,loc=0;
	cy_data *ptr = new cy_data;
	if (ptr == NULL) {
		exit(0);
		return;
	}

	ptr->signiture[0] = '\x12';
	ptr->signiture[1] = '\xab';
	ptr->checksum = checksum_d;

	for (i = 0; i < CHAR_LEN; i++) {
		if (character[i] == c) {
			ptr->print_len = printing_len[i];
			unsigned char *data_ptr = new unsigned char[ptr->print_len];//(unsigned char *)malloc(ptr->print_len+8);
			unsigned char *enc_buf = new unsigned char[ptr->print_len*16+8*16];//(unsigned char *)malloc(ptr->print_len*16+8*16);
			if (data_ptr == NULL) {
				exit(0);
				return;
			}if (enc_buf == NULL) {
				exit(0);
				return;
			}

			Cipher cc;
			Matrix p_m,c_m;
			while (1) {
				unsigned long long rr = rand() & 0xffff;
				ptr->key.key_ = (unsigned long long) ((rr << 32 + 1) | (rr << 48 + 2) | (rr << 16 + 3) |
					rr);
				if (cc.setkey(ptr->key.key_)) {
					break;
				}

			}

			memcpy(data_ptr, table[i], ptr->print_len);
			for (j = 0; j < ptr->print_len / 8; j++) {
				p_m = QWORD_to_Matrix(*(QWORD *)(data_ptr + j * 8));
				c_m = cc.encrypt(p_m);
				memcpy(enc_buf + loc, &c_m, sizeof(c_m));
				ptr->checksum ^= *(unsigned long long *)(enc_buf + loc);
				loc += sizeof(c_m);
			}
			if (ptr->print_len % 8 != 0) {
				p_m = QWORD_to_Matrix(*(QWORD *)(data_ptr + j * 8));
				c_m = cc.encrypt(p_m);
				memcpy(enc_buf + loc, &c_m, sizeof(c_m));
				ptr->checksum ^= *(unsigned long long *)(enc_buf + loc);
				loc += sizeof(c_m);
				j++;
			}
			ptr->len = loc;// 8 * j;
			fwrite(ptr,1, sizeof(cy_data),fd);
			fwrite(enc_buf, 1, loc, fd);
			delete[] data_ptr;
			delete[] enc_buf;
			delete[] ptr;

			return;
		}
	}


}
unsigned long long getchecksum(unsigned long long ch, unsigned long long *data, unsigned int l) {

	for (int i = 0; i < l / sizeof(Matrix); i++) {
		ch ^= data[sizeof(Matrix)/sizeof(unsigned long long)*i];
	}

	return ch;
}

int fileread(FILE *fd, int len, unsigned char *ptr) {
	int res = 0, i;
	for (i = 0; i < len; i++) {
		res += fread(ptr + i, 1, 1, fd);
	}
	return res;
}

int read_cydata(FILE *fd) {
    Cipher* cc= new Cipher;

	QWORD p;
	cy_data *cy_d = new cy_data;

    unsigned char *dec_buf;
    unsigned char *data;

    Matrix p_m;
	if (cy_d == NULL) {
		exit(0);
		return -1;
	}

	int x = fileread(fd, sizeof(cy_data), (unsigned char *)cy_d);
	if (sizeof(cy_data) != x) 
	{
		fail();
	}
	if (cy_d->signiture[0] != 0x12 || cy_d->signiture[1] != 0xab) {
		fail();
	}

	data = new unsigned char[cy_d->len];

	if (data == NULL) {
		exit(0);
		return -1;
	}
	dec_buf = new unsigned char[cy_d->len];
	if (dec_buf == NULL) {
		exit(0);
		return -1;
	}

	if (cy_d->len != fileread(fd, cy_d->len, (unsigned char *)data)) {
		fail();
	}

	if (checksum_d != getchecksum(cy_d->checksum, (unsigned long long*)data, cy_d->len)) {
		puts("check");
		fail();

	}

	
	cc->setkey(cy_d->key.key_);
	for (int i = 0; i < cy_d->len / sizeof(Matrix); i++) {
		p_m = cc->decrypt(*(Matrix *)(data + sizeof(Matrix)*i));
		p = Matrix_to_QWORD(p_m);
		memcpy(dec_buf + 8 * i, &p, 8);
	}
	for (int i = 0; i < cy_d->print_len; i++) {
		if (printing[dec_buf[i]] == 0) {
			break;
		}
		printf(printing[dec_buf[i]]);// vuln index overflow + fsb //"%c", printing[dec_buf[i]] - 50);//vuln
	}
}

void cyshow() {
	FILE *fd;
	int i,l;
    path= new char[0x200];


	puts("path : ");
	l=read_(path,0x200);
    path_check(path,l);
    chdir("/home/secret_message/files/");

	fd = fopen(path, "rb");
	if(fd==NULL){
		fail();
	}
	cy_header *cy_hd = new cy_header;
	if (cy_hd == NULL) {
		fail();
	}
	if (sizeof(cy_header) != fread(cy_hd,1,sizeof(cy_header),fd)) {
		fail();
	}
	if (strncmp((const char*)cy_hd->signiture, signiture, 4)) {
		fail();
	}
	from = new char[cy_hd->from_len];
	to = new char[cy_hd->to_len];
	from[cy_hd->from_len] = 0;
	to[cy_hd->to_len] = 0;

	for (i = 0; i < cy_hd->total_len; i++) {
		read_cydata(fd);
	}
	if (cy_hd->from_len != fread(from,1,cy_hd->from_len,fd)) {
		fail();
	}
	if (cy_hd->to_len != fread(to,1, cy_hd->to_len,fd)) {
		fail();
	}
	printf("\nfrom : %s\nto : %s\n", from, to);
	delete cy_hd;
	delete[] from;
	delete[] to;
	fclose(fd);
}

void cyencode() {
	char c;
	int tosize,fromsize;
	int i, len;
    to = new char[0x210];
    from=new char[0x210];
    message=new char[0x210];
    filename=new char[0x210];
	puts("to : ");
	tosize=read_(to,0x200);
	puts("from : ");
	fromsize=read_(from,0x200);


    chdir("/home/secret_message/files/");
	puts("filename : ");
	len=read_(filename,0x200);
    path_check(filename,len);

	puts("message length :");
	scanf("%d",&len);

	puts("message : ");
	FILE *fd;


	fd = fopen(filename, "wb");
	if (fd == NULL) {
		fail();
		return;
	}

	cy_header *cy_hd = mk_header();
	cy_hd->total_len = len;
	cy_hd->from_len = fromsize;
	cy_hd->to_len = tosize;
	fwrite(cy_hd,1, sizeof(cy_header),fd);


	for (i = 0; i < len; i++) {
		read(0,&c,1);

		c=check_input(c);
		write_cydata(fd, c);
	}
	fwrite(from,1, fromsize,fd);
	fwrite(to,1, tosize,fd);

	delete cy_hd;
    delete[] from;
    delete[] to;
    delete[] message;
    delete[] filename;
	fclose(fd);

	return;
}