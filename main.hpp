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

#endif /* MAIN_H_ */
