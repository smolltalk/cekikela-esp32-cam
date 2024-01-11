#ifndef UPLOADER_H
#define UPLOADER_H

#include "Arduino.h"
#include "error.h"
#include "filename.h"
#include "logging.h"
#include "timemgt.h"
#include "FS.h"
#include "sd.h"
#include "wifimgt.h"

// Logger name for this module
#define UPLOAD_LOG "Upload"

// Server address max size
#define UPLOAD_SERVER_ADDRESS_MAX_SIZE 64
// Default server port
#define UPLOAD_SERVER_PORT_DEFAULT 80
// Upload path max size
#define UPLOAD_PATH_MAX_SIZE 64
// Authorization max size
#define UPLOAD_AUTH_MAX_SIZE 32
// Default bunch size
#define UPLOAD_BUNCH_SIZE_DEFAULT 10
// Default file name random size
#define UPLOAD_FILE_NAME_RANDOM_SIZE 5

// Size of the upload buffer used by sendData()
#define UPLOAD_BUFFER_SIZE 1024

/**
 * Upload settings.
 */
typedef struct {
  bool enabled;                                       // True to enable the upload feature.
  char serverAddress[UPLOAD_SERVER_ADDRESS_MAX_SIZE]; // Server address (IP or FQDN) to which pictures are uploaded. Ex: pic.mydom.org.
  int serverPort;                                     // Server TCP port. Ex: 8080.
  char path[UPLOAD_PATH_MAX_SIZE];                    // URL path. Ex: /upload.php.
  char auth[UPLOAD_AUTH_MAX_SIZE];                    // Any secret string authorizing this device to upload.
  uint8_t bunchSize;                                  // Minimum number of pictures to upload.
  uint8_t fileNameRandSize;                           // Used when pictures are not stored on SD card.
                                                      // As no counter is maintained in this case, the file name is randomly generated.
                                                      // This parameter defines the number of random characters composing the uploaded file name.
} upload_settings_t;

/**
 * @brief Upload a SD stored picture file identified by its index.
 *
 * @param uploadSettings required to determine the upload destination
 * @param fileIndex
 *
 * @return IS_OK when it succeeds or UPLOAD_PICTURE_ERROR if the operation failed
 *
 * @see uploadPictureFiles()
 */
status_code_t uploadPictureFileByIndex(upload_settings_t* uploadSettings, uint16_t fileIndex);

/**
 * @brief Determine if there is a new bunch of files to upload.
 *
 * @param bunchSize
 * @param fileCounters
 *
 * @return true when there is a new bunch of files to upload
 *
 * @see uploadPictureFiles()
 */
bool canUploadPictures(uint8_t bunchSize, fileCounters_t* fileCounters);

/**
 * @brief Upload SD stored picture files that have not yet been uploaded.
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
status_code_t uploadPictureFiles(wifi_settings_t* wifiSettings, upload_settings_t* uploadSettings, fileCounters_t* fileCounters);

/**
 * @brief Upload a picture contained in a (frame) buffer.
 *
 * @param wifiSettings   required to establish the WiFi connection
 * @param uploadSettings required to determine the upload destination
 * @param destFileName   the uploaded destination file name that the server will see
 * @param srcBuffer      buffer containing the picture data
 * @param dataLen        the number of bytes of the payload to upload (i.e the picture size in byte)
 *
 * @return IS_OK when it succeeds
 *         or WIFI_INIT_ERROR when the WiFi can not be initialized
 *         or UPLOAD_PICTURE_ERROR if the operation failed
 */
status_code_t uploadPicture(wifi_settings_t* wifiSettings, upload_settings_t* uploadSettings, char* pictureName, uint8_t* srcBuffer, size_t dataLen);

/**
 * Abstract class in charge to upload one picture to the server
 * according to the given upload settings and the destination file name.
 *
 * @see upload_settings_t
 * @see BufferUploader
 * @see FileUploader
 */
class Uploader {
public:
  /**
   * Abstract constructor
   * 
   * @param uploadSettings required to determine the upload destination
   * @param dataLen        the number of bytes of the payload to upload (i.e the picture size in byte).
   * @param destFileName   the uploaded destination file name that the server will see. 
   */
  Uploader(upload_settings_t* uploadSettings, uint32_t dataLen, String destFileName);

  /**
   * Launch the data upload.
   *
   * @return IS_OK when it succeeds
   *         or WIFI_INIT_ERROR when the WiFi can not be initialized
   *         or UPLOAD_PICTURE_ERROR if the operation failed.
   */
  status_code_t upload();

protected:
  upload_settings_t* uploadSettings;  // Settings required to upload
  uint32_t dataLen;                   // Length of the data to upload
  String destFileName;                // The uploaded destination file name that the server will see
  WiFiClient client;                  // WiFi client

private:
  /**
   * Called by upload() to send payload data
   * with the WiFi client.
   * Must be implemented by subclasses, because
   * it depends on the source type: buffer or file.
   *
   * @see BufferUploader::sendData()
   * @see FileUploader::sendData()
   */
  virtual void sendData();
};

/**
 * Uploads a picture contained in a buffer.
 * Useful when the picture can not be stored in the SD card.
 * It reads the data directly from the sensor buffer.
 */
class BufferUploader : public Uploader {

public:
  /**
   * Constructor
   * 
   * @param uploadSettings required to determine the upload destination
   * @param srcBuffer      buffer containing the picture data.
   * @param dataLen        the number of bytes of the payload to upload (i.e the picture size in byte).
   * @param destFileName   the uploaded destination file name that the server will see. 
   */
  BufferUploader(upload_settings_t* uploadSettings, uint8_t* srcBuffer, size_t len, String destFileName);

private:
  uint8_t* srcBuffer;  // Buffer containing the picture data.

  /**
   * @see Uploader::sendData()
   */
  void sendData();
};

/**
 * Upload a picture contained in a file.
 * Useful when the picture is stored in the SD card.
 * It reads the data from the given file.
 * The file name on the server will be the same as the source file.
 */
class FileUploader : public Uploader {
public:
  /**
   * Constructor
   * 
   * @param uploadSettings required to determine the upload destination
   * @param srcFile        the file to upload
   */
  FileUploader(upload_settings_t* uploadSettings, File srcFile);

private:
  File srcFile;  // Source picture file

  /**
   * @see Uploader::sendData()
   */
  void sendData();
};

#endif
