#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fault-local.h"
#include "../include/tarantula.h"
#include "../include/runner.h"

#define MAX(a,b) a > b ? a : b

suscon_t * suscon_list = NULL;

suscon_t *
sort_rank_tarantula ();

suscon_t *
find_max_suscon ();

void
suspicious_confidence (char * dir_path);

void
free_suscon (suscon_t * suscon);

void
print_tarantula (suscon_t * sorted_list);

void
add_suscon_list (suscon_t * suscon);

unsigned total_branch;

void
tarantula (char * dir_path) {

	suspicious_confidence(dir_path);
	suscon_t * sorted_list = sort_rank_tarantula();
	printf("Tarantula\n");
	printf("Total Number of Branch: %u\n", total_branch);
	printf("Total Number of Bcov Files: %d\n", bcovc);
	printf("Number of Test Failure: %d\nNumber of Test Pass: %d\n", fn, pn);

	print_tarantula(sorted_list);
	free_suscon(sorted_list);
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
					float p_pass = pass_n / pn;
					float p_fail = fail_n / fn;
					float suspect;
					if (p_fail + p_pass != 0) {
						suspect = p_fail / (p_fail + p_pass);
					}
					else {
						suspect = 0.0f;
					}
					float conf = MAX(p_fail, p_pass);

					//printf("%u %.f %.f (%u, %u)\n", branch, pass_n, fail_n, pn, fn);

					suscon_t * suscon = malloc(sizeof(suscon_t));
					suscon->branch = branch;
					suscon->sus = suspect;
					suscon->con = conf;
					suscon->next = NULL;

					add_suscon_list(suscon);

					// write suscon
	                        	char * suscon_path = malloc(sizeof(char) * (parrent_len+child_len+11));
	                        	sprintf(suscon_path, "%s/%d.suscon", dir_path, branch);
					FILE * suscon_fp = fopen(suscon_path, "wb");

					fwrite(&suspect, 1, 4, suscon_fp);
					fwrite(&conf, 1, 4, suscon_fp);

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
add_suscon_list (suscon_t * suscon) {

	if (suscon_list == NULL) {
		suscon_list = suscon;
		return;
	}

	suscon_t * sc = suscon_list;
	while (sc->next != NULL) {
		sc = sc->next;
	}

	sc->next = suscon;
}


suscon_t *
sort_rank_tarantula () {	
	suscon_t * sorted_list = NULL;

	suscon_t * max_suscon;
	while ((max_suscon = find_max_suscon(suscon_list)) != NULL) {

		suscon_t * sc = suscon_list;

		if (sc == max_suscon) {

			// (sc == max_suscon) => (sc2)
			suscon_list = suscon_list->next;
		}
		else if (max_suscon->next != NULL) {

			// (sc) => (max_suscon) => (sc2)
			while (sc->next != max_suscon) {
				sc = sc->next;
			} 
			suscon_t * sc2 = max_suscon->next; 
			sc->next = sc2;
		}
		else {
		
			// (sc2) => (max_suscon) => null
			suscon_t * sc2 = suscon_list;
			while (sc2->next != max_suscon) {
				sc2 = sc2->next;
			}
			sc2->next = NULL;
		}
		max_suscon->next = NULL;

		if (sorted_list == NULL) {
			sorted_list = max_suscon;
		}
		else {
			suscon_t * sc2 = sorted_list; 
			while (sc2->next != NULL) {
				sc2 = sc2->next;
			}
			sc2->next = max_suscon;
		}
	}
	return sorted_list;
}

void
free_suscon (suscon_t * suscon) {

	suscon_t * next;
	while (suscon != NULL) {
	        next = suscon->next;
	        free(suscon);
	        suscon = next;
	}
}

suscon_t *
find_max_suscon() {

	suscon_t * max_suscon = NULL;
	suscon_t * sc = suscon_list;

	while (sc != NULL) {

		if (max_suscon == NULL) {
			max_suscon = sc;
		}
		else if (max_suscon->sus < sc->sus) {
			max_suscon = sc;
		}
		else if (max_suscon->sus == sc->sus && max_suscon->con < sc->con) {
			max_suscon = sc;
		}

		sc = sc->next;
	}

	return max_suscon;
}

void
print_tarantula (suscon_t * sorted_list) {
	suscon_t * sc = sorted_list;
	while (sc != NULL) {
		printf("branch %u \nsuspiciousness:%.2f, confidence:%.2f\n\n", sc->branch, sc->sus, sc->con);
		sc = sc->next;
	}
}
