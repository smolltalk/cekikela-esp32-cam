#include "sd.h"

statusCode initSDCard() {
  uint8_t cardType = SD_MMC.cardType();

  // SD card has been already initialized?
  if (cardType != CARD_NONE) {
    return ok;
  }

  // Initialize SD card
  if (!SD_MMC.begin("/sdcard", true)) {
    logError(SD_LOG, "SD card Mount Failed.");
    return sdInitError;
  }

  cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    logError(SD_LOG, "No SD card attached.");
    return sdInitError;
  }

  logInfo(SD_LOG, "SD Card Type:");
  if (cardType == CARD_MMC) {
    logInfo(SD_LOG, "MMC");
  } else if (cardType == CARD_SD) {
    logInfo(SD_LOG, "SDSC");
  } else if (cardType == CARD_SDHC) {
    logInfo(SD_LOG, "SDHC");
  } else {
    logInfo(SD_LOG, "UNKNOWN");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  logInfo(SD_LOG, "SD card Size: %lluMB.\n", cardSize);
  logInfo(SD_LOG, "SD card mounted.");

  return ok;
}

void endSDCard() {
  logInfo(SD_LOG, "SD card unmounted.");
  SD_MMC.end();
}

statusCode readFilesCounters(FilesCounters *filesCounters) {
  statusCode result = ok;

  // Init SD Card
  result = initSDCard();
  if (result != ok) {
    return result;
  }
  fs::FS &fs = SD_MMC;
  logInfo(SD_LOG, "%s: file %s %s.", __func__, FILES_COUNTERS_FILE_NAME, fs.exists(FILES_COUNTERS_FILE_NAME) ? "exists" : "does not exist");

  FileConfig fileConfig;

  //Initialize FileConfig object
  if (fileConfig.begin(fs, FILES_COUNTERS_FILE_NAME, FILES_COUNTERS_VALUE_MAX_SIZE, FILES_COUNTERS_VALUE_MAX_SIZE, true, true)) {
    bool pictureCounterRead = false;
    bool uploadedPictureCounterRead = false;

    while (fileConfig.readNextSetting()) {
      if (fileConfig.nameIs("pictureCounter")) {
        filesCounters->pictureCounter = fileConfig.getIntValue();
        pictureCounterRead = true;
      } else if (fileConfig.nameIs("uploadedPictureCounter")) {
        filesCounters->uploadedPictureCounter = fileConfig.getIntValue();
        uploadedPictureCounterRead = true;
      } else {
        result = readConfigError;
        logError(SD_LOG, "%s: unknown counter %s.", __func__, fileConfig.getName());
      }
    }
    fileConfig.end();
    if (result == ok) {
      if (!pictureCounterRead) {
        result = readConfigError;
        logError(SD_LOG, "%s: pictureCounter is missing.", __func__);
      } else if (!uploadedPictureCounterRead) {
        result = readConfigError;
        logError(SD_LOG, "%s: uploadedPictureCounter is missing.", __func__);
      }
    }
    return result;
  }

  // Else
  logError(SD_LOG, "%s: failed to open file %s in reading mode.\n", __func__, FILES_COUNTERS_FILE_NAME);
  return sdReadError;
}

statusCode writeFilesCounters(FilesCounters *filesCounters) {
  logDebug(SD_LOG, "%s...", __func__);

  // Init SD Card
  statusCode result = initSDCard();
  if (result != ok) {
    return result;
  }

  fs::FS &fs = SD_MMC;
  File file = fs.open(FILES_COUNTERS_FILE_NAME, FILE_WRITE);
  if (file) {
    char buffer[64];
    logInfo(SD_LOG, "%s: file %s open in writing mode.\n", __func__, FILES_COUNTERS_FILE_NAME);
    result = (file.write((uint8_t *)buffer, sprintf(buffer, "pictureCounter=%d\nuploadedPictureCounter=%d\n", filesCounters->pictureCounter, filesCounters->uploadedPictureCounter)) > 0) ? ok : sdWriteError;
    file.close();
    logInfo(SD_LOG, "%s: written metadata:\n%s.", __func__, buffer);
    return result;
  }
  // Else
  logError(SD_LOG, "%s: failed to open file %s in writing mode.", __func__, FILES_COUNTERS_FILE_NAME);
  return sdWriteError;
}

statusCode createFilesCounters(FilesCounters *filesCounters) {
  logDebug(SD_LOG, "%s...", __func__);
  filesCounters->pictureCounter = 0;
  filesCounters->uploadedPictureCounter = 0;
  return writeFilesCounters(filesCounters);
}

statusCode readOrCreateFilesCounters(FilesCounters *filesCounters) {
  logDebug(SD_LOG, "%s...", __func__);
  return (readFilesCounters(filesCounters) == ok) ? ok : createFilesCounters(filesCounters);
}

statusCode savePictureOnSDCard(char *pictureName, uint8_t *buf, size_t len) {
  logDebug(SD_LOG, "%s...", __func__);
  statusCode result = ok;

  // Path where new picture will be saved in SD Card
  // Format file name like "picture-xxxxx.jpg"
  // where x is the current picture number with leading 0.
  char path[20];
  sprintf(path, "/%s", pictureName);

  fs::FS &fs = SD_MMC;
  logInfo(SD_LOG, "%s: save picture file name: %s.", __func__, path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    result = sdWriteError;
    logError(SD_LOG, "%s: failed to open picture file %s in writing mode.", __func__, path);
  } else {
    file.write(buf, len);  // payload (image), payload length
    logInfo(SD_LOG, "%s: saved file to path: %s.", __func__, path);
  }
  file.close();
  return result;
}
