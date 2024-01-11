#include "sd.h"

/**
 * @brief Initialize (mount) the SD card.
 *
 * Note that the code uses the SD_MMC library and not the SD one.
 *
 * @return IS_OK when it succeeds or SD_INIT_ERROR in case of failure
 */
status_code_t initSdCard() {
  uint8_t cardType = SD_MMC.cardType();

  // SD card has been already initialized?
  if (cardType != CARD_NONE) {
    return IS_OK;
  }

  // Initialize SD card
  if (!SD_MMC.begin("/sdcard", true)) {
    logError(SD_LOG, "SD card Mount Failed.");
    return SD_INIT_ERROR;
  }

  cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    logError(SD_LOG, "No SD card attached.");
    return SD_INIT_ERROR;
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

  return IS_OK;
}

/**
 * Unmount the SD card.
 */
void endSdCard() {
  logInfo(SD_LOG, "SD card unmounted.");
  SD_MMC.end();
}

/**
 * @brief Save the picture data contained in pictureBuffer to a file named
 *        according to pictureFileName.
 *
 * The SD card has to be mounted first by calling initSdCard().
 *
 * @param pictureFileName a C string containing the picture file name
 * @param pictureBuffer a byte buffer containing the picture data to store to the file
 * @param pictureLen the length of data contained the pictureBuffer
 *
 * @return IS_OK when the operation succeeds. SD_WRITE_ERROR in case of failure
 */
status_code_t savePictureOnSdCard(char *pictureFileName, uint8_t *pictureBuffer, size_t pictureLen) {
  logDebug(SD_LOG, "%s...", __func__);
  status_code_t result = IS_OK;

  // Path where new picture will be saved in SD Card
  // Format file name like "picture-xxxxx.jpg"
  // where x is the current picture number with leading 0.
  char path[20];
  sprintf(path, "/%s", pictureFileName);

  fs::FS &fs = SD_MMC;
  logInfo(SD_LOG, "%s: save picture file name: %s.", __func__, path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    result = SD_WRITE_ERROR;
    logError(SD_LOG, "%s: failed to open picture file %s in writing mode.", __func__, path);
  } else {
    file.write(pictureBuffer, pictureLen);  // payload (image), payload length
    logInfo(SD_LOG, "%s: saved file to path: %s.", __func__, path);
  }
  file.close();
  return result;
}

/**
 * @brief Read file counters stored on the SD card and populate
 *        the given structure.
 * 
 * @param fileCounters a pointer to a fileCounters_t to populate
 *
 * @return IS_OK when the operation succeeds. SD_READ_ERROR in case of failure
 *
 * @see SD_FILES_COUNTERS_FILE_NAME
 */
status_code_t loadFileCounters(fileCounters_t *fileCounters) {
  status_code_t result = IS_OK;

  // Init SD Card
  result = initSdCard();
  if (result != IS_OK) {
    return result;
  }
  fs::FS &fs = SD_MMC;
  logInfo(SD_LOG, "%s: file %s %s.", __func__, SD_FILES_COUNTERS_FILE_NAME, fs.exists(SD_FILES_COUNTERS_FILE_NAME) ? "exists" : "does not exist");

  FileConfig fileConfig;

  //Initialize FileConfig object
  if (fileConfig.begin(fs, SD_FILES_COUNTERS_FILE_NAME, SD_FILES_COUNTERS_VALUE_MAX_SIZE, SD_FILES_COUNTERS_VALUE_MAX_SIZE, true, true)) {
    bool pictureCounterRead = false;
    bool uploadedPictureCounterRead = false;

    while (fileConfig.readNextSetting()) {
      if (fileConfig.nameIs("pictureCounter")) {
        fileCounters->pictureCounter = fileConfig.getIntValue();
        pictureCounterRead = true;
      } else if (fileConfig.nameIs("uploadedPictureCounter")) {
        fileCounters->uploadedPictureCounter = fileConfig.getIntValue();
        uploadedPictureCounterRead = true;
      } else {
        result = READ_CONFIG_ERROR;
        logError(SD_LOG, "%s: unknown counter %s.", __func__, fileConfig.getName());
      }
    }
    fileConfig.end();
    if (result == IS_OK) {
      if (!pictureCounterRead) {
        result = READ_CONFIG_ERROR;
        logError(SD_LOG, "%s: pictureCounter is missing.", __func__);
      } else if (!uploadedPictureCounterRead) {
        result = READ_CONFIG_ERROR;
        logError(SD_LOG, "%s: uploadedPictureCounter is missing.", __func__);
      }
    }
    return result;
  }

  // Else
  logError(SD_LOG, "%s: failed to open file %s in reading mode.\n", __func__, SD_FILES_COUNTERS_FILE_NAME);
  return SD_READ_ERROR;
}

/**
 * @brief Write the given file counters in a file on the SD card,
 *        so they can be loaded at the next wakeup.
 *
 * @param fileCounters a pointer to a fileCounters_t to store
 *
 * @return IS_OK when the operation succeeds. SD_WRITE_ERROR in case of failure
 *
 * @see SD_FILES_COUNTERS_FILE_NAME
 */
status_code_t saveFileCounters(fileCounters_t *fileCounters) {
  logDebug(SD_LOG, "%s...", __func__);

  // Init SD Card
  status_code_t result = initSdCard();
  if (result != IS_OK) {
    return result;
  }

  fs::FS &fs = SD_MMC;
  File file = fs.open(SD_FILES_COUNTERS_FILE_NAME, FILE_WRITE);
  if (file) {
    char buffer[64];
    logInfo(SD_LOG, "%s: file %s open in writing mode.\n", __func__, SD_FILES_COUNTERS_FILE_NAME);
    result = (file.write((uint8_t *)buffer, sprintf(buffer, "pictureCounter=%d\nuploadedPictureCounter=%d\n", fileCounters->pictureCounter, fileCounters->uploadedPictureCounter)) > 0) ? IS_OK : SD_WRITE_ERROR;
    file.close();
    logInfo(SD_LOG, "%s: written metadata:\n%s.", __func__, buffer);
    return result;
  }
  // Else
  logError(SD_LOG, "%s: failed to open file %s in writing mode.", __func__, SD_FILES_COUNTERS_FILE_NAME);
  return SD_WRITE_ERROR;
}

/**
 * @brief Populate the given structure file counters to 0
 *        and save them on SD card using saveFileCounters().
 *
 * @param fileCounters a pointer to a fileCounters_t to store
 *
 * @return IS_OK when the operation succeeds. SD_WRITE_ERROR in case of failure
 *
 * @see SD_FILES_COUNTERS_FILE_NAME
 */
status_code_t createFileCounters(fileCounters_t *fileCounters) {
  logDebug(SD_LOG, "%s...", __func__);
  fileCounters->pictureCounter = 0;
  fileCounters->uploadedPictureCounter = 0;
  return saveFileCounters(fileCounters);
}

/**
 * @brief Like loadFileCounters(), excepted that it calls createFileCounters
 *        in case of loading error.
 *
 * @return IS_OK when the operation succeeds. SD_READ_ERROR or SD_WRITE_ERROR in case of failure
 *
 * @see SD_FILES_COUNTERS_FILE_NAME
 * @see loadFileCounters()
 * @see createFileCounters()
 * @see saveFileCounters()
 */
status_code_t loadOrCreateFileCounters(fileCounters_t *fileCounters) {
  logDebug(SD_LOG, "%s...", __func__);
  return (loadFileCounters(fileCounters) == IS_OK) ? IS_OK : createFileCounters(fileCounters);
}

