#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fault-local.h"
#include "../include/runner.h"

#define MAX(a,b) a > b ? a : b

char * top_path = 0x0;
float top_susp = 0;
float top_conf = 0;

int fn=0;
int pn=0;
int nn=0;
int temp = 0;

int bcovc = 0;

unsigned total_branch;

void
rank (char * dir_path);

int
_rank (char * dir_path);

void
write_branch_files (char * dir_path);

void
suspicious_confidence (char * dir_path);

// main for test TODO: remove
/*
int
main () {
	bcov_tarantula("../bin/result");
}
*/

void
bcov_tarantula (char * dir_path) {

	write_branch_files(dir_path);
	suspicious_confidence(dir_path);
	rank(dir_path);
	printf("gloval: %u\n", total_branch);
	printf("fn: %d, pn: %d, nn: %d\n", fn, pn, nn);
	printf("bcovc: %d\n", bcovc);
}

void
write_branch_files (char * dir_path) {

        DIR * dp = opendir(dir_path);
        struct dirent *ep;

        if (dp != NULL)
        {
                while ((ep = readdir(dp))) {
                        if (strcmp(ep->d_name,".") == 0) {
                                continue;
                        }
                        if (strcmp(ep->d_name,"..") == 0) {
                                continue;
                        }

                        if (ep->d_type == DT_DIR) {

                                int parrent_len = strlen(dir_path);
                                int child_len = strlen(ep->d_name);

                                char * inner_dir_path = malloc(sizeof(char) * (parrent_len+child_len+2));
                                sprintf(inner_dir_path, "%s/%s",dir_path,ep->d_name);
                                //printf(" \t DIR: %s\n", inner_dir_path);

                                write_branch_files(inner_dir_path);
                                free(inner_dir_path);
                        } else {

				if(strstr(ep->d_name, ".bcov") != 0x0) {
						
					if ((ep->d_name)[1] != '-') {
						continue;
					}

					if ((ep->d_name)[0] == 'p' && (ep->d_name)[1] == '-') {
						pn++;
					}

					else if ((ep->d_name)[0] == 'f' && (ep->d_name)[1] == '-') {
						fn++;
                                        }
					else {
						nn++;
					}

                                	int parrent_len = strlen(dir_path);
                                	int child_len = strlen(ep->d_name);
                                	char * inner_file_path = malloc(sizeof(char) * (parrent_len+child_len+2));
                                	sprintf(inner_file_path, "%s/%s",dir_path,ep->d_name);

					bcov* blist = read_bcov(inner_file_path);
					bcovc ++;

					bcov* _blist = blist;
					while (_blist != 0x0) {

						unsigned int branch = _blist->branch;

						char * branch_fname = malloc(sizeof(char) * (parrent_len + 23));

						sprintf(branch_fname, "%s/%u.branch", dir_path, branch);

						FILE * fp = fopen(branch_fname, "ab");
						if ((ep->d_name)[0] == 'p' && (ep->d_name)[1] == '-') {
							int n = 0;
							fputc(n, fp);
						}
						else if ((ep->d_name)[0] == 'f' && (ep->d_name)[1] == '-') {
							int n = 1;
							fputc(n, fp);
						}
						else { 
							//exit code != 1
						}
						fclose(fp);
						free(branch_fname);
						_blist = _blist->next;
					}

					free_bcov(blist);
					free(inner_file_path);
				}
                        }
                }
                closedir(dp);
        }
        else {
                fprintf(stderr,"Couldn't open the directory: %s", dir_path);
                exit(1);
        }

	//closedir(dp);
}

