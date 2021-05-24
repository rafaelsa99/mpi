 
#ifndef CONTROLINFO_H
#define CONTROLINFO_H

#include <stdlib.h>
#include "probConst.h"

typedef struct
{
	int fileId;	//File identifier
	int filePosition;
	int nCharacters; //Number of characteres 
	int nConsoants; //Number of consoants
	int largestWord; //Number of characters of the largest word
	int numBytes;
	int wordCount; //Number of words
	int nWordsBySize[MAX_SIZE_WORD]; //Number of words for each word size
	int nConsoantsByWordLength[MAX_SIZE_WORD][MAX_SIZE_WORD]; //Number of consoants for each word size 
}CONTROLINFO;

#endif
