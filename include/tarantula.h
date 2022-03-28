void
tarantula (char * dir_path);

typedef struct _suscon_t {

	unsigned int branch;
	float sus;
	float con;
	struct _suscon_t * next;

} suscon_t;
