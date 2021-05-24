
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "probConst.h"
#include "CONTROLINFO.h"

/* General definitions */

# define  WORKTODO       1
# define  NOMOREWORK     0

/** \brief results of processed text */
CONTROLINFO *results;

/** \brief max word length for each file */
int *maxWordLEN;

/* Allusion to internal functions */
static void savePartialResults(CONTROLINFO*);
static int isValidStopCharacter(char);
static void printResults(unsigned int numFiles, char *filesToProcess[]);
static void processText(unsigned char*, CONTROLINFO*);
void printWordLength(int largestWord);
void printRelativeCountConsoant(int largestWord, int wordCount, int arrayLength[MAX_SIZE_WORD], int arrayConsoantCount[MAX_SIZE_WORD][MAX_SIZE_WORD]);
void printRelativeLength(int largestWord, int wordCount, int arrayLenght[MAX_SIZE_WORD]);
void printWordBySize(int largestWord, int arrayLenght[MAX_SIZE_WORD]);
int isNumeric(char character);
int isAlphaNumeric(char character);
int isVowel(char character);
int isPonctuationSymbol(unsigned char ch);
int isSeparationSymbol(unsigned char ch);
int isUnderscore(unsigned char ch);
int isApostrophe(unsigned char ch);
int isSpace(unsigned char ch);
char removeAccented(unsigned char ch);
int maxWord(int size1, int size2);

/**
 *  \brief Main function.
 *
 *  Instantiation of the processing configuration.
 *
 *  \param argc number of words of the command line
 *  \param argv list of words of the command line
 *
 *  \return status of operation
 */
