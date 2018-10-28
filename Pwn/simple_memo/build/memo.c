// gcc memo.c -Wl,-z,norelro -fPIE -pie -o memo
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <stddef.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <linux/audit.h>
#include <linux/filter.h>
#include <linux/seccomp.h>

#define BUF_SIZE	128
#define MEMO_SIZE	0x28
#define MEMOS		0x10

static int set_seccomp(void);
static int menu(void);
static void add(char **memo);
static void show(char **memo);
static void delete(char **memo);

static int getnline(char *buf, int len);
static int getint(void);

__attribute__((constructor))
int init(){
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	set_seccomp();
	return 0;
}

static int set_seccomp(void){
	struct sock_filter filter[] = {
		BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, arch))),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),

		BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr))),

		BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K, __X32_SYSCALL_BIT, 0, 1),
		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),

		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_open, 1, 0),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_openat, 0, 1),
		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL),

		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
	};

	struct sock_fprog prog = {
		.len = (unsigned short) (sizeof(filter) / sizeof(struct sock_filter)),
		.filter = filter,
	};

	if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
		perror("prctl PR_SET_NO_NEW_PRIVS");
		return -1;
	}

	if(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)){
		perror("prctl PR_SET_SECCOMP");
		return -1;
	}

	return 0;
}

int main(void){
	char *memo[MEMOS] = {};

	puts("<<<< simple memo service >>>>");

	for(;;){
		switch(menu()){
			case 1:
				add(memo);
				break;
			case 2:
				show(memo);
				break;
			case 3:
				delete(memo);
				break;
			case 0:
				goto end;
			default:
				puts("Wrong input.");
		}
	}

end:
	puts("Bye!");

	return 0;
}

static int menu(void){
	printf(	"\nMENU\n"
			"1. Add\n"
			"2. Show\n"
			"3. Remove\n"
			"0. Exit\n"
			"> ");

	return getint();
}

static void add(char **memo){
	int id;

	for(id = 0; id < MEMOS; id++)
		if(!memo[id])
			break;

	if(id >= MEMOS){
		puts("Entry is FULL...");
		return;
	}

	memo[id] = (char*)calloc(MEMO_SIZE, 1);

	printf("Input memo > ");
	getnline(memo[id], MEMO_SIZE);

	printf("Added id:%02d\n", id);
}

static void show(char **memo){
	int id, n;

	printf("Input id > ");
	id = getint();

	if(!memo[id]){
		puts("Entry does not exist...");
		return;
	}

	printf("Show id:%02d\n%s\n", id, memo[id]);
}

static void delete(char **memo){
	int id;

	printf("Input id > ");
	id = getint();

	if(!memo[id]){
		puts("Entry does not exist...");
		return;
	}

	free(memo[id]);
	memo[id] = NULL;

	printf("Deleted id:%02d\n", id);
}

static int getnline(char *buf, int size){
	char *lf;

	if(size < 0)
		return 0;

	fgets(buf, size, stdin);
	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return 1;
}

static int getint(void){
	char buf[BUF_SIZE];

	getnline(buf, sizeof(buf));
	return atoi(buf);
}
