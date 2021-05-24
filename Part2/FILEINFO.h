/**
 *  \file FILEINFO.h (interface file)
 */
 
#ifndef FILEINFO_H
#define FILEINFO_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
   bool read;
   size_t filePosition;
   size_t numSamples;
   size_t rxyIndex;
   double* result;
   double* expected;
} FILEINFO;

#endif