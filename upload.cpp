#include "upload.h"

statusCode initWifi() {
  statusCode result = ok;
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int connectAttemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && connectAttemptCount < WIFI_CONNECT_MAX_ATTEMPTS) {
    delay(500);
    Serial.print(".");
    connectAttemptCount++;
  }

  Serial.println("");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    result = uploadWifiInitError;
    Serial.println("Wifi NOT connected");
  }

  return result;
}

void endWifi() {
  WiFi.disconnect();
}

DataUploader::DataUploader(connectionInfo_t *connectionInfo, uint32_t dataLen, String fileName)
  : connectionInfo(connectionInfo), dataLen(dataLen), fileName(fileName){};

statusCode DataUploader::upload() {
  statusCode result = ok;

  Serial.printf("Connecting to server: %s\n", connectionInfo->serverName);

  if (client.connect(connectionInfo->serverName, connectionInfo->serverPort)) {
    Serial.println("Connection successful!");
    Serial.printf("Uploading file %s...\n", fileName);

    String head = "--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"auth\"\r\n\r\n" + String(connectionInfo->auth) + "\r\n--EspCamWebUpload\r\nContent-Disposition: form-data; name=\"fileToUpload\"; filename=\"" + fileName + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--EspCamWebUpload--\r\n";

    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = dataLen + extraLen;
 
    client.print("POST ");
    client.print(connectionInfo->uploadPath);
    client.println(" HTTP/1.1");
    client.print("Host: "); 
    client.println(connectionInfo->serverName);
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
    Serial.printf("Connection to %s failed.\n", connectionInfo->serverName);
    result = uploadPictureError;
  }
  return result;
};

void DataUploader::sendData() {}

BufferDataUploader::BufferDataUploader(connectionInfo_t *connectionInfo, uint8_t *buf, size_t len, String fileName)
  : DataUploader(connectionInfo, len, fileName), buf(buf){};

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

FileDataUploader::FileDataUploader(connectionInfo_t *connectionInfo, File file)
  : DataUploader(connectionInfo, file.size(), file.name()), file{ file } {};

void FileDataUploader::sendData() {
  uint8_t buf[256];
  size_t len = file.available();
  while (len) {
    client.write(buf, file.read(buf, len < 256 ? len : (uint16_t)256));
    len = file.available();
  }
}

statusCode uploadPictureFile(connectionInfo_t *connectionInfo, uint16_t i) {
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
    FileDataUploader fdu(connectionInfo, file);
    result = fdu.upload();
  }
  file.close();
  return result;
}

bool canUploadPictures(FilesMetaData *filesMetaData) {
  return (filesMetaData->pictureNumber - filesMetaData->uploadedPictureNumber >= UPLOAD_BUNCH_SIZE);
}

statusCode uploadPictureFiles(connectionInfo_t *connectionInfo, FilesMetaData *filesMetaData) {
  statusCode result = ok;
  if (canUploadPictures(filesMetaData)) {
    result = initWifi();
    if (result == ok) {
      for (uint16_t i = filesMetaData->uploadedPictureNumber + 1; i <= filesMetaData->pictureNumber; i++) {
        result = uploadPictureFile(connectionInfo, i);
        if (result == ok) {
          filesMetaData->uploadedPictureNumber++;
        } else {
          break;
        }
      }
      writeFilesMetaData(filesMetaData);
    }
  }
  return result;
}

statusCode uploadPicture(connectionInfo_t *connectionInfo, char *pictureName, uint8_t *buf, size_t len) {
  statusCode result = ok;
  result = initWifi();
  if (result == ok) {
    BufferDataUploader bdu(connectionInfo, buf, len, String(pictureName));
    result = bdu.upload();
  }
  return result;
}
