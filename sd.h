#ifndef SD_H
#define SD_H

#include "FS.h"
#include "SD_MMC.h"
#include "error.h"

#define FILES_COUNTERS_FILE_NAME "/counters.txt"

typedef struct {
  uint16_t pictureNumber;
  uint16_t uploadedPictureNumber;
} FilesCounters;

statusCode initSDCard();
void endSDCard();
statusCode savePictureOnSDCard(char * pictureName, uint8_t * buf, size_t len);
statusCode readFilesCounters(FilesCounters * filesCounters);  
statusCode writeFilesCounters(FilesCounters * filesCounters);
statusCode readOrCreateFilesCounters(FilesCounters * filesCounters);

#endif