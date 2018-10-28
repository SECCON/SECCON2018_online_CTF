#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/queue.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/select.h>
#include <strings.h>

in_addr_t permitted_address;
#define VERSION "0.0.1"

#define DEBUG 0

typedef struct _http {
    int fd;
    int state;
    struct sockaddr_in address;
    char *buffer, *content;
    int bufoffset;

    char *chunk;
    int chunkoffset;
    unsigned int chunklen;

    unsigned int buflen;
    struct {
        int path, version, method;
        int content_length;
        char *host;
        int chunked;
        int chunked_state;
    } headers;
    LIST_ENTRY(_http) entries;
} http;

typedef int (*handler_t)(http *);

typedef struct {
    const char *path;
    handler_t handler;
} route_t;

enum {
    HTTP_READ_HEADER,
    HTTP_READ_BODY,
    HTTP_CODE_MIN = 400
};

enum {
    HTTP_CHUNK_HEADER,
    HTTP_CHUNK_CONTENT,
    HTTP_CHUNK_CONTENT_END
};

void reset_headers(http *r, int keep_alive) {
    if(keep_alive && r->buffer) {
        free(r->buffer);
    }

    r->buffer = r->content = NULL;
    r->buflen = 0;
    r->bufoffset = 0;
    r->chunk = NULL;

    r->chunkoffset = r->chunklen = 0;

    memset(&r->headers, 0, sizeof(r->headers));
}

http *new_http(int fd) {
    http *r = (http *)malloc(sizeof(http));
    socklen_t addrlen = sizeof(r->address);

    r->fd = fd;
    r->state = 0;

    if(getpeername(fd, (struct sockaddr *)&r->address, &addrlen) == -1) goto err;
    if(addrlen != 16) goto err;

    if(permitted_address != inet_addr("0.0.0.0") &&
        r->address.sin_addr.s_addr != permitted_address) return NULL;

    reset_headers(r, 0);

    return r;

    err:
    if(r)
        free(r);
    return NULL;
}

void delete_http(http *r) {
    close(r->fd);
    if(r->buffer)
        free(r->buffer);
    free(r);
}

int recv_buffer(http *h, int padding) {
    char buf[0x8000];
    int n = recv(h->fd, buf, sizeof(buf), 0);
    printf("recv: %05d (%p) -> 0x%08x B\n", h->fd, h, n + h->buflen);
    if(n <= 0) {
        h->state = 400;
        return 0;
    }
    h->buffer = (char *)realloc(h->buffer, n + h->buflen + (padding ? 1 : 0));
#if DEBUG
    if(h->buffer == NULL) {
        printf("Triggered! %p %p\n", n, h->buffer + h->buflen);
    }
#endif
    memcpy(h->buffer + h->buflen, buf, n);
    h->bufoffset = h->buflen;
    h->buflen += n;
    if(padding)
        h->buffer[h->buflen] = '\0';
    return 1;
}

char *end_of_header(http *r) {
    char *result = strstr(r->buffer, "\n\n");
    if(result) {
        return result + 2;
    }
    result = strstr(r->buffer, "\r\n\r\n");
    if(result) {
        return result + 4;
    }
    return NULL;
}

char *trim(char *c) {
    while(isspace(*c)) {
        c++;
    }
    char *d = c + strlen(c) - 1;
    while(isspace(*d)) {
        d--;
    }
    if(isspace(d[1])) {
        d[1] = '\0';
    }
    return c;
}

int parse_http_line(http *r, char *line) {
    char *path = strchr(line, ' ');
    if(!path) return 0;
    *path++ = '\0';
    r->headers.path = path - r->buffer;
    r->headers.method = line - r->buffer;
    char *version = strchr(path, ' ');
    if(!version) return 0;
    *version++ = '\0';
    r->headers.version = version - r->buffer;
    // printf("%s %s\n", line, path);
    return 1;
}

int parse_header_line(http *r, char *line) {
    if(!strlen(line)) return 1;

    char *value = strchr(line, ':');
    if(!value) return 0;
    *value++ = '\0';

    char *key = trim(line);
    value = trim(value);

    if(!strcasecmp(key, "Content-Length")) {
        r->headers.content_length = atoi(value);
    }
    else if(!strcasecmp(key, "Host")) {
        r->headers.host = value;
    }
    else if(!strcasecmp(key, "Transfer-Encoding")) {
        if(!strcasecmp(value, "chunked")) {
            r->headers.chunked = 1;
        }
    }
    return 1;
}

char *my_memchr(char *c, int n, char x) {
    for(int i = 0; i < n; i++) {
        if(c[i] == x) return c + i;
    }
    return NULL;
}

void parse_header(http *r) {
    r->content = end_of_header(r);
    r->bufoffset = r->content - r->buffer;
    char *c = r->buffer, *line;
    int first_line = 1;
    line = c;
    while(c < r->content) {
        switch(*c) {
            case '\r':
            case '\n':
                *c = '\0';
                if(first_line)
                    parse_http_line(r, line);
                else
                    parse_header_line(r, line);
                first_line = 0;
                line = c + 1;
                break;
            default:
                break;
        }
        c++;
    }
}