int main (int argc, char *argv[]){
  int rank,                                /* number of processes in the group */
  totProc;                                 /* group size */
  unsigned int numFiles = argc - 1;       /* number of files to process*/
  double start, finish;                    /* variables to calculate how much time the execution took */

  /* get processing configuration */

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &totProc);

  MPI_Barrier (MPI_COMM_WORLD);
  start = MPI_Wtime();

  /* processing */

  if (rank == 0){                          /* dispatcher process it is the first process of the group */

    FILE *f = NULL;                        /* pointer to the text stream associated with the file name */
    unsigned int whatToDo;                 /* command */
    unsigned int workProc, x;              /* counting variables */
    size_t i, aux;                          /* auxiliary variables*/
    CONTROLINFO ci = {0};                  /* data transfer variable */
    unsigned char dataToBeProcessed[K+1] = {0}; /* text to process */
    size_t filePos = 1;                         /* current file being processed*/
    if((results = (CONTROLINFO*) calloc(numFiles, sizeof(CONTROLINFO))) == NULL){
    	perror ("Error on allocating memory");
    	whatToDo = NOMOREWORK;
      	for (x = 1; x < totProc; x++)
        	MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
      	MPI_Finalize ();
      	return EXIT_FAILURE;
    }
    if((maxWordLEN = (int *) calloc(numFiles, sizeof(int))) == NULL){
    	perror ("Error on allocating memory");
    	free(results);
    	whatToDo = NOMOREWORK;
      	for (x = 1; x < totProc; x++)
        	MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
      	MPI_Finalize ();
      	return EXIT_FAILURE;
    }

    /* check running parameters and load list of names into memory */

    if (argc < 2){ 
      perror("Please insert text files to be processed as arguments!");
      free(results);
      free(maxWordLEN);
      whatToDo = NOMOREWORK;
      for (x = 1; x < totProc; x++)
        MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
      MPI_Finalize ();
      return EXIT_FAILURE;
    }
    
    /* loop until all files have been processed*/
    while(filePos <= numFiles) {
      
      workProc = 1;
      
      /* send text to process to all workers */
      for (x = 1; x < totProc; x++, workProc++){
        if(filePos > numFiles){
          break;
        }

        /* open file if necessary */
        if(f == NULL) {
          if((f = fopen (argv[filePos], "rb")) == NULL){
            perror ("Error on file opening for reading");
            free(results);
		    free(maxWordLEN);
            whatToDo = NOMOREWORK;
            for (x = 1; x < totProc; x++)
              MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
            MPI_Finalize ();
            exit (EXIT_FAILURE);
          }
          ci.filePosition = filePos - 1;
          ci.numBytes = 0;
          ci.wordCount = 0;
          ci.largestWord = 0;
          ci.nCharacters = 0;
          ci.nConsoants = 0;
        }

        i = fread(dataToBeProcessed, 1, K, f);
        if(i < K) {
          /* close file */
          if (fclose (f) == EOF){
            perror ("Error on closing file");
            free(results);
	        free(maxWordLEN);
            whatToDo = NOMOREWORK;
            for (x = 1; x < totProc; x++)
              MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
            MPI_Finalize ();
            exit (EXIT_FAILURE);
          }

          filePos++;
          f = NULL;

        } else {
          aux = i;
          while(isValidStopCharacter(dataToBeProcessed[i-1]) == 0 && i > 0){
            i--;
          }
          if(i == 0)
            i = aux;
          if(fseek(f, i-K, 1) != 0){
          	perror ("Error on fseek!");
          	free(results);
	  	     free(maxWordLEN);
            whatToDo = NOMOREWORK;
            for (x = 1; x < totProc; x++)
              MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
            MPI_Finalize ();
            exit (EXIT_FAILURE);
          }
        }
        ci.numBytes = i;
     
      	/* distribute sorting task */
        whatToDo = WORKTODO;
        MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
        MPI_Send (&ci, sizeof (CONTROLINFO), MPI_BYTE, x, 0, MPI_COMM_WORLD);
        MPI_Send (&dataToBeProcessed, K+1, MPI_UNSIGNED_CHAR, x, 0, MPI_COMM_WORLD);
        memset(dataToBeProcessed, 0, K+1);
      }
      
      /* receive results of processing from workers*/
      for (x = 1; x < workProc; x++) {
        MPI_Recv (&ci, sizeof(CONTROLINFO), MPI_BYTE, x, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        savePartialResults(&ci);
      }

    }
    
    /* dismiss worker processes */
    whatToDo = NOMOREWORK;
    for (x = 1; x < totProc; x++)
      MPI_Send (&whatToDo, 1, MPI_UNSIGNED, x, 0, MPI_COMM_WORLD);
    

  } else { /* worker processes the remainder processes of the group */

    unsigned int whatToDo;                /* command */
    CONTROLINFO ci;                       /* data transfer variable */
    unsigned char dataToBeProcessed[K+1] = {0}; /* text to process */

    while (true){
      MPI_Recv (&whatToDo, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if (whatToDo == NOMOREWORK)
        break;
      MPI_Recv (&ci, sizeof (CONTROLINFO), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv (&dataToBeProcessed, K+1, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      processText(dataToBeProcessed, &ci);
      MPI_Send (&ci, sizeof (CONTROLINFO), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }
  }

  /* print results and execution time */
  MPI_Barrier (MPI_COMM_WORLD);
  if(rank == 0) {
    printResults(numFiles, argv+1);
    finish = MPI_Wtime();
    printf("Execution time: %f seconds\n", finish - start);
  }
  MPI_Finalize ();
  return EXIT_SUCCESS;
}



/**
 *  \brief Save partial results received from worker
 *
 *  Operation carried out by dispatcher process.
 *
 *  \param *ci    pointer to the shared data structure
 *
 */
void savePartialResults(CONTROLINFO *ci){                                                                          

  size_t filePosition = ci->filePosition;
  results[filePosition].numBytes += ci->numBytes;
  results[filePosition].wordCount += ci->wordCount;
  results[filePosition].nCharacters += ci->nCharacters;
  results[filePosition].nConsoants += ci->nConsoants;
  ci->wordCount = 0;
  ci->nCharacters = 0;
  ci->nConsoants = 0;
  if (ci->largestWord > results[filePosition].largestWord) {
    results[filePosition].largestWord = ci->largestWord;
    maxWordLEN[filePosition] = ci->largestWord;
  }

  for (size_t i = 0; i < ci->largestWord+1; i++){
    results[filePosition].nWordsBySize[i] += ci->nWordsBySize[i];
    ci->nWordsBySize[i] = 0;
  }

  for (size_t i = 0; i < ci->largestWord+1; i++){
    for (size_t j = 0; j < ci->largestWord+1; j++){
      results[filePosition].nConsoantsByWordLength[i][j] += ci->nConsoantsByWordLength[i][j];
      ci->nConsoantsByWordLength[i][j] = 0;
    }
  }
}

/**
 *  \brief Validate if a character is a stop character.
 *
 *  Operation carried out by dispatcher process.
 *
 * \param character   character to which the validation is done.
 *
 * \return    0 if the character provided is not a stop character, otherwise is the number of bytes the character should have. Can be used as true/false.
 *
 */
static int isValidStopCharacter(char character) {
  char separation[15] = { (char)0x20, (char)0x9, (char)0xA, '-', '"', '(', ')', '[', ']', '.', ',', ':', ';', '?', '!' };
  char separation3[4] = { (char)0x9C, (char)0x9D, (char)0x93, (char)0xA6 };
  int x;
  for (x = 0; x < 15; x++)
    if (character == separation[x])
      return 1;
  for (x = 0; x < 4; x++)
    if (character == separation3[x])
      return 3;
  return 0;
}


/**
 *  \brief Print the results of each file.
 *
 *  Operation carried out by dispatcher process at the end of processing.
 *
 */
static void printResults(unsigned int numFiles, char *filesToProcess[]){

for (int i = 0; i < numFiles; i++) {
    printf("\nFile name: %s\n", filesToProcess[i]);
    printf("Total number of words = %d\nWord Length\n", results[i].wordCount);
    printWordLength(results[i].largestWord);
    printWordBySize(results[i].largestWord, results[i].nWordsBySize);
    printRelativeLength(results[i].largestWord, results[i].wordCount, results[i].nWordsBySize);
    printRelativeCountConsoant(results[i].largestWord, results[i].wordCount, results[i].nWordsBySize, results[i].nConsoantsByWordLength);
}
  free(results);
  free(maxWordLEN);
}

/**
 *  \brief Process text function.
 *
 *  Processes text sent by dispatcher and calculates the required statistics.
 *
 *  \param dataToBeProcessed chunk of text data being processed
 *  \param ci structure where calculated statistics are saved
 */
static void processText(unsigned char *buffer, CONTROLINFO * partialInfo) {
    char ch;
    int inWord = 0;
    int nConsoantsWord = 0, nCharacters = 0, length = partialInfo->numBytes;
    
    for (int i = 0; i < length; i++) {
        if ((unsigned char)buffer[i] == 0xc3) { //Second bit more significant
            i++;
            ch = removeAccented((unsigned char)buffer[i]);
        }
        else if ((unsigned char)buffer[i] == 0xc2) { //Second bit more significant
            i++;
            if ((unsigned char)buffer[i] == 0xB4) //Accent to apostrophe
            {
                ch = 0x27;
            }
            else if ((unsigned char)buffer[i] == 0xAB || (unsigned char)buffer[i] == 0xBB) { //Left and right angled double quotation marks to double quotation mark
                ch = 0x22;
            }
        }
        else if ((unsigned char)buffer[i] == 0xe2) //Third bit more significant
        {
            i = i + 2;
            if ((unsigned char)buffer[i] == 0x98 || (unsigned char)buffer[i] == 0x99) //Left and right single quotation marks to apostrophe
            {
                ch = 0x27;
            }
            else if ((unsigned char)buffer[i] == 0x9c || (unsigned char)buffer[i] == 0x9d) { //Left and right double quotation marks to double quotation mark
                ch = 0x22;
            }
            else if ((unsigned char)buffer[i] >= 0x90 || (unsigned char)buffer[i] <= 0x94) { //Dash to hyphen
                ch = 0x2d;
            }
            else if ((unsigned char)buffer[i] == 0xa6) { //Ellipsis to full point
                ch = 0x2e;
            }
        }
        else
        {
            if (buffer[i] == 0x60) //Accent to apostrophe
                ch = 0x27;
            else
                ch = buffer[i];
        }
        ch = tolower(ch);
        if ((isPonctuationSymbol(ch) || isSpace(ch) || isSeparationSymbol(ch)) && inWord) { //Check if is delimiter
            inWord = 0;
            partialInfo->largestWord = maxWord(partialInfo->largestWord, nCharacters);
            partialInfo->nCharacters += nCharacters;
            partialInfo->nConsoants += nConsoantsWord;
            partialInfo->nWordsBySize[nCharacters]++;
            partialInfo->nConsoantsByWordLength[nConsoantsWord][nCharacters]++;
            nCharacters = 0;
            nConsoantsWord = 0;
        }
        else if (((isAlphaNumeric(ch) || isUnderscore(ch))) && !inWord) //Check if is start of word
        {
            inWord = 1;
            partialInfo->wordCount++;
        }
        if (inWord && !isApostrophe(ch)) //Counters on word
        {
            if (!isVowel(ch) && !isUnderscore(ch) && !isNumeric(ch))
            {
                nConsoantsWord++;
            }
            nCharacters++;
        }

        if (buffer[i] == '\0') {
            break;
        }
    }
}

/**
 *  \brief Returns the biggest of two sizes.
 *
 *  Internal thread operation.
 *
 *  \param size1 first size
 *  \param size2 second size
 *
 *  \return biggest size between the two
 */
int maxWord(int size1, int size2) {
    if (size1 <= size2)
        return size2;
    else
        return size1;
}
/**
 *  \brief Removes accent from vowels. Transform from multibyte character to single byte.
 *
 *  Internal thread operation.
 *
 *  \param ch multibyte character
 *
 *  \return single byte character
 */
char removeAccented(unsigned char ch) {

    if ((ch >= 0xA0 && ch <= 0xA5) || (ch >= 0x80 && ch <= 0x85))
        return 'a';

    if ((ch >= 0xA8 && ch <= 0xAB) || (ch >= 0x88 && ch <= 0x8B))
        return 'e';

    if ((ch >= 0xAC && ch <= 0xAF) || (ch >= 0x8C && ch <= 0x8F))
        return 'i';

    if ((ch >= 0xB2 && ch <= 0xB6) || (ch >= 0x92 && ch <= 0x96))
        return 'o';

    if ((ch >= 0xB9 && ch <= 0xBB) || (ch >= 0x99 && ch <= 0x9B))
        return 'u';

    if (ch == 0xA7 || ch == 0x87)
        return 'c';

    if (ch == 0xBC || ch == 0x9C)
        return 0x27;

    if (ch == 0xBF || ch == 0xBD || ch == 0x9D)
        return 'y';

    return ch;
}
/**
 *  \brief Check if char is space,tab, newline or carriage return.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isSpace(unsigned char ch) {
    if (ch == 0x20 || ch == 0x9 || ch == 0xA)
    {
        return 1;

    }
    return 0;
}
/**
 *  \brief Check if char is an apostrophe.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isApostrophe(unsigned char ch) {
    if (ch == 0x27)
    {
        return 1;

    }
    return 0;
}
/**
 *  \brief Check if char is an underscore.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isUnderscore(unsigned char ch) {
    if (ch == 0x5f)
    {
        return 1;

    }
    return 0;
}
/**
 *  \brief Check if char is a separation symbol (hyphen, double quotation mark, bracket or parentheses).
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isSeparationSymbol(unsigned char ch) {
    if (ch == 0x22 || ch == 0x5b || ch == 0x5d || ch == 0x28 || ch == 0x29 || ch == 0x2d)
    {
        return 1;

    }
    return 0;
}
/**
 *  \brief Check if char is a ponctuation symbol (full point, comma, colon, semicolon, question mark or exclamation point).
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isPonctuationSymbol(unsigned char ch) {
    if (ch == 0x2e || ch == 0x2c || ch == 0x3a || ch == 0x3b || ch == 0x3f || ch == 0x21)
    {
        return 1;

    }
    return 0;
}

/**
 *  \brief Check if char is a vowel.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isVowel(char character) {
    if (character == 'a' || character == 'e' || character == 'i' || character == 'o' || character == 'u' || character == 'y')
    {
        return 1;

    }
    return 0;
}
/**
 *  \brief Check if char is an alphanumerical character.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isAlphaNumeric(char character) {
    if ((character >= 65 && character <= 90) || (character >= 97 && character <= 122) || (character >= 48 && character <= 57)) {
        return 1;
    }
    return 0;
}
/**
 *  \brief Check if char is a numeric character.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isNumeric(char character) {
    if (character >= 48 && character <= 57) {
        return 1;
    }
    return 0;
}

void printWordLength(int largestWord) {
    int i;
    for (i = 1; i <= largestWord; i++)
    {
        printf("\t");
        printf("%d", i);
    }
    printf("\n");
}

void printWordBySize(int largestWord, int arrayLenght[MAX_SIZE_WORD]) {
    for (int i = 1; i <= largestWord; i++)
    {
        printf("\t");
        printf("%d", arrayLenght[i]);
    }
    printf("\n");
}

void printRelativeLength(int largestWord, int wordCount, int arrayLenght[MAX_SIZE_WORD]) {
    for (int i = 1; i <= largestWord; i++)
    {
        printf("\t");
        printf("%0.2f", (((double)arrayLenght[i] / (double)wordCount) * 100));
    }
    printf("\n");
}

void printRelativeCountConsoant(int largestWord, int wordCount, int arrayLength[MAX_SIZE_WORD], int arrayConsoantCount[MAX_SIZE_WORD][MAX_SIZE_WORD]) {
    int value;
    for (int lign = 0; lign <= largestWord; lign++) {
        printf("%d", lign);
        for (int column = 1; column <= largestWord; column++)
        {
            printf("\t");
            if (lign <= column) {
                value = arrayConsoantCount[lign][column];
                if (arrayLength[column]) {
                    printf("%0.1f", ((double)value / (double)arrayLength[column]) * 100);
                }
                else {
                    printf("%0.1f", (double)0);
                }
            }
        }
        printf("\n");
    }
}

