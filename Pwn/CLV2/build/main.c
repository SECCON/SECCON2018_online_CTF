#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include "sha256.h"

/****************************** MACROS ******************************/
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))


/****************************** define...variables ******************************/
//char * encode 
int g_is_master;
char * g_master;
char * g_name;
char * g_user_list;

typedef struct node
{
  char padding[32];
  unsigned int key;
  char * content;
  size_t con_len;
  struct node * left;
  struct node * right;
}
Node;

typedef struct user
{
  char padding[32];
  unsigned int key;
  char * name;
  Node * prov_root;
  struct user * left;
  struct user * right;
}
User;

Node * g_root;
User * g_user_root;
User * g_user;
Node * g_node_arr[0x10000];


static const WORD k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/****************************** define...lib_functions ******************************/
//Node * make_node(char * content, size_t len);
void sha256_transform(SHA256_CTX *ctx, const BYTE data[])
{
	WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
	for ( ; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

	for (i = 0; i < 64; ++i) {
		t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
		t2 = EP0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
	WORD i;

	for (i = 0; i < len; ++i) {
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64) {
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
	WORD i;

	i = ctx->datalen;

	// Pad whatever data is left in the buffer.
	if (ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, ctx->data);
		memset(ctx->data, 0, 56);
	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = ctx->bitlen;
	ctx->data[62] = ctx->bitlen >> 8;
	ctx->data[61] = ctx->bitlen >> 16;
	ctx->data[60] = ctx->bitlen >> 24;
	ctx->data[59] = ctx->bitlen >> 32;
	ctx->data[58] = ctx->bitlen >> 40;
	ctx->data[57] = ctx->bitlen >> 48;
	ctx->data[56] = ctx->bitlen >> 56;
	sha256_transform(ctx, ctx->data);

	// Since this implementation uses little endian byte ordering and SHA uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
	}
}


/****************************** define...user_lib_functions ******************************/


void eat_nl(void)
{
  char c;

  c = getchar();
  while(c!=10)
  {
    c = getchar();
  }
}

size_t my_gets(char * buf)
{
  char c;
  int i=0;
  int ret;

  while(1)
  {
    ret = read(0, &c, 1);
    if(ret != 1)
      break;
    if(c == 10)
      break;
    buf[i++] = c;
  }
  //buf[i] = '\0';
  return i;
}

int my_getint(void)
{
  char buf[32];
  int ret;
  int i;

  for(i=0;i<32;i++)
  {
    ret = read(0, &buf[i], 1);
    if(!ret)
      break;
    if(buf[i] == '\n')
    {
      buf[i] = '\0';
      break;
    }
  }
  buf[31] = '\0';
  return atoi(buf);
}

/****************************** define...user_functions ******************************/



void Node_ctor(Node * n)
{
  strcpy(n->padding, "##Welcome_to_Secccon_CTF_2018##");
  n->left = NULL;
  n->right = NULL;
  n->content = NULL;
  n->con_len = 0;
  n->key = 0;
}

void User_ctor(User * u)
{
  strcpy(u->padding, "##Welcome_to_Secccon_CTF_2018##");
  u->left = NULL;
  u->right = NULL;
  u->prov_root = NULL;
  u->name = g_name;
  u->key = 0;
}

unsigned int get_hashint(char * content, size_t len)
{
  SHA256_CTX ctx;
  BYTE buf[SHA256_BLOCK_SIZE];
  unsigned int ret = 0;

  sha256_init(&ctx);
  sha256_update(&ctx, content, len);
  sha256_final(&ctx, buf);

  ret = *(unsigned int *)buf;
  return ret;
}

int is_bst_node(Node * node, unsigned int min, unsigned int max)
{
  if (node == NULL)
    return 1;
  if (node->key < min || node->key > max)
    return 0;
  return
    is_bst_node(node->left, min, node->key-1) &&
    is_bst_node(node->right, node->key+1, max);
}

int is_bst_user(User * node, unsigned int min, unsigned int max)
{
  if (node == NULL)
    return 1;
  if (node->key < min || node->key > max)
    return 0;
  return
    is_bst_user(node->left, min, node->key-1) &&
    is_bst_user(node->right, node->key+1, max);
}

void print_ascending(Node * n)
{
  if(n == NULL)
    return;
  print_ascending(n->left);
  printf("%d : %s\n", n->key, n->content);
  print_ascending(n->right);
  return;
}

void print_descending(Node * n)
{
  if(n == NULL)
    return;
  print_descending(n->right);
  printf("%d : %s\n", n->key, n->content);
  print_descending(n->left);
  return;
}

User * search_user(User * u, unsigned int key)
{
  if(u == NULL || u->key == key)
    return u;
  else if(u->key < key)
    return search_user(u->right, key);
  else
    return search_user(u->left, key);
}

Node * search(Node * n, unsigned int key)
{
  if(n == NULL || n->key == key)
    return n;
  else if(n->key < key)
    return search(n->right, key);
  else
    return search(n->left, key);
}

void show(void)
{
  char select;
  int k;
  Node * n1, *n2;

  if(!g_is_master)
    return;
  puts("==============================");
  printf("User : %s\n", g_name);
  printf("Wanna see ? [Y]es, [N]o\n");
  read(0, &select, 1);
  eat_nl();
  if(select == 'Y')
  {
    printf("Hint : ");
    k = my_getint();
    //scanf("%d", &k);
    //eat_nl();
    if(k == 0x1cafe)
    {
      n1 = g_user->prov_root;
      print_ascending(n1);
    }
    else if(k == 0x1beef)
    {
      n1 = g_user->prov_root;
      print_descending(n1);
    }
    else
    {
      n1 = g_user->prov_root;
      n2 = search(n1, k);
      if(n2 == NULL)
        return;
      printf("%d : %s\n", n2->key, n2->content);
    }
  }
  puts("==============================");
}

void tree_add_user(User ** parent, User * child)
{
  if (!*parent)
    *parent = child;
  else if ((*parent)->key > child->key)
    tree_add_user(&(*parent)->left, child);
  else
    tree_add_user(&(*parent)->right, child);
}

void tree_add(Node ** parent, Node * child)
{
  if (!*parent)
  {
    if(parent == &g_root)
    {
      g_user->prov_root = child;
    }
    *parent = child;
  }
  else if ((*parent)->key < child->key)
    tree_add(&(*parent)->right, child);
  else
    tree_add(&(*parent)->left, child);    
}

Node * minKeyNode(Node * n)
{
  Node * curr = n;

  while(curr->left != NULL)
    curr = curr->left;
  return curr;
}

Node * tree_delete(Node * n, unsigned int key)
{
  Node * tmp;

  if(n == NULL)
    return n;
  if(key < n->key)
    n->left = tree_delete(n->left, key);
  else if(key > n->key)
    n->right = tree_delete(n->right, key);
  else
  {
    if(n->left == NULL)
    {
      int k = n->key; 
      tmp = n->right;
      free(n->content);
      free(n);
      return tmp;
    }
    else if(n->right == NULL)
    {
      int k = n->key;
      tmp = n->left;
      free(n->content);
      free(n);
      return tmp;
    }
    tmp = minKeyNode(n->right);
    n->key = tmp->key;
    n->right = tree_delete(n->right, n->key);
  }
  return n;
}


void play(void)
{
  char select;
  int gap = 10000;
  int power;
  int i, j;
  int n;
  size_t con_len;
  char * content;
  Node * nd;
  char buf[512];
  SHA256_CTX ctx;

  //getchar();
  while(1)
  {
    puts("[S]how prov.");
    puts("[A]dd prov.");
    puts("[D]elete prov.");
    puts("[E]xit");
    
    read(0, &select, 1);
    eat_nl();

    if(select == 'S')
    {
      show();
    }
    else if(select == 'A')
    {
      printf("Your words > ");
      con_len = read(0, buf, 511);
      content = (char *)malloc(con_len+1);
      memcpy(content, buf, con_len);

      puts("Choose your favorite > ");
      puts("Method [1]");
      puts("Method [2]");
      puts("Method [3]");

      read(0, &select, 1);
      eat_nl();

      if(select == '1')
      {
        puts("My favorite...");
        gap = 10000;
        int power;
        puts("How strong?");
        scanf("%d",&power);
        eat_nl();
        if(power>50)
        {
          puts("Too strong!");
          _exit(1);
        }
        for(i=0;i<power;i++)
        {
          n = printf("%d", rand()&0xffff);
          usleep(gap);
          gap *= 1.1;
          for(j=0;j<n;j++)
          {
            putchar(8);
            putchar(32);
            putchar(8);
          }
        }
        n = rand()&0xffff;
        printf("%d!\n",n);
        nd = g_node_arr[n];
        if(!nd)
        {
          nd = (Node *)malloc(sizeof(Node));
          Node_ctor(nd);
          g_node_arr[n] = nd;
        }
        nd->key = n;
        nd->content = content;
        nd->con_len = con_len;
        tree_add(&g_root, nd);
      }
      else if(select == '2')
      {
        puts("Also good choice...");
        n = 0;
        for(i=0;i<con_len;i++)
        {
          n += content[i];
        }
        nd = g_node_arr[n];
        if(!nd)
        {
          nd = (Node *)malloc(sizeof(Node));
          Node_ctor(nd);
          g_node_arr[n] = nd;
        }
        nd->key = n;
        nd->content = content;
        nd->con_len = con_len;
        tree_add(&g_root, nd);
        if(!is_bst_node(g_root, 0, 0xffffffff))
        {
          _exit(1);
        }
      }
      else if(select == '3')
      {
        puts("Not bad...");
        n = get_hashint(content, con_len)&0xffff;
        nd = g_node_arr[n];
        if(!nd)
        {
          nd = (Node *)malloc(sizeof(Node));
          Node_ctor(nd);
          g_node_arr[n] = nd;
        }
        nd->key = n;
        nd->content = content;
        nd->con_len = con_len;
        tree_add(&g_root, nd);
        if(!is_bst_node(g_root, 0, 0xffffffff))
        {
          _exit(1);
        }
      }
      else
      {
        puts("Constructing...");
      }
      for(i=0;i<3;i++)
      {
        printf(".");
        usleep(100000);
      }
      printf("\nAdded!\n");
    }
    else if(select == 'D')
    {
      printf("Hint : ");
      n = my_getint();
      g_root = tree_delete(g_root, n);
      g_user->prov_root = g_root;

      for(i=0;i<3;i++)
      {
        printf(".");
        usleep(100000);
      }
      printf("\nDeleted!\n");
    }
    else if(select == 'E')
    {
      puts("Don't give up!");
      return;
    }
  }

  return;
}

int login(void)
{
  int ret = 0;
  char buf[256] = {};
  int n;
  User * u;

  printf("User : ");
  n = read(0, buf, 255);
  if(buf[n-1] == 10)
  {
    n -= 1;
  }
  n = get_hashint(buf, n);
  u = search_user(g_user_root, n);
  if(u == NULL)
    return ret;
  g_root = u->prov_root;
  g_user = u;
  return ret ^ 1;
}

void __init(void)
{
  int i;

  setvbuf(stdin, 0, 2, 0);
  setvbuf(stdout, 0, 2, 0);

  srand((long)&i & 0xffff);
  alarm(240);

  g_user_root = (User *)malloc(sizeof(User));
  User_ctor(g_user_root);
  for(i=0;i<0x10000;i++)
  {
    g_node_arr[i] = (Node *)malloc(sizeof(Node));
    Node_ctor(g_node_arr[i]);
  }

  printf("Loading");

  for(i=0;i<3;i++)
  {
    printf(".");
    sleep(1);
  }

  putchar(10);
}

int main(void)
{

  int is_login = 0;
  char select;
  char * env_user = NULL;
  char buf[256];
  User * u;
  Node * n;

  __init();
  
  while(1)
  {
    if(!is_login)
      puts("[L]ogin");
    else
    {
      puts("[L]ogout");
      puts("[P]lay");
    }
    puts("[R]egister");
    puts("[E]xit");

    read(0, &select, 1);
    eat_nl();

    if(select == 'L')
    {
      if(is_login)
      {
        is_login = 0;
      }
      else
      {
        is_login = login();
      }
    }
    if(select == 'P')
    {
      if(is_login)
        play();
    }
    else if(select == 'R')
    {
      char * tmp;
      int len;
      puts("name?");
      len = my_gets(buf);
      env_user = getenv("MAST");
      if(!strcmp(env_user, buf))
      {
        g_is_master = 1;
        g_name = env_user;
      }
      else
      {
        g_is_master = 0;
        g_name = (char *)malloc(len+2);
        strncpy(g_name, buf, len);
        g_name[len] = '\0';
      }

      unsigned int key = get_hashint(g_name, strlen(g_name));
      if(search_user(g_user_root, key))
      {
        continue;
      }

      u = (User *)malloc(sizeof(User));
      User_ctor(u);
      u->key = key;
      tree_add_user(&g_user_root, u);
      printf("%s registered.\n", g_name);
      g_user = u;
      g_root = u->prov_root;
      is_login = 1;
    }
    else
    {
      if(select == 'E')
        _exit(0);
    }

  }
  
}
