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

//TODO: updtae table values

/* (lower bound, upper_bound, block score denominator's power, Y max range */
#define ALGO_PARAMS_TABLE \
X(0, 0.1, 5, 1)          \
X(0.1, 0.2, 5, 1)         \
X(0.2, 0.3, 7, 0)         \
X(0.3, 0.4, 8, 0)         \
X(0.4, 0.5, 8, 1)         \
X(0.5, 0.6, 6, 1)         \
X(0.6, 0.7, 5, 1)         \
X(0.7, 0.8, 4, 2)         \
X(0.9, 1, 5, 2)         \
X(1, INT32_MAX, 5, 1)
#undef X


#endif /* MAIN_H_ */
