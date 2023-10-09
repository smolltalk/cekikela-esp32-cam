#ifndef FILENAME_H
#define FILENAME_H

#include "Arduino.h"

char getRandomChar();
void fillWithRandom(char *sToFill, uint8_t count);
void computePictureNameFromIndex(char * pictureName, uint16_t index);
void computePictureNameFromRandom(char * pictureName, uint8_t size);

#endif