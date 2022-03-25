enum E_Type { 
	NO_ERROR,
	E_FORK,
	E_TIMEOUT_KILL,
};

typedef struct _runner_error_code {
	enum E_Type type;
	int exit_code;
} runner_error_code;

runner_error_code
runner (char* target_path, char* input_path, char *output_path, int is_bcov);

void
multiple_runner (char * target_path, char * input_dir_path, char * output_dir_path, int is_bcov);

typedef struct _bcov_set {
        unsigned int branch;
        struct _bcov_set * next;

} bcov_set;

typedef struct _bcov {
        unsigned int branch;
        unsigned int des_len;
        char des[1024];
        struct _bcov * next;
} bcov;

void
free_bcov (bcov * b);

bcov*
read_bcov (char * bcov_path);
