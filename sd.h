#ifndef SD_H
#define SD_H

#include "error.h"
#include "FileConfig.h"
#include "logging.h"
#include "FS.h"
#include "SD_MMC.h"

#define SD_LOG "SD"

#define FILES_COUNTERS_FILE_NAME "/counters.txt"
#define FILES_COUNTERS_VALUE_MAX_SIZE 100

typedef struct {
  uint16_t pictureCounter;
  uint16_t uploadedPictureCounter;
} FilesCounters;

statusCode initSdCard();
void endSdCard();
statusCode savePictureOnSdCard(char * pictureName, uint8_t * buf, size_t len);
statusCode readFilesCounters(FilesCounters * filesCounters);  
statusCode writeFilesCounters(FilesCounters * filesCounters);
statusCode readOrCreateFilesCounters(FilesCounters * filesCounters);

#endif