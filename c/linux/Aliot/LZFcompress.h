#ifndef LZFCOMPRESS_H
#define LZFCOMPRESS_H

/* AGENT IMPLEMENTERS SHOULD ADJUST THIS BLOCKSIZE FOR COMPRESSION AS PER THE CONTROLLER MEMORY AVAILABILITY */
#define BLOCKSIZE (1024 * 8 - 1)


/* Do not change the values here. It affects compression data structures */
#define MAX_BLOCKSIZE BLOCKSIZE
#define MAX_HDR_SIZE 7
#define MIN_HDR_SIZE 5


unsigned int LZFcompress_data (const char* input, unsigned char *outputBuffer);
#endif
