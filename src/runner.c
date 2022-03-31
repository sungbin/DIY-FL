#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "../include/runner.h"
#include "../include/fault-local.h"

void
explore_dir_with_runner (char * input_dir_path, char * sub_dir, runner_error_code (*_runner)(char*, char*, char*, int), char * target_path, char * output_dir_path, int recursive_dir, int is_bcov);

int
mkdirs (char * dir_path);

char *
parent_dir (char * dir_path);

unsigned int total_branch;

runner_error_code
get_error (enum E_Type type, int exit_code);

void
multiple_runner (char * target_path, char * input_dir_path, char * output_dir_path, int is_bcov) {

	int is_dir = 0;
	explore_dir_with_runner(input_dir_path, "",  &runner, target_path, output_dir_path, is_dir, is_bcov);
}

int bcnt = 0;

runner_error_code
runner (char* target_path, char* input_path, char *output_path, int is_bcov) {

	char *output_err_path = malloc((strlen(output_path) + 5) * sizeof(char));
	sprintf(output_err_path,"%s.err",output_path);

	pid_t pid = fork();
        if (pid < 0) { 
		runner_error_code error_code = get_error(E_FORK, 0);
	       return error_code;
        }

	char * bcov_path;
	char * bcov_env;
	char * f_name;
	char * d_name;
	if (is_bcov) {

		f_name = basename(input_path);
		d_name = dirname(strdup(output_path));
		bcov_path = malloc(sizeof(char) * (strlen(f_name) + strlen(d_name) + 11));
		sprintf(bcov_path, "%s/%s.bcov", d_name, f_name);

		bcov_env = malloc(sizeof(char) * (strlen(bcov_path) + 20));
		sprintf(bcov_env, "BCOV_PATH=%s", bcov_path);

	}

        /* Child process */
        if (pid == 0) { 

	       int input_fd = open(input_path, O_RDONLY);
	       int out_fd = open(output_path, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	       int out_err_fd = open(output_err_path, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

	       if (dup2(input_fd, STDIN_FILENO) == -1) {
		       perror("dup2-input");
	       }
	       if (dup2(out_fd, STDOUT_FILENO) == -1) {
		       perror("dup2-output");
	       }
	       if (dup2(out_err_fd, STDERR_FILENO) == -1) {
		       perror("dup2-outerr");
	       }
		if(is_bcov) {
	       		putenv(bcov_env);
		}

	       execl(target_path, target_path, NULL);
	       _exit(1);
        }

	/* Parent process */
        int status = 0;
        int start, end;
        start = ((int)clock()) / CLOCKS_PER_SEC;
        end = ((int)clock()) / CLOCKS_PER_SEC;

        while ((end - start) < 10) {
		int w = waitpid(pid, &status, WNOHANG);
		if (w != 0) {
			break;
		}
	       end = ((int)clock()) / CLOCKS_PER_SEC;
	}

	free(output_err_path);

	int exit_stated = WEXITSTATUS(status);
	if ((end - start) >= 10) {
		// Time out Kill
		kill(pid, SIGKILL);
		wait(&status);
		fprintf(stderr, "kill %d (%s)\n",++bcnt, input_path);
		runner_error_code error_code = get_error(E_TIMEOUT_KILL, exit_stated);

		if (is_bcov) {
			char * new_name = malloc(sizeof(char) * (strlen(f_name) + strlen(d_name) + 13) );
			sprintf(new_name, "%s/f-%s.bcov", d_name, f_name);
			rename(bcov_path, new_name);

			free(new_name);
			free(bcov_path);
			free(bcov_env);
			free(d_name);
		}
		return error_code;
	}
	else {
		// normally exit
		runner_error_code error_code = get_error(NO_ERROR, exit_stated);

		if (is_bcov) {
			char * new_name = malloc(sizeof(char) * (strlen(f_name) + strlen(d_name) + 13));
			if (error_code.exit_code == 0) {
				sprintf(new_name, "%s/p-%s.bcov", d_name, f_name);
				rename(bcov_path, new_name);
			}
			else {
				sprintf(new_name, "%s/f-%s.bcov", d_name, f_name);
				rename(bcov_path, new_name);
			}
			free(new_name);
			free(bcov_path);
			free(bcov_env);
			free(d_name);
		}
			
		return error_code;
	}
}

runner_error_code
get_error (enum E_Type type, int exit_code) {
	runner_error_code _code;
	_code.type = type;
	_code.exit_code = exit_code;

	return _code;
}

void
explore_dir_with_runner (char * input_dir_path, char * sub_dir, runner_error_code (*_runner)(char*, char*, char*, int), char * target_path, char * output_dir_path, int recursive_dir, int is_bcov) {

	char * inner_dir_path;

	int sub_dir_len = strlen(sub_dir);
	if(sub_dir_len > 0) {

		int inner_dir_path_len = strlen(input_dir_path)+sub_dir_len+2;
		inner_dir_path = malloc(sizeof(char) * inner_dir_path_len);
		snprintf(inner_dir_path, inner_dir_path_len, "%s/%s", input_dir_path, sub_dir);

	}
	else {

		inner_dir_path = strdup(input_dir_path);

	}

        DIR * dp = opendir(inner_dir_path);
        struct dirent *ep;

        if (dp != NULL) {
                while ((ep = readdir(dp))) {
                        if (strcmp(ep->d_name,".") == 0) {
                                continue;
                        }
                        if (strcmp(ep->d_name,"..") == 0) {
                                continue;
                        }

			char * sub_child_dir;
			if (sub_dir_len > 0) {

				int sub_child_dir_len = strlen(sub_dir)+strlen(ep->d_name)+2;
				sub_child_dir = malloc(sizeof(char) * sub_child_dir_len);
				snprintf(sub_child_dir, sub_child_dir_len, "%s/%s", sub_dir, ep->d_name);

			}
			else {

				sub_child_dir = strdup(ep->d_name);

			}

			char * input_path, * program_out_path;
			int input_path_len, program_out_path_len;

			input_path_len = strlen(input_dir_path)+strlen(sub_child_dir)+2;
			program_out_path_len = strlen(output_dir_path)+strlen(sub_child_dir)+2;

			input_path = malloc(sizeof(char) * input_path_len);
			program_out_path = malloc(sizeof(char) * program_out_path_len);

			snprintf(input_path, input_path_len, "%s/%s", input_dir_path, sub_child_dir);
			snprintf(program_out_path, program_out_path_len, "%s/%s", output_dir_path, sub_child_dir);

			char * p_dir = parent_dir(program_out_path);
			if (mkdirs(p_dir)) {
				fprintf(stderr, "ERROR: cannot mkdirs %s\n", dirname(dirname(program_out_path)));
			}
			free(p_dir);

			runner_error_code error_code = (*_runner)(target_path, input_path, program_out_path, is_bcov);

			if (error_code.type == NO_ERROR) {
				
				if (error_code.exit_code == 1) {
					//fprintf(stderr,"BufferOver run: %s\n", input_path);
				}
				else {
					//fprintf(stderr,"No ERROR: %s\n", input_path);
				}
			}
			else {
				//fprintf(stderr,"Fail to run with: %s\n", input_path);
			}

                        if (recursive_dir && ep->d_type == DT_DIR) {

				explore_dir_with_runner(input_dir_path, "",  &runner, target_path, output_dir_path, 0, is_bcov);

                        }

			free(sub_child_dir);
			free(input_path);
			free(program_out_path);
                }
                closedir(dp);
        }
        else {

		fprintf(stderr, "Couldn't open the directory: %s \n", inner_dir_path);
                exit(1);

        }

	free(inner_dir_path);

}

// need to free()
char *
parent_dir (char * path) {

	char * p = strrchr(path, '/');
	if (p == 0x0) {
		fprintf(stderr,"strrchr error!\n");
		exit(1);
	}
	int lidx = p - path;
	char * parent_dir_path = malloc((lidx+1) * sizeof(char));
	strncpy(parent_dir_path, path, lidx);
	parent_dir_path[lidx] = 0x0;

	return parent_dir_path;
}

int
mkdirs (char * dir_path) {

	char * p_dir;
	if (access(dir_path, F_OK) == 0) {
		/* already exists*/
		return 0;
	}
	else if (access((p_dir=parent_dir(dir_path)), F_OK) == 0) {
		free(p_dir);
		return mkdir(dir_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	}
	else {

		int val = mkdirs(p_dir) || mkdir(dir_path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		free(p_dir);
		return val;
	}
}

void
free_bcov (bcov * b) {
	bcov * next;
	while (b != 0x0) {
		next = b->next;
		free(b);
		b = next;
	}
}


bcov*
read_bcov (char * bcov_path) {

	FILE * bcov_fp = fopen(bcov_path, "rb");
	fread(&total_branch, 1, 4, bcov_fp);
	int buf_len = 0;

	bcov * b = 0x0;
	bcov * last_b = 0x0;

	do {
		unsigned int branch;
		buf_len = fread(&branch, 1, 4, bcov_fp);
		if (buf_len != 4) break;

		unsigned int des_len;
		buf_len = fread(&des_len, 1, 4, bcov_fp);
		if (buf_len != 4) break;

		char des[1024];
		buf_len = fread(des, 1, des_len, bcov_fp);
		if (buf_len != des_len) break;

		//printf("read: %s\nbranch:%u, des_len:%u, des: %s\n",bcov_path, branch, des_len, des);
		
		if (b  == 0x0) {
			b = malloc(sizeof(bcov));
			b->branch = branch;
			b->des_len = des_len;
			strcpy(b->des, des);
			b->next = 0x0;
			last_b = b;
		}
		else {

			// prevent duplicates
			bcov * _b = b;
			int dupl = 0;

			// TODO: remove
			
			while (_b != 0x0) {
				
				if (_b->branch == branch) {
					dupl = 1; break;
				}
				/*
				*/
				_b = _b->next;
			}
			if (dupl) continue;
			

			_b = malloc(sizeof(bcov));
			_b->branch = branch;
			_b->des_len = des_len;
			strcpy(b->des, des);
			_b->next = 0x0;
			last_b->next = _b;
			last_b = _b;
		}

		//update_branch_set(b_set, branch);

	} while(buf_len > 0);

	fclose(bcov_fp);
	return b;
}

