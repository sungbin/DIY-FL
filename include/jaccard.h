void
jaccard (char * dir_path);

typedef struct _jsusp_t {

	unsigned int branch;
	float sus;
	struct _jsusp_t * next;

} jsusp_t;
