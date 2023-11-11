#include "upload.h"

DataUploader::DataUploader(uploadSettings_t *uploadSettings, uint32_t dataLen, String fileName)
  : uploadSettings(uploadSettings), dataLen(dataLen), fileName(fileName){};

statusCode DataUploader::upload() {
  statusCode result = ok;

  Serial.printf("Connecting to server: %s\n", uploadSettings->serverName);

  if (client.connect(uploadSettings->serverName, uploadSettings->serverPort)) {
    Serial.println("Connection successful!");
    Serial.printf("Uploading file %s...\n", fileName);

    String head = "--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"auth\"\r\n\r\n" + String(uploadSettings->auth) + "\r\n--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"fileToUpload\"; filename=\"" + fileName + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--EspCamWebUpload--\r\n";

    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = dataLen + extraLen;
 
    client.print("POST ");
    client.print(uploadSettings->path);
    client.println(" HTTP/1.1");
    client.print("Host: "); 
    client.println(uploadSettings->serverName);
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
    char statusCodeStr[4];

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
            statusCodeStr[statusCodeReadingPosition++] = c;
          }
        }
      }
    }
    client.stop();
    // End char array with \0
    statusCodeStr[statusCodeReadingPosition] = 0;
    // Convert status code to int
    int statusCode = atoi(statusCodeStr);
    Serial.printf("\nResponse status code: %d\n", statusCode);
    result = statusCode == 200 ? ok : uploadPictureError;
  } else { 
    Serial.printf("Connection to %s failed.\n", uploadSettings->serverName);
    result = uploadPictureError;
  }
  return result;
};

void DataUploader::sendData() {}

BufferDataUploader::BufferDataUploader(uploadSettings_t *uploadSettings, uint8_t *buf, size_t len, String fileName)
  : DataUploader(uploadSettings, len, fileName), buf(buf){};

void BufferDataUploader::sendData() {
  uint8_t *iBuf = buf;
  size_t fbLen = dataLen;
  for (size_t n = 0; n < fbLen; n = n + 1024) {
    if (n + 1024 < fbLen) {
      client.write(iBuf, 1024);
      iBuf += 1024;
    } else if (fbLen % 1024 > 0) {
      size_t remainder = fbLen % 1024;
      client.write(iBuf, remainder);
    }
  }
}

FileDataUploader::FileDataUploader(uploadSettings_t *uploadSettings, File file)
  : DataUploader(uploadSettings, file.size(), file.name()), file{ file } {};

void FileDataUploader::sendData() {
  uint8_t buf[256];
  size_t len = file.available();
  while (len) {
    client.write(buf, file.read(buf, len < 256 ? len : (uint16_t)256));
    len = file.available();
  }
}

statusCode uploadPictureFile(uploadSettings_t *uploadSettings, uint16_t i) {
  statusCode result = ok;
  // Path where new picture will be saved in SD Card
  char pictureFilePath[20] = "/";
  computePictureNameFromIndex(pictureFilePath + 1, i);

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", pictureFilePath);

  File file = fs.open(pictureFilePath, FILE_READ);
  if (!file) {
    Serial.printf("Failed to open file %s in reading mode\n", pictureFilePath);
    result = uploadPictureError;
  } else {
    FileDataUploader fdu(uploadSettings, file);
    result = fdu.upload();
  }
  file.close();
  return result;
}

bool canUploadPictures(uint8_t bunchSize, FilesCounters *filesCounters) {
  return (filesCounters->pictureCounter - filesCounters->uploadedPictureCounter >= bunchSize);
}

statusCode uploadPictureFiles(wifiSettings_t * wifi, uploadSettings_t *uploadSettings, FilesCounters *filesCounters) {
  statusCode result = ok;
  if (canUploadPictures(uploadSettings->bunchSize, filesCounters)) {
    result = initWifi(wifi);
    if (result == ok) {
      for (uint16_t i = filesCounters->uploadedPictureCounter + 1; i <= filesCounters->pictureCounter; i++) {
        result = uploadPictureFile(uploadSettings, i);
        if (result == ok) {
          filesCounters->uploadedPictureCounter++;
        } else {
          break;
        }
      }
      writeFilesCounters(filesCounters);
    }
  }
  return result;
}

statusCode uploadPicture(wifiSettings_t * wifi, uploadSettings_t *uploadSettings, char *pictureName, uint8_t *buf, size_t len) {
  statusCode result = ok;
  result = initWifi(wifi);
  if (result == ok) {
    BufferDataUploader bdu(uploadSettings, buf, len, String(pictureName));
    result = bdu.upload();
  }
  return result;
}
