#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <sanitizer/coverage_interface.h>

#include "../include/trace-pc.h"

FILE * bcov_fp = 0x0;
unsigned int N = 0;  // Counter for the guards.
int minit = 0;

void
close_bcovfp () {
  if (bcov_fp != 0x0) {
    fclose(bcov_fp);
  }
}

extern void
__sanitizer_cov_trace_pc_guard_init (uint32_t *start, uint32_t *stop) {
  if (start == stop || *start) return;  // Initialize only once.
  printf("INIT: %p %p\n", start, stop);
  for (uint32_t *x = start; x < stop; x++)
    *x = ++N;  // Guards should start from 1.

  bcov_fp = fopen("./temp.bcov", "wb");
  if (atexit(close_bcovfp) != 0) {
    fprintf(stderr,"fclose error\n");
    exit(1);
  }
}

extern void 
__sanitizer_cov_trace_pc_guard (uint32_t *guard) {
  if (!*guard) return;  // Duplicate the guard check.
  void *PC = __builtin_return_address(0);
  char PcDescr[1024];
  __sanitizer_symbolize_pc(PC, "%p %F %L", PcDescr, sizeof(PcDescr));
  printf("guard: %p %x PC %s\n", guard, *guard, PcDescr);

  if (bcov_fp != 0x0) {
     if (! minit) {
        fwrite(&N, 1, 4, bcov_fp);
	minit = 1;
     }
     unsigned int PcD_len = (unsigned int) strlen(PcDescr); 
     fwrite(guard, 1, 4, bcov_fp);
     fwrite(&PcD_len, 1, 4, bcov_fp);
     fwrite(PcDescr, 1, PcD_len, bcov_fp);
  }
}
