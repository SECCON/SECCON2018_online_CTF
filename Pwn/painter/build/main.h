typedef bool (*cmdhandler_t)(struct image **, const char *args);

typedef struct {
	const char *name;
	cmdhandler_t handler;
} cmditem_t;
