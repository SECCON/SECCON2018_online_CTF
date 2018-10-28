#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>





main()
{
  char *a;
  char mp[2048];
  char to[2048];
  char to2[2048];
  int len;
  int fd;
  int fd2;

  struct hostent *host;
  struct in_addr addr;
  char *h;
  char revh[2048];
  int maxlen;
  int llen;

//  printf("Key?:");
//  scanf("%s", a);

  addr.s_addr = 0x0200007f;
  host = gethostbyaddr((const char *)&addr.s_addr,
                       sizeof(addr.s_addr), AF_INET);
  if (host == NULL) {
    herror("gethostbyaddr");
    return 1;
  }

//  printf("%s\n", host->h_name);

  h = host->h_name;
  maxlen = strlen(h);
  for(llen = 0; llen < maxlen; llen++)
  {
    revh[maxlen - llen -1] = h[llen];
  }

  memset(mp, 0, 2048);
  memset(to, 0, 2048);
  memset(to2, 0, 2048);
  fd = open("data.dat", O_RDWR);
  read(fd, mp, 2048);
//  write(2, mp, 2048);
  close(fd);
  len = strlen(mp);

  enc(mp, to, len, revh);
  fd2 = open("flag.txt.encrypted", O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
  write(fd2, to, len);
  close(fd2);
  dec(to, to2, 2048, "abc");
//  write(2, mp, 2048);
//  write(2, to2, 2048);
//  write(2, to, 2048);
}

void enc(char *from, char *to, int length, char *key)
{
	char *enckey;
	int keylength;
	int l;
	enckey = key;
	keylength = strlen(key);

	for(l = 0; l < length; l++)
	{
		to[l] = from[l] ^ enckey[l%keylength];
	}
}

void dec(char *from, char *to, int length, char *key)
{
	char *deckey;
	int keylength;
	int l;
	deckey = key;
	keylength = strlen(key);

	for(l = 0; l < length; l++)
	{
		to[l] = from[l] ^ deckey[l%keylength];
	}
}
