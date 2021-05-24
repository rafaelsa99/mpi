/**
 *  \file prob1.h (header file)
 *
 *  \brief prob1 thread definition.
 * *
 *  Definition of the operations carried out by the prob1 thread:
 *     \li *prob1
 *     \li processDataChunk
 *	   \li Auxiliar functions for the processing
 *
 *  \author Rafael Sá (104552), Luís Laranjeira (81526)
 */

#ifndef prob1_H_
#define prob1_H_

int maxWord(int size1, int size2);
/**
 *  \brief Removes accent from vowels. Transform from multibyte character to single byte.
 *
 *  Internal thread operation.
 *
 *  \param ch multibyte character
 *
 *  \return single byte character
 */
char removeAccented(unsigned char ch);
/**
 *  \brief Check if char is space,tab, newline or carriage return.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isSpace(unsigned char ch);
/**
 *  \brief Check if char is an apostrophe.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isApostrophe(unsigned char ch);
/**
 *  \brief Check if char is an underscore.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isUnderscore(unsigned char ch);
/**
 *  \brief Check if char is a separation symbol (hyphen, double quotation mark, bracket or parentheses).
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isSeparationSymbol(unsigned char ch);
/**
 *  \brief Check if char is a ponctuation symbol (full point, comma, colon, semicolon, question mark or exclamation point).
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isPonctuationSymbol(unsigned char ch);
/**
 *  \brief Check if char is a vowel.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isVowel(char character);
/**
 *  \brief Check if char is an alphanumerical character.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isAlphaNumeric(char character);
/**
 *  \brief Check if char is a numeric character.
 *
 *  Internal thread operation.
 *
 *  \param ch character
 *
 *  \return 1 if it is. 0 otherwise.
 */
int isNumeric(char character);

#endif /* prob1_H_ */