void
suspicious_confidence (char * dir_path) {

	DIR * dp = opendir(dir_path);
        struct dirent *ep;

        if (dp != NULL)
        {
                while ((ep = readdir(dp))) {
                        if (strcmp(ep->d_name,".") == 0) {
                                continue;
                        }
                        if (strcmp(ep->d_name,"..") == 0) {
                                continue;
                        }

                        if (ep->d_type == DT_DIR) {

                                int parrent_len = strlen(dir_path);
                                int child_len = strlen(ep->d_name);

                                char * inner_dir_path = malloc(sizeof(char) * (parrent_len+child_len+2));
                                sprintf(inner_dir_path, "%s/%s",dir_path,ep->d_name);
                                //printf(" \t DIR: %s\n", inner_dir_path);

                                suspicious_confidence(inner_dir_path);
                                free(inner_dir_path);
                        } else {

				unsigned int branch;
				if(strstr(ep->d_name, ".branch") != 0x0) {

                                	int parrent_len = strlen(dir_path);
                                	int child_len = strlen(ep->d_name);
                                	char * inner_file_path = malloc(sizeof(char) * (parrent_len+child_len+2));
                                	sprintf(inner_file_path, "%s/%s",dir_path,ep->d_name);

					FILE * fp = fopen(inner_file_path, "rb");

					float pass_n = 0.0f;
					float fail_n = 0.0f;

					int b_size = 0;
					int n;
					while ((n = fgetc(fp)) != EOF) {

						if (n == 0) {
							pass_n ++;
						}
						else if (n == 1) {
							fail_n ++;
						}
					}
					float p_fail = pass_n / pn;
					float p_pass = fail_n / fn;
					float suspect;
					if (fail_n != 0) {
				        	suspect = p_fail / (p_fail + p_pass);
					}
					else {
						suspect = 0.0f;
					}

					//printf("suspiciousness of %s: %.2f\n", inner_file_path, suspect);
					//printf("p: %.0f, f: %.0f\n", pass_n, fail_n);
					
					char * susp_path = malloc(sizeof(char) * (parrent_len+child_len+2+7)); // 7 chars: ".suscon"
					sprintf(susp_path,"%s.suscon",inner_file_path);
					FILE * suscon_fp = fopen(susp_path, "wb");
					fwrite(&suspect, 1, 4, suscon_fp);
					float conf = MAX(p_fail, p_pass);
					fwrite(&conf, 1, 4, suscon_fp); //confidence

					fclose(suscon_fp);
					fclose(fp);
					free(inner_file_path);
				}
                        }
                }
                closedir(dp);
        }
        else {
                fprintf(stderr,"Couldn't open the directory: %s", dir_path);
                exit(1);
        }

}


void
rank (char * dir_path) {
	int r = 1;
	while(1) {
		top_path = 0x0;
		top_susp = 0;
		top_conf = 0;
		_rank(dir_path);
		if (top_path == 0x0) {
			break;
		}
		else {
			// fwrite: 1.rank
			// 1.rank contains: susp, conf, branch-buf
		}
	};
}

int
_rank (char * dir_path) {

	DIR * dp = opendir(dir_path);
        struct dirent *ep;

        if (dp != NULL)
        {
                while ((ep = readdir(dp))) {
                        if (strcmp(ep->d_name,".") == 0) {
                                continue;
                        }
                        if (strcmp(ep->d_name,"..") == 0) {
                                continue;
                        }

                        if (ep->d_type == DT_DIR) {

                                int parrent_len = strlen(dir_path);
                                int child_len = strlen(ep->d_name);

                                char * inner_dir_path = malloc(sizeof(char) * (parrent_len+child_len+2));
                                sprintf(inner_dir_path, "%s/%s",dir_path,ep->d_name);
                                //printf(" \t DIR: %s\n", inner_dir_path);

				_rank(inner_dir_path);
                                free(inner_dir_path);
                        } else {

				if(strstr(ep->d_name, ".suscon") != 0x0) {

                                	int parrent_len = strlen(dir_path);
                                	int child_len = strlen(ep->d_name);
                                	char * inner_file_path = malloc(sizeof(char) * (parrent_len+child_len+2));
                                	sprintf(inner_file_path, "%s/%s",dir_path,ep->d_name);

					FILE * fp = fopen(inner_file_path, "rb");
					float susp, conf;
					fread(&susp, 1, 4, fp);
					fread(&conf, 1, 4, fp);

					if (top_path == 0x0) {
						top_path = inner_file_path;
						top_conf = conf;
						top_susp = susp;
					}
					else {
						
						if (top_susp < susp) {
							free(top_path);
							top_path = inner_file_path;
							top_conf = conf;
							top_susp = susp;
						}
						else if (top_susp == susp && top_conf < conf) {
							free(top_path);
							top_path = inner_file_path;
							top_conf = conf;
							top_susp = susp;
						}
						else {
							free(inner_file_path);
						}
					}

					fclose(fp);
				}
                        }
                }
                closedir(dp);
        }
        else {
                fprintf(stderr,"Couldn't open the directory: %s", dir_path);
                exit(1);
        }


}
