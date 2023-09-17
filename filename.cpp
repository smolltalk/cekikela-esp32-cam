#include "filename.h"

char getRandomChar() {
  uint8_t randomValue = random(0, 37);
  return randomValue + ((randomValue < 10) ? '0' : 'a' - 10);
}

void fillWithRandom(char *sToFill, uint8_t count) {
  // End string = \0
  sToFill[count] = 0;
  while(count-- > 0) {
    sToFill[count] = getRandomChar();
  }
}

void computePictureNameFromIndex(char * pictureName, uint16_t index){
  sprintf(pictureName, "pic-%05d.jpg", index);
}

void computePictureNameFromRandom(char * pictureName){
  char sRandom[FILE_NAME_RANDOM_SIZE + 1];
  fillWithRandom(sRandom, FILE_NAME_RANDOM_SIZE);
  sprintf(pictureName, "pic-%s.jpg", sRandom);
}
