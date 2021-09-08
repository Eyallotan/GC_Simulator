#ifndef MAIN_H_
#define MAIN_H_

#include <unistd.h>

int PHYSICAL_BLOCK_NUMBER, LOGICAL_BLOCK_NUMBER, PAGES_PER_BLOCK, PAGE_SIZE;
unsigned long long NUMBER_OF_PAGES;
/* PHYSICAL_BLOCK_NUMBER - number of physical blocks
 * LOGICAL_BLOCK_NUMBER - number of logical blocks
 * PAGES_PER_BLOCK - number of pages per block
 * PAGE_SIZE - number of bytes per page
 */

int fd_stdout = dup(1);
char* output_file = nullptr;

void printHelp();

//TODO: BUGGY! fix
/* (lower bound, upper_bound, block score denominator's power, best number of generations, Y max range */
#define ALGO_PARAMS_TABLE                           \
X(0, (float)11/105, 7, 2, 0)                        \
X((float)11/105, (float)17/91, 7, 4, 0)             \
X((float)17/91, (float)11/39, 7, 5, 0)              \
X((float)11/39, (float)13/33, 5, 2, 0)              \
X((float)13/33, (float)29/55, 6, 4, 0)              \
X((float)29/55, (float)131/198, 3, 7, 0)            \
X((float)131/198, (float)8/9, 5, 6, 0)              \
X((float)8/9, (float)8/7, 2, 6, 0)                  \
X((float)8/7, (float)79/40, 3, 5, 0)                \
X((float)79/40, (float)99/40, 4, 4, 0)              \
X((float)99/40, (float)11/3, 2, 4, 0)               \
X((float)11/3, INT32_MAX, 5, 4, 0)
#undef X


#endif /* MAIN_H_ */