// Used for spraying
int process_chunked(http *r) {
    char *endptr, *next_line;
    int new_len, recvlen;
    int remain = r->buflen - r->bufoffset;
    if(remain <= 0) {
        return 0;
    }
    switch(r->headers.chunked_state) {
        case HTTP_CHUNK_HEADER:
            next_line = my_memchr(r->buffer + r->bufoffset, remain, '\n');
            if(next_line) {
                endptr = r->buffer + r->buflen;
                *next_line++ = '\0';
                r->chunklen = strtoul(r->buffer + r->bufoffset, &endptr, 16);
                if(r->chunklen >= 0x10000000U || (r->chunklen + r->buflen < r->chunklen)) {
                    r->state = 400;
                    return 0;
                }
                r->bufoffset = next_line + 1 - r->buffer;
                r->chunk = realloc(r->chunk, r->chunklen);
                r->chunkoffset = r->bufoffset;
                if(r->chunk == NULL) {
                    r->state = 400;
                    return 0;
                }
                if(r->chunklen == 0) {
                    return 1;
                }

                r->headers.chunked_state = HTTP_CHUNK_CONTENT;
                return 0;
            } else {
                r->state = 400;
                return 0;
            }
            puts("???");
            exit(0);
            break;

        case HTTP_CHUNK_CONTENT:
            recvlen = remain > r->chunklen ? r->chunklen : remain;
            memcpy(r->chunk + r->chunkoffset, r->buffer + r->bufoffset, recvlen);
            r->chunkoffset += recvlen;
            if(r->chunklen <= r->chunkoffset) {
                r->headers.chunked_state = HTTP_CHUNK_CONTENT_END;
            }
            r->bufoffset += recvlen;
            return 0;

        case HTTP_CHUNK_CONTENT_END:
            next_line = memchr(r->buffer + r->bufoffset, remain, '\n');
            new_len = r->bufoffset + r->chunklen + (r->buflen - r->bufoffset); // remaining
            if(!next_line) {
                r->state = 400;
                return 0;
            }
            r->bufoffset = next_line + 1 - r->buffer;
            r->buffer = realloc(r->buffer, new_len);
            if(!r->buffer) {
                r->state = 400;
                return 0;
            }
            memcpy(r->buffer + r->bufoffset + r->chunklen,
                r->buffer + r->bufoffset,
                r->buflen - r->bufoffset);
            memcpy(r->buffer + r->bufoffset, r->chunk, r->chunklen);
            r->buflen = new_len;
            r->headers.chunked_state = HTTP_CHUNK_HEADER;
            r->headers.content_length += r->chunklen;
            return 0;
        default:
            r->state = 500;
            return 0;
    }
}

void send_error(http *r) {
    char buf[0x1000];
    int l = sprintf(buf, "HTTP/1.1 %d Error\r\nConnection: close\r\nContent-Length: 5\r\n\r\nerror", r->state);
    send(r->fd, buf, l, 0);
}

void send_status(http *r, int code, char *text) {
    char buf[0x100];
    int len = sprintf(buf, "HTTP/1.1 %d ", code);
    send(r->fd, buf, len, 0);
    send(r->fd, text, strlen(text), 0);
    send(r->fd, "\r\n", 2, 0);
}

void send_sep(http *r) {
    send(r->fd, "\r\n", 2, 0);
}

void send_header(http *r, char *key, char *value) {
    send(r->fd, key, strlen(key), 0);
    send(r->fd, ": ", 2, 0);
    send(r->fd, value, strlen(value), 0);
    send(r->fd, "\r\n", 2, 0);
}

void end_response_headers(http *r) {
    send_header(r, "X-Powered-By", "ios-daemon "VERSION);
    send_sep(r);
}

void send_string(http *r, const char *str) {
    char buf[0x100];
    send_status(r, 200, "OK");
    sprintf(buf, "%d", strlen(str));
    send_header(r, "Content-Length", buf);
    end_response_headers(r);
    send(r->fd, str, strlen(str), 0);
}

int hello(http *r) {
    send_string(r, "hello world");
    r->state = 200;
    return 1;
}

int version(http *r) {
    send_string(r, VERSION);
    r->state = 200;
    return 1;
}

int echo(http *r) {
    send_status(r, 200, "OK");
    char buf[0x100];
    sprintf(buf, "%ld", r->buffer + r->buflen - r->content);
    send_header(r, "Content-Length", buf);
    send_sep(r);
    send(r->fd, r->content, r->buffer + r->buflen - r->content, 0);
    return 1;
}

route_t routes[] = {
    {"/", &hello},
    {"/version", &version},
    {"/echo", &echo}
};

