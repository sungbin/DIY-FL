#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fault-local.h"
#include "../include/jaccard.h"
#include "../include/runner.h"

#define MAX(a,b) a > b ? a : b

jsusp_t * jsusp_list = NULL;

jsusp_t *
sort_rank_jaccard ();

jsusp_t *
find_max_jsusp ();

void
diaccuracy (char * dir_path);

void
free_jsusp (jsusp_t * jsusp);

void
print_result_jaccard (jsusp_t * sorted_list);

void
add_jsusp_list (jsusp_t * jsusp);

unsigned total_branch;

void
jaccard (char * dir_path) {

	diaccuracy(dir_path);
	jsusp_t * sorted_list = sort_rank_jaccard();
	printf("Jaccard\n");
	printf("Total Number of Branch: %u\n", total_branch);
	printf("Total Number of Bcov Files: %d\n", bcovc);
	printf("Number of Test Failure: %d\nNumber of Test Pass: %d\n", fn, pn);

	print_result_jaccard(sorted_list);
	free_jsusp(sorted_list);
}

void
diaccuracy (char * dir_path) {

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

	                        diaccuracy(inner_dir_path);
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
					if (fn + pass_n != 0) {
						suspect = fail_n / (fn + pass_n);
					}

					jsusp_t * jsusp = malloc(sizeof(jsusp_t));
					jsusp->branch = branch;
					jsusp->sus = suspect;
					jsusp->next = NULL;

					add_jsusp_list(jsusp);

	                        	char * suscon_path = malloc(sizeof(char) * (parrent_len+child_len+11));
	                        	sprintf(suscon_path, "%s/%d.jsus", dir_path, branch);
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
add_jsusp_list (jsusp_t * jsusp) {

	if (jsusp_list == NULL) {
		jsusp_list = jsusp;
		return;
	}

	jsusp_t * sc = jsusp_list;
	while (sc->next != NULL) {
		sc = sc->next;
	}

	sc->next = jsusp;
}


jsusp_t *
sort_rank_jaccard () {	
	jsusp_t * sorted_list = NULL;

	jsusp_t * max_jsusp;
	while ((max_jsusp = find_max_jsusp(jsusp_list)) != NULL) {

		jsusp_t * sc = jsusp_list;

		if (sc == max_jsusp) {

			// (sc == max_jsusp) => (sc2)
			jsusp_list = jsusp_list->next;
		}
		else if (max_jsusp->next != NULL) {

			// (sc) => (max_jsusp) => (sc2)
			while (sc->next != max_jsusp) {
				sc = sc->next;
			} 
			jsusp_t * sc2 = max_jsusp->next; 
			sc->next = sc2;
		}
		else {
		
			// (sc2) => (max_jsusp) => null
			jsusp_t * sc2 = jsusp_list;
			while (sc2->next != max_jsusp) {
				sc2 = sc2->next;
			}
			sc2->next = NULL;
		}
		max_jsusp->next = NULL;

		if (sorted_list == NULL) {
			sorted_list = max_jsusp;
		}
		else {
			jsusp_t * sc2 = sorted_list; 
			while (sc2->next != NULL) {
				sc2 = sc2->next;
			}
			sc2->next = max_jsusp;
		}
	}
	return sorted_list;
}

void
free_jsusp (jsusp_t * jsusp) {

	jsusp_t * next;
	while (jsusp != NULL) {
	        next = jsusp->next;
	        free(jsusp);
	        jsusp = next;
	}
}

jsusp_t *
find_max_jsusp() {

	jsusp_t * max_jsusp = NULL;
	jsusp_t * sc = jsusp_list;

	while (sc != NULL) {

		if (max_jsusp == NULL) {
			max_jsusp = sc;
		}
		else if (max_jsusp->sus < sc->sus) {
			max_jsusp = sc;
		}

		sc = sc->next;
	}

	return max_jsusp;
}

void
print_result_jaccard (jsusp_t * sorted_list) {
	jsusp_t * sc = sorted_list;
	while (sc != NULL) {
		printf("branch %u \nsuspiciousness:%.2f\n\n", sc->branch, sc->sus);
		sc = sc->next;
	}
}
