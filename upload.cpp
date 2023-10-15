#include "upload.h"

DataUploader::DataUploader(uploadInfo_t *uploadInfo, uint32_t dataLen, String fileName)
  : uploadInfo(uploadInfo), dataLen(dataLen), fileName(fileName){};

statusCode DataUploader::upload() {
  statusCode result = ok;

  Serial.printf("Connecting to server: %s\n", uploadInfo->serverName);

  if (client.connect(uploadInfo->serverName, uploadInfo->serverPort)) {
    Serial.println("Connection successful!");
    Serial.printf("Uploading file %s...\n", fileName);

    String head = "--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"auth\"\r\n\r\n" + String(uploadInfo->auth) + "\r\n--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"fileToUpload\"; filename=\"" + fileName + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--EspCamWebUpload--\r\n";

    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = dataLen + extraLen;
 
    client.print("POST ");
    client.print(uploadInfo->uploadPath);
    client.println(" HTTP/1.1");
    client.print("Host: "); 
    client.println(uploadInfo->serverName);
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
    Serial.printf("Connection to %s failed.\n", uploadInfo->serverName);
    result = uploadPictureError;
  }
  return result;
};

void DataUploader::sendData() {}

BufferDataUploader::BufferDataUploader(uploadInfo_t *uploadInfo, uint8_t *buf, size_t len, String fileName)
  : DataUploader(uploadInfo, len, fileName), buf(buf){};

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

FileDataUploader::FileDataUploader(uploadInfo_t *uploadInfo, File file)
  : DataUploader(uploadInfo, file.size(), file.name()), file{ file } {};

void FileDataUploader::sendData() {
  uint8_t buf[256];
  size_t len = file.available();
  while (len) {
    client.write(buf, file.read(buf, len < 256 ? len : (uint16_t)256));
    len = file.available();
  }
}

statusCode uploadPictureFile(uploadInfo_t *uploadInfo, uint16_t i) {
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
    FileDataUploader fdu(uploadInfo, file);
    result = fdu.upload();
  }
  file.close();
  return result;
}

bool canUploadPictures(uint8_t bunchSize, FilesCounters *filesCounters) {
  return (filesCounters->pictureCounter - filesCounters->uploadedPictureCounter >= bunchSize);
}

statusCode uploadPictureFiles(wifiInfo_t * wifiInfo, uploadInfo_t *uploadInfo, FilesCounters *filesCounters) {
  statusCode result = ok;
  if (canUploadPictures(uploadInfo->bunchSize, filesCounters)) {
    result = initWifi(wifiInfo);
    if (result == ok) {
      for (uint16_t i = filesCounters->uploadedPictureCounter + 1; i <= filesCounters->pictureCounter; i++) {
        result = uploadPictureFile(uploadInfo, i);
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

statusCode uploadPicture(wifiInfo_t * wifiInfo, uploadInfo_t *uploadInfo, char *pictureName, uint8_t *buf, size_t len) {
  statusCode result = ok;
  result = initWifi(wifiInfo);
  if (result == ok) {
    BufferDataUploader bdu(uploadInfo, buf, len, String(pictureName));
    result = bdu.upload();
  }
  return result;
}
