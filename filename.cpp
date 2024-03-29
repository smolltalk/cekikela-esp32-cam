#include "filename.h"

/**
 * @brief Generate a random char between a-z or 0-9.
 * 
 * It uses the function random().
 *
 * @return a char
 *
 * @see random()
 */
char getRandomChar() {
  uint8_t randomValue = random(0, 37);
  return randomValue + ((randomValue < 10) ? '0' : 'a' - 10);
}

/**
 * @brief Fill a char array with a randomized C string.
 *
 * Each char is generated by getRandomChar().
 * 
 * @param destToFill the destination char array to fill with random chars.
 * @param len        the len of the random C string to generate
 *                   Make sure that the size of sToFill is at least len + 1
 *                   (for the end string char '\0')
 *
 * @see getRandomChar()
 */
void fillWithRandom(char *destToFill, uint8_t len) {
  // End string = \0
  destToFill[len] = 0;
  while(len-- > 0) {
    destToFill[len] = getRandomChar();
  }
}

/**
 * @brief Compute a picture name in "pic-%05d.jpg" format
 *        with the given index.
 *
 * The generated picture name is stored as a C string
 * in destPictureName.
 * Ex: if index is 236, then destPictureName will be "pic-00236.jpg"
 *
 * @param destPictureName the char array receiving the computed picture name
 * @param index           the index to include in the picture name
 */
void computePictureNameFromIndex(char * destPictureName, uint16_t index){
  sprintf(destPictureName, "pic-%05d.jpg", index);
}

/**
 * @brief Compute a picture name in "pic-R.jpg" format
 *        where R is randomized part of a given size.
 *
 * The generated picture name is stored as a C string
 * in destPictureName.
 * Ex: if size is 5, then destPictureName could be "pic-ahr5z.jpg"
 *
 * @param destPictureName the char array receiving the computed picture name
 * @param size            the size of the randomized part
 *
 * @see fillWithRandom()
 */
void computePictureNameFromRandom(char * destPictureName, uint8_t size){
  char sRandom[size + 1];
  fillWithRandom(sRandom, size);
  sprintf(destPictureName, "pic-%s.jpg", sRandom);
}
