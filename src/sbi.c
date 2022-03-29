#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fault-local.h"
#include "../include/sbi.h"
#include "../include/runner.h"

#define MAX(a,b) a > b ? a : b

precat_t * precat_list = NULL;

precat_t *
sort_rank_sbi ();

precat_t *
find_max_precat ();

void
predicate (char * dir_path);

void
free_precat (precat_t * precat);

void
print_result_sbi (precat_t * sorted_list);

void
add_precat_list (precat_t * precat);

unsigned total_branch;

void
sbi (char * dir_path) {

	predicate(dir_path);
	precat_t * sorted_list = sort_rank_sbi();
	printf("Statistical Bug Isolation\n");
	printf("Total Number of Branch: %u\n", total_branch);
	printf("Total Number of Bcov Files: %d\n", bcovc);
	printf("Number of Test Failure: %d\nNumber of Test Pass: %d\n", fn, pn);

	print_result_sbi(sorted_list);
	free_precat(sorted_list);
}

void
predicate (char * dir_path) {

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

	                        predicate(inner_dir_path);
	                        free(inner_dir_path);
	                } else {

				unsigned int branch;
				if (strstr(ep->d_name, ".branch") && strlen(strstr(ep->d_name, ".branch")) == 7) {

					sscanf(ep->d_name, "%u", &branch);

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
					float suspect = 0.0f;
					if (pass_n + fail_n != 0) {
						suspect = fail_n / (fail_n + pass_n);
					}

					precat_t * precat = malloc(sizeof(precat_t));
					precat->branch = branch;
					precat->sus = suspect;
					precat->next = NULL;

					add_precat_list(precat);

	                        	char * suscon_path = malloc(sizeof(char) * (parrent_len+child_len+11));
	                        	sprintf(suscon_path, "%s/%d.ssus", dir_path, branch);
					FILE * suscon_fp = fopen(suscon_path, "wb");

					fwrite(&suspect, 1, 4, suscon_fp);

					fclose(suscon_fp);
					free(suscon_path);

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
add_precat_list (precat_t * precat) {

	if (precat_list == NULL) {
		precat_list = precat;
		return;
	}

	precat_t * sc = precat_list;
	while (sc->next != NULL) {
		sc = sc->next;
	}

	sc->next = precat;
}


precat_t *
sort_rank_sbi () {	
	precat_t * sorted_list = NULL;

	precat_t * max_precat;
	while ((max_precat = find_max_precat(precat_list)) != NULL) {

		precat_t * sc = precat_list;

		if (sc == max_precat) {

			// (sc == max_precat) => (sc2)
			precat_list = precat_list->next;
		}
		else if (max_precat->next != NULL) {

			// (sc) => (max_precat) => (sc2)
			while (sc->next != max_precat) {
				sc = sc->next;
			} 
			precat_t * sc2 = max_precat->next; 
			sc->next = sc2;
		}
		else {
		
			// (sc2) => (max_precat) => null
			precat_t * sc2 = precat_list;
			while (sc2->next != max_precat) {
				sc2 = sc2->next;
			}
			sc2->next = NULL;
		}
		max_precat->next = NULL;

		if (sorted_list == NULL) {
			sorted_list = max_precat;
		}
		else {
			precat_t * sc2 = sorted_list; 
			while (sc2->next != NULL) {
				sc2 = sc2->next;
			}
			sc2->next = max_precat;
		}
	}
	return sorted_list;
}

void
free_precat (precat_t * precat) {

	precat_t * next;
	while (precat != NULL) {
	        next = precat->next;
	        free(precat);
	        precat = next;
	}
}

precat_t *
find_max_precat() {

	precat_t * max_precat = NULL;
	precat_t * sc = precat_list;

	while (sc != NULL) {

		if (max_precat == NULL) {
			max_precat = sc;
		}
		else if (max_precat->sus < sc->sus) {
			max_precat = sc;
		}

		sc = sc->next;
	}

	return max_precat;
}

void
print_result_sbi (precat_t * sorted_list) {
	precat_t * sc = sorted_list;
	while (sc != NULL) {
		printf("branch %u \nsuspiciousness:%.2f\n\n", sc->branch, sc->sus);
		sc = sc->next;
	}
}
