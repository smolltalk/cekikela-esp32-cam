#ifndef SD_H
#define SD_H

#include "error.h"
#include "FileConfig.h"
#include "logging.h"
#include "FS.h"
#include "SD_MMC.h"

// Logger name for this module
#define SD_LOG "SD"

// Name of the file storing the counters on the SD card
#define SD_FILES_COUNTERS_FILE_NAME "/counters.txt"
// Maximum length of a counter value in the file
#define SD_FILES_COUNTERS_VALUE_MAX_SIZE 10

/**
 * File counters.
 * Stored in a file on the SD card, it is used to
 * - name picture files on the SD card
 * - memorizes which files have been already uploaded
 * Ex: if pictureCounter is 25 and uploadedPictureCounter is 15
 * then the application knows that files between 16 and 25 are not
 * yet uploaded.
 * See the upload policy (upload_settings_t.bunchSize) and logs
 * to understand why counters are different.
 */
typedef struct {
  uint16_t pictureCounter;         // Counter of taken pictures used to name files stored on the SD card
  uint16_t uploadedPictureCounter; // Counter of uploaded pictures
} fileCounters_t;

/**
 * @brief Initialize (mount) the SD card.
 *
 * @return IS_OK when it succeeds or SD_INIT_ERROR in case of failure
 */
status_code_t initSdCard();

/**
 * Unmount the SD card.
 */
void endSdCard();

/**
 * @brief Save the picture data contained in pictureBuffer to a file named
 *        according to pictureFileName.
 *
 * @param pictureFileName a C string containing the picture file name
 * @param pictureBuffer a byte buffer containing the picture data to store to the file
 * @param pictureLen the length of data contained the pictureBuffer
 *
 * @return IS_OK when the operation succeeds. SD_WRITE_ERROR in case of failure
 */
status_code_t savePictureOnSdCard(char * pictureFileName, uint8_t * pictureBuffer, size_t pictureLen);

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
status_code_t loadFileCounters(fileCounters_t * fileCounters);  

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
status_code_t saveFileCounters(fileCounters_t * fileCounters);

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
status_code_t createFileCounters(fileCounters_t *fileCounters);

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
status_code_t loadOrCreateFileCounters(fileCounters_t * fileCounters);

#endif