/**
 *  \file CONTROLINFO.h (interface file)
 */
 
#ifndef CONTROLINFO_H
#define CONTROLINFO_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
   bool processing;
   size_t filePosition;
   size_t numSamples;
   size_t rxyIndex;
   double result;
} CONTROLINFO;

#endif