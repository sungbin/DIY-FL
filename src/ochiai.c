#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/fault-local.h"
#include "../include/ochiai.h"
#include "../include/runner.h"

#define MAX(a,b) a > b ? a : b

osus_t * osus_list = NULL;

osus_t *
sort_rank_ochiai ();

osus_t *
find_max_osus ();

void
moleq (char * dir_path);

void
free_osus (osus_t * osus);

void
print_result_ochiai (osus_t * sorted_list);

void
add_osus_list (osus_t * osus);

unsigned total_branch;

void
ochiai (char * dir_path) {

	moleq(dir_path);
	osus_t * sorted_list = sort_rank_ochiai();
	printf("Ochiai\n");
	printf("Total Number of Branch: %u\n", total_branch);
	printf("Total Number of Bcov Files: %d\n", bcovc);
	printf("Number of Test Failure: %d\nNumber of Test Pass: %d\n", fn, pn);

	print_result_ochiai(sorted_list);
	free_osus(sorted_list);
}

void
moleq (char * dir_path) {

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

	                        moleq(inner_dir_path);
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
					if (fail_n + pass_n != 0) {
						suspect = fail_n / sqrtf(fn*(fail_n + pass_n));
					}

					osus_t * osus = malloc(sizeof(osus_t));
					osus->branch = branch;
					osus->sus = suspect;
					osus->next = NULL;

					add_osus_list(osus);

	                        	char * suscon_path = malloc(sizeof(char) * (parrent_len+child_len+11));
	                        	sprintf(suscon_path, "%s/%d.osus", dir_path, branch);
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
add_osus_list (osus_t * osus) {

	if (osus_list == NULL) {
		osus_list = osus;
		return;
	}

	osus_t * sc = osus_list;
	while (sc->next != NULL) {
		sc = sc->next;
	}

	sc->next = osus;
}


osus_t *
sort_rank_ochiai () {	
	osus_t * sorted_list = NULL;

	osus_t * max_osus;
	while ((max_osus = find_max_osus(osus_list)) != NULL) {

		osus_t * sc = osus_list;

		if (sc == max_osus) {

			// (sc == max_osus) => (sc2)
			osus_list = osus_list->next;
		}
		else if (max_osus->next != NULL) {

			// (sc) => (max_osus) => (sc2)
			while (sc->next != max_osus) {
				sc = sc->next;
			} 
			osus_t * sc2 = max_osus->next; 
			sc->next = sc2;
		}
		else {
		
			// (sc2) => (max_osus) => null
			osus_t * sc2 = osus_list;
			while (sc2->next != max_osus) {
				sc2 = sc2->next;
			}
			sc2->next = NULL;
		}
		max_osus->next = NULL;

		if (sorted_list == NULL) {
			sorted_list = max_osus;
		}
		else {
			osus_t * sc2 = sorted_list; 
			while (sc2->next != NULL) {
				sc2 = sc2->next;
			}
			sc2->next = max_osus;
		}
	}
	return sorted_list;
}

void
free_osus (osus_t * osus) {

	osus_t * next;
	while (osus != NULL) {
	        next = osus->next;
	        free(osus);
	        osus = next;
	}
}

osus_t *
find_max_osus() {

	osus_t * max_osus = NULL;
	osus_t * sc = osus_list;

	while (sc != NULL) {

		if (max_osus == NULL) {
			max_osus = sc;
		}
		else if (max_osus->sus < sc->sus) {
			max_osus = sc;
		}

		sc = sc->next;
	}

	return max_osus;
}

void
print_result_ochiai (osus_t * sorted_list) {
	osus_t * sc = sorted_list;
	while (sc != NULL) {
		printf("branch %u \nsuspiciousness:%.2f\n\n", sc->branch, sc->sus);
		sc = sc->next;
	}
}
