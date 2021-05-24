 
#ifndef CONTROLINFO_H
#define CONTROLINFO_H

#include <stdlib.h>
#include "probConst.h"

typedef struct
{
	size_t fileId;	//File identifier
	size_t filePosition;
	size_t nCharacters; //Number of characteres 
	size_t nConsoants; //Number of consoants
	size_t largestWord; //Number of characters of the largest word
	size_t numBytes;
	size_t wordCount; //Number of words
	size_t nWordsBySize[MAX_SIZE_WORD]; //Number of words for each word size
	int nConsoantsByWordLength[MAX_SIZE_WORD][MAX_SIZE_WORD]; //Number of consoants for each word size 
}CONTROLINFO;

#endif
