void
bcov_tarantula (char * dir_path);

typedef struct _suscon_t {

	unsigned int branch;
	float sus;
	float con;
	struct _suscon_t * next;

} suscon_t;

extern int fn;
extern int pn;
extern int bcovc;
