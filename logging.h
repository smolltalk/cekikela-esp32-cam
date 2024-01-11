/**
 * Logging tricks based on conditional preprocessor macros
 * to add logging instructions only when 
 * the log function level is <= to the #define LOG_LEVEL.
 * This very simple way saves memory and CPU.
 * The LOG_LEVEL applies to the whole application.
 */

#define LOG_LEVEL_DEBUG    4
#define LOG_LEVEL_INFO     3
#define LOG_LEVEL_WARN     2
#define LOG_LEVEL_ERROR    1
#define LOG_LEVEL_DISABLED 0

// Define here the application log level.
#define LOG_LEVEL LOG_LEVEL_DEBUG

// Error logger
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define logError(LOGGER, ARGS...) Serial.printf("\n%-8s|Error  | ", LOGGER); Serial.printf(ARGS);
#else
#define logError(LOGGER, ARGS...)
#endif

// Warning logger
#if LOG_LEVEL >= LOG_LEVEL_WARN
#define logWarn(LOGGER, ARGS...)  Serial.printf("\n%-8s|Warning| ", LOGGER); Serial.printf(ARGS);
#else
#define logWarn(LOGGER, ARGS...)
#endif

// Info logger
#if LOG_LEVEL >= LOG_LEVEL_INFO
#define logInfo(LOGGER, ARGS...)  Serial.printf("\n%-8s|Info   | ", LOGGER); Serial.printf(ARGS);
#else
#define logInfo(LOGGER, ARGS...)
#endif

// Debug logger
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define logDebug(LOGGER, ARGS...)  Serial.printf("\n%-8s|Debug  | ", LOGGER); Serial.printf(ARGS);
#else
#define logDebug(LOGGER, ARGS...)
#endif

// Useful to log a boolean value
// Ex: Serial.printf("param = %s", bool_str(paramValue));
#define bool_str(V) V ? "true" : "false"