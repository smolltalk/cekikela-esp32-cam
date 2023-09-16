#include "sd.h"

statusCode initSDCard() {
  //Serial.println("Starting SD Card");
  // Fix SD card initialization
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("SD Card Mount Failed.");
    return sdInitError;
  }
  uint8_t cardType = SD_MMC.cardType();

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

statusCode readFilesMetaData(FilesMetaData *filesMetaData) {
  Serial.println("readFilesMetaData...");
  fs::FS &fs = SD_MMC;
  Serial.printf("File %s %s.\n", FILES_META_DATA_FILE_NAME, fs.exists(FILES_META_DATA_FILE_NAME) ? "exists" : "does not exist");
  File file = fs.open(FILES_META_DATA_FILE_NAME, FILE_READ);
  if (file) {
    char buffer[16];
    int fileLen = file.available();
    Serial.printf("File %s open in reading mode. %d bytes to read.\n", FILES_META_DATA_FILE_NAME, fileLen);
    file.readBytes(buffer, fileLen);
    buffer[fileLen] = 0;
    uint16_t pictureNumber, uploadedPictureNumber;
    int readParamCount = sscanf(buffer, "%d,%d", &pictureNumber, &uploadedPictureNumber);
    filesMetaData->pictureNumber = pictureNumber;
    filesMetaData->uploadedPictureNumber = uploadedPictureNumber;
    file.close();
    Serial.printf("Read metadata: %s\n", buffer);
    if (readParamCount == 2) {
      return ok;
    }
    // Else
    Serial.printf("Read error: %d parameter read. Expected 2.\n", readParamCount);
    return sdReadError;
  }
  // Else
  Serial.printf("Failed to open file %s in reading mode.\n", FILES_META_DATA_FILE_NAME);
  return sdReadError;
}

statusCode writeFilesMetaData(FilesMetaData *filesMetaData) {
  Serial.println("writeFilesMetaData...");
  fs::FS &fs = SD_MMC;
  File file = fs.open(FILES_META_DATA_FILE_NAME, FILE_WRITE);
  if (file) {
    char buffer[16];
    Serial.printf("File %s open in writing mode.\n", FILES_META_DATA_FILE_NAME);
    statusCode result = (file.write((uint8_t *)buffer, sprintf(buffer, "%d,%d", filesMetaData->pictureNumber, filesMetaData->uploadedPictureNumber)) > 0) ? ok : sdWriteError;
    file.close();
    Serial.printf("Written metadata: %s\n", buffer);
    return result;
  }
  // Else
  Serial.printf("Failed to open file %s in writing mode.\n", FILES_META_DATA_FILE_NAME);
  return sdWriteError;
}

statusCode createFilesMetaData(FilesMetaData *filesMetaData) {
  Serial.println("createFilesMetaData...");
  filesMetaData->pictureNumber = 0;
  filesMetaData->uploadedPictureNumber = 0;
  return writeFilesMetaData(filesMetaData);
}

statusCode readOrCreateFilesMetaData(FilesMetaData *filesMetaData) {
  Serial.println("readOrCreateFilesMetaData...");
  return (readFilesMetaData(filesMetaData) == ok) ? ok : createFilesMetaData(filesMetaData);
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
