#ifndef FILENAME_H
#define FILENAME_H

#include "Arduino.h"

/**
 * Generate a random char between a-z or 0-9.
 *
 * @return a char
 */
char getRandomChar();

/**
 * @brief Fill a char array with a randomized C string.
 * 
 * @param destToFill the destination char array to fill with random chars
 * @param len        the len of the random C string to generate.
 *                   Make sure that the size of sToFill is at least len + 1
 *                   (for the end string char '\0')
 *
 * @see getRandomChar()
 */
void fillWithRandom(char *destToFill, uint8_t len);

/**
 * @brief Compute a picture name in "pic-%05d.jpg" format
 *        with the given index.
 *
 * @param destPictureName the char array receiving the computed picture name
 * @param index           the index to include in the picture name
 */
void computePictureNameFromIndex(char * destPictureName, uint16_t index);

/**
 * @brief Computes a picture name in "pic-R.jpg" format
 *        where R is randomized part of a given size.
 *
 * @param destPictureName the char array receiving the computed picture name
 * @param size            the size of the randomized part
 *
 * @see fillWithRandom()
 */
void computePictureNameFromRandom(char * destPictureName, uint8_t size);

#endif