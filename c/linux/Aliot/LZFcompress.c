#include "config.h"
#include <stdio.h>
#include <string.h>
#include "LZFcompress.h"
#include "lzf.h"

typedef unsigned char u8;

#define TYPE0_HDR_SIZE 5
#define TYPE1_HDR_SIZE 7
unsigned int LZFcompress_data (const char* input, u8 *outputBuffer)
{
  unsigned int us, cs, len;
  size_t length;
  u8 buf1[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
  u8 buf2[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
  u8 *header;
  size_t read = 0, byteToRead = 0, chunkSize = 3000;
  int i = 0;
  unsigned int outputPtr = 0;
  length = strlen(input);
 
  do
  {
    while(read < length)
    {

	if((length-read)>chunkSize)
	{
	    byteToRead=chunkSize;
	}
	else
	{
	    byteToRead=length-read;
	}
	
        memcpy(&buf1[MAX_HDR_SIZE],input+read,byteToRead);
	read += byteToRead;       	
      us = byteToRead; 
      cs = lzf_compress (&buf1[MAX_HDR_SIZE], us, &buf2[MAX_HDR_SIZE], us > 4 ? us - 4 : us);
      if (cs) 
      {
          header = &buf2[MAX_HDR_SIZE - TYPE1_HDR_SIZE];
          header[0] = 'Z';
          header[1] = 'V';
          header[2] = 1;
          header[3] = cs >> 8;
          header[4] = cs & 0xff;
          header[5] = us >> 8;
          header[6] = us & 0xff;
          len = cs + TYPE1_HDR_SIZE;
      }
      else
      {                       // write uncompressed
          header = &buf1[MAX_HDR_SIZE - TYPE0_HDR_SIZE];
          header[0] = 'Z';
          header[1] = 'V';
          header[2] = 0;
          header[3] = us >> 8;
          header[4] = us & 0xff;
          len = us + TYPE0_HDR_SIZE;
      }

     for(i = 0; i<len;i++,outputPtr++)
      {
	outputBuffer[outputPtr] = header[i];
      }

    }

  }while(read==BLOCKSIZE);
  outputBuffer[outputPtr] = '\0';
  return outputPtr;
}