void process_query(http *r) {
    route_t *found = NULL;
    for(int i = 0; i < sizeof(routes) / sizeof(routes[0]); i++) {
        route_t *route = &routes[i];
        if(!strcmp(route->path, r->buffer + r->headers.path)) {
            found = route;
            break;
        }
    }

    if(!found) {
        send_status(r, 404, "Not Found");
        send_header(r, "Content-Length", "9");
        end_response_headers(r);
        send(r->fd, "not found", 9, 0);
        r->state = 404;
        return;
    }

    if(!found->handler(r)) {
        char buf[0x100];
        send_status(r, 200, "OK");
        int content_len = r->buffer + r->buflen - r->content;
        sprintf(buf, "%d", content_len);
        send_header(r, "Content-Length", buf);
        end_response_headers(r);
        send(r->fd, r->content, content_len, 0);
        r->state = -1;
    } else {
        r->bufoffset = r->buflen;
        memcpy(r->buffer, r->buffer + r->bufoffset, r->buflen - r->bufoffset);
        r->buflen = r->buflen - r->bufoffset;
        r->bufoffset = r->chunkoffset = 0;
        memset(&r->headers, 0, sizeof(r->headers));
        r->state = HTTP_READ_HEADER;
    }
}

void process_http(http *r) {
    if(!recv_buffer(r, r->state == HTTP_READ_HEADER)) return;
    while(r->bufoffset < r->buflen && r->state < HTTP_CODE_MIN) {
        switch(r->state) {
        case HTTP_READ_HEADER:
            if(end_of_header(r)) {
                parse_header(r);
                if(r->headers.chunked) {
                    r->chunkoffset = r->content - r->buffer;
                    r->headers.chunked_state = HTTP_CHUNK_HEADER;
                    r->headers.content_length = 0;
                }
                r->state = HTTP_READ_BODY;
            } else {
                return;
            }
            break;
        case HTTP_READ_BODY:
            if(r->headers.chunked) {
                if(process_chunked(r))
                    process_query(r);
            }
            else if(r->buffer + r->buflen - r->content >= r->headers.content_length) {
                process_query(r);
            } else {
                return;
            }
            break;
        default:
            if(r->state >= HTTP_CODE_MIN)
                return;
            }
    }
}

int main() {
    fd_set readset, writeset;
    int server_fd;
    int max_fd = 0;
    struct _http *cur;
    struct timeval timeout;

    LIST_HEAD(http_entries, _http) header;
    struct sockaddr_in server_address = {0};
    socklen_t addrlen = sizeof(server_address);

    LIST_INIT(&header);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(getenv("PORT") ? getenv("PORT") : "80"));
    server_address.sin_addr.s_addr = inet_addr("0.0.0.0");

    permitted_address = inet_addr(getenv("REMOTE") ? getenv("REMOTE") : "0.0.0.0");

    alarm(240);
    setvbuf(stdout, 0, 2, 0);

    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_fd == -1) {
        perror("socket");
        return 1;
    }

    long option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if(bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))) {
        perror("bind");
        return 1;
    };

    if(listen(server_fd, 0)) {
        perror("listen");
        return 1;
    }

    getsockname(server_fd, (struct sockaddr *)&server_address, &addrlen);
    printf("Listening on %d...\n", htons(server_address.sin_port));

    #define UPDATE_MAX_FD(fd) { if(max_fd < fd) max_fd = fd; }

    UPDATE_MAX_FD(server_fd);

    while(1) {
        FD_ZERO(&readset);
        FD_ZERO(&writeset);

        FD_SET(server_fd, &readset);

        {
            LIST_FOREACH(cur, &header, entries) {
                FD_SET(cur->fd, &readset);
            }
        }

        timeout.tv_usec = 0;
        timeout.tv_sec = 1;

        if(select(max_fd + 1, &readset, &writeset, NULL, &timeout) < 0) {
            perror("select");
            return 1;
        }

        if(FD_ISSET(server_fd, &readset)) {
            struct sockaddr_in client_address;
            socklen_t addrlen = sizeof(client_address);
            int fd = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);
            if(fd == -1) {
                perror("accept");
                return 1;
            }
            cur = new_http(fd);
            if(!cur) {
                close(fd);
                continue;
            }
            UPDATE_MAX_FD(fd);
            LIST_INSERT_HEAD(&header, cur, entries);
            continue;
        }

        {
            LIST_FOREACH(cur, &header, entries) {
                if(FD_ISSET(cur->fd, &readset)) {
                    process_http(cur);
                    break;
                }
            }
        }

        {
            cur = LIST_FIRST(&header);
            while(cur) {
                struct _http *next = LIST_NEXT(cur, entries);
                if(cur->state == -1 || cur->state >= HTTP_CODE_MIN) {
                    if(cur->state != -1) {
                        send_error(cur);
                    }
                    LIST_REMOVE(cur, entries);
                    delete_http(cur);
                }
                cur = next;
            }
        }
    }
}
