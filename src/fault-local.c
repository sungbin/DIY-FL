#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fault-local.h"
#include "../include/runner.h"
#include "../include/tarantula.h"
#include "../include/sbi.h"

#define MAX(a,b) a > b ? a : b

int fn=0;
int pn=0;
int bcovc = 0;

void
write_branch_files (char * dir_path);

void
false_localization (char * dir_path) {

	write_branch_files(dir_path);
	tarantula(dir_path);
	sbi(dir_path);
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

	                        write_branch_files(inner_dir_path);
	                        free(inner_dir_path);
	                } else {

				if(strstr(ep->d_name, ".bcov") != NULL) {

					if ((ep->d_name)[1] != '-') {
                                               continue;
                                        }

                                        if ((ep->d_name)[0] == 'p' && (ep->d_name)[1] == '-') {
                                               pn++;
                                        }

                                        else if ((ep->d_name)[0] == 'f' && (ep->d_name)[1] == '-') {
                                               fn++;
                                        }
						
	                        	int parrent_len = strlen(dir_path);
	                        	int child_len = strlen(ep->d_name);
	                        	char * inner_file_path = malloc(sizeof(char) * (parrent_len+child_len+2));
	                        	sprintf(inner_file_path, "%s/%s",dir_path,ep->d_name);

					bcov* blist = read_bcov(inner_file_path);
					bcovc ++;

					bcov* _blist = blist;
					while (_blist != NULL) {

						unsigned int branch = _blist->branch;

						//printf("FAIL: %d\n", _blist->fail);

						char * branch_fname = malloc(sizeof(char) * (parrent_len + 23));

						sprintf(branch_fname, "%s/%u.branch", dir_path, branch);

						FILE * fp = fopen(branch_fname, "ab");
						
						if ((ep->d_name)[0] == 'p') {
							int n = 0;
							fputc(n, fp);
						}
						else if ((ep->d_name)[0] == 'f') {
							int n = 1;
							fputc(n, fp);
						}
						else { 

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

}
