#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/runner.h"
#include "../include/fault-local.h"

int 
main (int argc, char* argv[])
{

	int is_bcov;
	if (argc < 4) {
                fprintf(stderr, "ERROR: THE NUMBER OF AGURMENTS MUST BE BIGGER THAN THREE!\n");
                exit(1);
        }
	else if (argc == 4) {

		// argv[1]: input_dir_path;
		// argv[2]: output_dir_path;
		// argv[3]: target_path;
		is_bcov = 0;
		multiple_runner(argv[3], argv[1], argv[2], is_bcov);
	}
	else if (strcmp(argv[1], "-bcov") == 0) {

		is_bcov = 1;
		multiple_runner(argv[4], argv[2], argv[3], is_bcov);
		false_localization(argv[3]);
	}
	else if (strcmp(argv[4], "-bcov") == 0) {

		is_bcov = 1;
		multiple_runner(argv[3], argv[1], argv[2], is_bcov);
		false_localization(argv[2]);
	}
	else {
                fprintf(stderr, "ERROR: THE NUMBER OF AGURMENTS MUST BE BIGGER THAN THREE!\n");
                exit(1);
	}

	return 0;
}
