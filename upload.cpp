#include "upload.h"

/**
 * Abstract class in charge to upload one picture to the server
 * according to the given upload settings and the destination file name.
 *
 * @see upload_settings_t
 * @see BufferUploader
 * @see FileUploader
 */
Uploader::Uploader(upload_settings_t *uploadSettings, uint32_t dataLen, String destFileName)
  : uploadSettings(uploadSettings), dataLen(dataLen), destFileName(destFileName){};

/**
  * Launch the data upload.
  *
  * @return IS_OK when it succeeds
  *         or WIFI_INIT_ERROR when the WiFi can not be initialized
  *         or UPLOAD_PICTURE_ERROR if the operation failed.
  */
status_code_t Uploader::upload() {
  status_code_t result = IS_OK;

  logInfo(UPLOAD_LOG, "Connecting to server: %s.", uploadSettings->serverAddress);

  if (client.connect(uploadSettings->serverAddress, uploadSettings->serverPort)) {
    logInfo(UPLOAD_LOG, "Uploading file %s...", destFileName);

    String head = "--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"auth\"\r\n\r\n" + String(uploadSettings->auth) + "\r\n--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"fileToUpload\"; filename=\"" + destFileName + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--EspCamWebUpload--\r\n";

    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = dataLen + extraLen;

    client.print("POST ");
    client.print(uploadSettings->path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(uploadSettings->serverAddress);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=EspCamWebUpload");
    client.println();
    client.print(head);

    sendData();

    client.print(tail);

    // Read response status code
    long startTime = millis();
    long timeoutTime = startTime + 10000;
    boolean statusCodeReadingStarted = false;
    uint8_t statusCodeReadingPosition = 0;
    char status_code_tStr[4];

    while (millis() < timeoutTime) {
      Serial.print(".");
      delay(100);
      while (client.available()) {
        char c = client.read();
        if (!statusCodeReadingStarted) {
          statusCodeReadingStarted = (c == ' ');
        } else {
          if (statusCodeReadingPosition == 3 || c == ' ') {
            // status code read => break
            break;
          } else {
            // status code recording
            status_code_tStr[statusCodeReadingPosition++] = c;
          }
        }
      }
    }
    client.stop();
    // End char array with \0
    status_code_tStr[statusCodeReadingPosition] = 0;
    // Convert status code to int
    int status_code_t = atoi(status_code_tStr);
    logInfo(UPLOAD_LOG, "Response status code: %d.", status_code_t);
    result = status_code_t == 200 ? IS_OK : UPLOAD_PICTURE_ERROR;
  } else {
    logError(UPLOAD_LOG, "%s: connection to %s failed.", __func__, uploadSettings->serverAddress);
    result = UPLOAD_PICTURE_ERROR;
  }
  return result;
};

/**
  * Called by upload() to send payload data
  * with the WiFi client.
  * Must be implemented by subclasses, because
  * it depends on the source type: buffer or file.
  *
  * @see BufferUploader::sendData()
  * @see FileUploader::sendData()
  */
void Uploader::sendData() {}

/**
 * Uploads a picture contained in a buffer.
 * Useful when the picture can not be stored in the SD card.
 * It reads the data directly from the sensor buffer.
 */
BufferUploader::BufferUploader(upload_settings_t *uploadSettings, uint8_t *srcBuffer, size_t len, String destFileName)
  : Uploader(uploadSettings, len, destFileName), srcBuffer(srcBuffer){};

/**
 * Read picture data from the frame buffer and
 * send it to the server by calling client.write().
 * Data is sent in 1024-byte packets.
 * Sending stops when the length of data sent reaches dataLen.
 */
void BufferUploader::sendData() {
  uint8_t *iBuf = srcBuffer;
  size_t fbLen = dataLen;
  for (size_t n = 0; n < fbLen; n = n + UPLOAD_BUFFER_SIZE) {
    if (n + UPLOAD_BUFFER_SIZE < fbLen) {
      client.write(iBuf, UPLOAD_BUFFER_SIZE);
      iBuf += UPLOAD_BUFFER_SIZE;
    } else if (fbLen % UPLOAD_BUFFER_SIZE > 0) {
      size_t remainder = fbLen % UPLOAD_BUFFER_SIZE;
      client.write(iBuf, remainder);
    }
  }
}

/**
 * Upload a picture contained in a file.
 * Useful when the picture is stored in the SD card.
 * It reads the data from the given file.
 * The file name on the server will be the same as the source file.
 */
FileUploader::FileUploader(upload_settings_t *uploadSettings, File srcFile)
  : Uploader(uploadSettings, srcFile.size(), srcFile.name()), srcFile{ srcFile } {};

/**
 * Read picture data from the opened file and
 * send it to the server by calling client.write().
 * Data is sent in 1024-byte packets.
 * Sending stops when the length of data sent reaches the file size.
 */
void FileUploader::sendData() {
  uint8_t srcBuffer[UPLOAD_BUFFER_SIZE];
  size_t len = srcFile.available();
  while (len) {
    client.write(srcBuffer, srcFile.read(srcBuffer, len < UPLOAD_BUFFER_SIZE ? len : (uint16_t)UPLOAD_BUFFER_SIZE));
    len = srcFile.available();
  }
}

/**
 * @brief Upload a SD stored picture file identified by its index.
 *
 * It aimed to be used by uploadPictureFiles().
 * In other use cases, initialize first the WiFi connection.
 * It uses FileUploader.
 *
 * @param uploadSettings required to determine the upload destination
 * @param fileIndex
 *
 * @return IS_OK when it succeeds or UPLOAD_PICTURE_ERROR if the operation failed
 *
 * @see uploadPictureFiles()
 * @see FileUploader
 */
status_code_t uploadPictureFileByIndex(upload_settings_t *uploadSettings, uint16_t i) {
  status_code_t result = IS_OK;
  // Path where new picture will be saved in SD Card
  char pictureFilePath[20] = "/";
  computePictureNameFromIndex(pictureFilePath + 1, i);

  fs::FS &fs = SD_MMC;
  logInfo(UPLOAD_LOG, "%s: picture file name: %s.", __func__, pictureFilePath);

  File file = fs.open(pictureFilePath, FILE_READ);
  if (!file) {
    logError(UPLOAD_LOG, "%s: failed to open file %s in reading mode.", __func__, pictureFilePath);
    result = UPLOAD_PICTURE_ERROR;
  } else {
    FileUploader fdu(uploadSettings, file);
    result = fdu.upload();
  }
  file.close();
  return result;
}

/**
 * @brief Determine if there is a new bunch of files to upload.
 *        In other words, fileCounters->pictureCounter - fileCounters->uploadedPictureCounter >= bunchSize?
 *
 * @param bunchSize
 * @param fileCounters
 *
 * @return true when there is a new bunch of files to upload
 *
 * @see uploadPictureFiles()
 */
bool canUploadPictures(uint8_t bunchSize, fileCounters_t *fileCounters) {
  return (fileCounters->pictureCounter - fileCounters->uploadedPictureCounter >= bunchSize);
}

/**
 * @brief Upload SD stored picture files that have not yet been uploaded.
 *
 * fileCounters->uploadedPictureCounter is incremented for each succeeded upload.
 * If an error occurs during one file uploading, then the function stops there.
 * Upload will be resumed/retried at the next taken picture.
 *
 * @param wifiSettings   required to establish the WiFi connection
 * @param uploadSettings required to determine the upload destination
 * @param fileCounters   used to determine which file to upload
 *
 * @return IS_OK when it succeeds
 *         or WIFI_INIT_ERROR when the WiFi can not be initialized
 *         or UPLOAD_PICTURE_ERROR if the operation failed
 *
 * @see uploadPictureFileByIndex()
 * @see canUploadPictures()
 */
status_code_t uploadPictureFiles(wifi_settings_t *wifi, upload_settings_t *uploadSettings, fileCounters_t *fileCounters) {
  status_code_t result = IS_OK;
  if (canUploadPictures(uploadSettings->bunchSize, fileCounters)) {
    result = initWifi(wifi);
    if (result == IS_OK) {
      for (uint16_t i = fileCounters->uploadedPictureCounter + 1; i <= fileCounters->pictureCounter; i++) {
        result = uploadPictureFileByIndex(uploadSettings, i);
        if (result == IS_OK) {
          fileCounters->uploadedPictureCounter++;
        } else {
          break;
        }
      }
      saveFileCounters(fileCounters);
    }
  }
  return result;
}

/**
 * @brief Upload a picture contained in a (frame) buffer.
 *
 * It is based on the BufferedUploader.

 * @param wifiSettings   required to establish the WiFi connection
 * @param uploadSettings required to determine the upload destination
 * @param destFileName   the uploaded destination file name that the server will see
 * @param srcBuffer      buffer containing the picture data
 * @param dataLen        the number of bytes of the payload to upload (i.e the picture size in byte)
 *
 * @return IS_OK when it succeeds
 *         or WIFI_INIT_ERROR when the WiFi can not be initialized
 *         or UPLOAD_PICTURE_ERROR if the operation failed
 *
 * @see BufferedUploader
 */
status_code_t uploadPicture(wifi_settings_t *wifi, upload_settings_t *uploadSettings, char *pictureName, uint8_t *srcBuffer, size_t len) {
  status_code_t result = IS_OK;
  result = initWifi(wifi);
  if (result == IS_OK) {
    BufferUploader bdu(uploadSettings, srcBuffer, len, String(pictureName));
    result = bdu.upload();
  }
  return result;
}
