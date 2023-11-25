#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_DISABLED 0

#define LOG_LEVEL LOG_LEVEL_DEBUG

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define logError(LOGGER, ARGS...) Serial.printf("[%s]   Error: ", LOGGER); Serial.printf(ARGS); Serial.println(); 
#else
#define logError(LOGGER, ARGS...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define logWarn(LOGGER, ARGS...)  Serial.printf("[%s] Warning: ", LOGGER); Serial.printf(ARGS); Serial.println(); 
#else
#define logWarn(LOGGER, ARGS...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define logInfo(LOGGER, ARGS...)  Serial.printf("[%s]    Info: ", LOGGER); Serial.printf(ARGS); Serial.println(); 
#else
#define logInfo(LOGGER, ARGS...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define logDebug(LOGGER, ARGS...)  Serial.printf("[%s]  Debug: ", LOGGER); Serial.printf(ARGS); Serial.println(); 
#else
#define logDebug(LOGGER, ARGS...)
#endif
