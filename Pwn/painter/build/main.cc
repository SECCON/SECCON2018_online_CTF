#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "bmp.h"
#include "commands.h"
#include "main.h"

int getnline(char *buf, int size){
	char *lf;
	char *n;

	if(size < 1 || (n = fgets(buf, size, stdin)) <= 0)
		return 0;

	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return 1;
}

int getint(void){
	char buf[0x10]={0};

	getnline(buf, sizeof(buf));
	return atoi(buf);
}


const cmditem_t kCommands[] = {
	{"resize", &resize_img},
	{"color", &select_a_color},
	{"line", &draw_a_line},
	{"rectangle", &draw_a_rectangle},
	{"dump", &dump_image}
};

const cmditem_t *find_command(const char *cmd) {
	// Find commands by name
	for(int i = 0; i < sizeof(kCommands) / sizeof(kCommands[0]); i++) {
		const cmditem_t *cur = &kCommands[i];
		if(strcmp(cur->name, cmd)) continue;

		return cur;
	}
	return NULL;
}

char empty[] = "";

int main()
{
	struct image* img = NULL;
	char buf[0x1000];
	uint32_t width, height;

	alarm(60);

	setvbuf(stdout, 0, 2, 0);
	setvbuf(stdin, 0, 2, 0);

	while(true) {
		puts("Give me your data size (width height): ");
		if(!getnline(buf, sizeof(buf))) return 1;

		if(sscanf(buf, " %d %d", &width, &height) == 2) break;
	}

	img = create_image(width, height);

	if(img) {
		printf("load done\nwidth: %d\nheight: %d\n", width, height);
	}
	else {
		puts("width / height too large");
		return 1;
	}

	while(1) {
		if(!getnline(buf, sizeof(buf))) return 1;

		char *args = strchr(buf, ' ');
		if(!args) args = empty;
		else *args++ = '\0';

		const cmditem_t *cmd = find_command(buf);
		if(!cmd) break;

		if(cmd->handler(&img, args)) {
			printf("%s success!\n", cmd->name);
		} else {
			printf("%s failed!\n", cmd->name);
		}
	}

	puts("bye!");
	return 0;
}
