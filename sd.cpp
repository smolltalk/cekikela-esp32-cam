#include "sd.h"

statusCode initSDCard() {
  uint8_t cardType = SD_MMC.cardType();

  // SD card has been already initialized?
  if (cardType != CARD_NONE) {
    return ok;
  }

  // Initialize SD card
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("SD Card Mount Failed.");
    return sdInitError;
  }

  cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached.");
    return sdInitError;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.print("MMC");
  } else if (cardType == CARD_SD) {
    Serial.print("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.print("SDHC");
  } else {
    Serial.print("UNKNOWN");
  }
  Serial.println(".");

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB.\n", cardSize);

  Serial.println("SD Card mounted.");
  return ok;
}

void endSDCard() {
  Serial.println("SD Card unmounted.");
  SD_MMC.end();
}

statusCode readFilesCounters(FilesCounters *filesCounters) {
  statusCode result = ok;

  Serial.println("readFilesCounters...");

  // Init SD Card
  result = initSDCard();
  if (result != ok) {
    return result;
  }
  fs::FS &fs = SD_MMC;
  Serial.printf("File %s %s.\n", FILES_COUNTERS_FILE_NAME, fs.exists(FILES_COUNTERS_FILE_NAME) ? "exists" : "does not exist");

  SDConfig sdConfig;

  //Initialize SDConfig object
  if (sdConfig.begin(fs, FILES_COUNTERS_FILE_NAME, 100)) {
    bool pictureCounterRead = false;
    bool uploadedPictureCounterRead = false;

    while (sdConfig.readNextSetting()) {
      if (sdConfig.nameIs("pictureCounter")) {
        filesCounters->pictureCounter = sdConfig.getIntValue();
        pictureCounterRead = true;
      } else if (sdConfig.nameIs("uploadedPictureCounter")) {
        filesCounters->uploadedPictureCounter = sdConfig.getIntValue();
        uploadedPictureCounterRead = true;
      } else {
        result = readConfigError;
        Serial.printf("Unknown counter %s.\n", sdConfig.getName());
      }
    }
    sdConfig.end();
    if (result == ok) {
      if (!pictureCounterRead) {
        result = readConfigError;
        Serial.printf("pictureCounter is missing.\n");
      } else if (!uploadedPictureCounterRead) {
        result = readConfigError;
        Serial.printf("uploadedPictureCounter is missing.\n");
      }
    }
    return result;
  }

  // Else
  Serial.printf("Failed to open file %s in reading mode.\n", FILES_COUNTERS_FILE_NAME);
  return sdReadError;
}

statusCode writeFilesCounters(FilesCounters *filesCounters) {
  Serial.println("writeFilesCounters...");

  // Init SD Card
  statusCode result = initSDCard();
  if (result != ok) {
    return result;
  }

  fs::FS &fs = SD_MMC;
  File file = fs.open(FILES_COUNTERS_FILE_NAME, FILE_WRITE);
  if (file) {
    char buffer[64];
    Serial.printf("File %s open in writing mode.\n", FILES_COUNTERS_FILE_NAME);
    result = (file.write((uint8_t *)buffer, sprintf(buffer, "pictureCounter=%d\nuploadedPictureCounter=%d\n", filesCounters->pictureCounter, filesCounters->uploadedPictureCounter)) > 0) ? ok : sdWriteError;
    file.close();
    Serial.printf("Written metadata:\n%s\n", buffer);
    return result;
  }
  // Else
  Serial.printf("Failed to open file %s in writing mode.\n", FILES_COUNTERS_FILE_NAME);
  return sdWriteError;
}

statusCode createFilesCounters(FilesCounters *filesCounters) {
  Serial.println("createFilesCounters...");
  filesCounters->pictureCounter = 0;
  filesCounters->uploadedPictureCounter = 0;
  return writeFilesCounters(filesCounters);
}

statusCode readOrCreateFilesCounters(FilesCounters *filesCounters) {
  Serial.println("readOrCreateFilesCounters...");
  return (readFilesCounters(filesCounters) == ok) ? ok : createFilesCounters(filesCounters);
}

statusCode savePictureOnSDCard(char *pictureName, uint8_t *buf, size_t len) {
  statusCode result = ok;

  // Path where new picture will be saved in SD Card
  // Format file name like "picture-xxxxx.jpg"
  // where x is the current picture number with leading 0.
  char path[20];
  sprintf(path, "/%s", pictureName);

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
    result = sdWriteError;
  } else {
    file.write(buf, len);  // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path);
  }
  file.close();
  return result;
}
