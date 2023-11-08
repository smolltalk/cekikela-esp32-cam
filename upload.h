#ifndef UPLOADER_H
#define UPLOADER_H

#include "Arduino.h"
#include "wifimgt.h"
#include "FS.h"
#include "sd.h"
#include "filename.h"
#include "error.h"
#include "timemgt.h"

#define SERVER_NAME_MAX_SIZE 64
#define UPLOAD_PATH_MAX_SIZE 64
#define AUTH_PATH_MAX_SIZE 32

typedef struct {
  bool enabled = false;
  char serverName[SERVER_NAME_MAX_SIZE + 1];
  int serverPort;
  char uploadPath[UPLOAD_PATH_MAX_SIZE + 1];
  char auth[AUTH_PATH_MAX_SIZE + 1];
  uint8_t bunchSize;
  uint8_t fileNameRandSize;
} uploadSettings_t;

statusCode uploadPictureFile(uploadSettings_t * uploadSettings, uint16_t i);
bool canUploadPictures(uint8_t bunchSize, FilesCounters * filesCounters);
statusCode uploadPictureFiles(wifiSettings_t * wifiSettings, uploadSettings_t * uploadSettings, FilesCounters * filesCounters);
statusCode uploadPicture(wifiSettings_t * wifiSettings, uploadSettings_t * uploadSettings, char * pictureName, uint8_t * buf, size_t len);

class DataUploader {
public:
  DataUploader(uploadSettings_t * uploadSettings, uint32_t dataLen, String fileName);
  statusCode upload();

protected:
  uploadSettings_t * uploadSettings;
  uint32_t dataLen;
  String fileName;
  WiFiClient client;

private:
  virtual void sendData();
};

class BufferDataUploader : public DataUploader {

public:
  BufferDataUploader(uploadSettings_t * uploadSettings, uint8_t * buf, size_t len, String fileName);

private:
  uint8_t * buf;

  void sendData();

};

class FileDataUploader : public DataUploader {
public:
  FileDataUploader(uploadSettings_t * uploadSettings, File file);

private:
  File file;

  void sendData();
};

#endif
