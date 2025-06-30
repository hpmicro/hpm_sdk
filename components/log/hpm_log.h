/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_LOG_H
#define HPM_LOG_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
 * @def HPM_LOG_CONFIG_MAX_ENGINE_NUM
 * @brief Maximum number of log engines.
 *
 * This macro defines the maximum number of log engines that can be created.
 * If not defined externally, the default value is 1.
 */
#ifndef HPM_LOG_CONFIG_MAX_ENGINE_NUM
#define HPM_LOG_CONFIG_MAX_ENGINE_NUM 1
#endif

/**
 * @def HPM_LOG_CONFIG_MAX_MESSAGE_LEN
 * @brief Maximum length of a log message.
 *
 * This macro defines the maximum length of a log message.
 * If not defined externally, the default value is 256.
 */
#ifndef HPM_LOG_CONFIG_MAX_MESSAGE_LEN
#define HPM_LOG_CONFIG_MAX_MESSAGE_LEN 256
#endif

/**
 * @def HPM_LOG_CONFIG_ENABLE_TIMESTAMP
 * @brief Enable timestamp in log messages.
 *
 * This macro controls whether timestamps are included in log messages.
 * If set to 1, timestamps will be included; if set to 0, they will be omitted.
 * If not defined externally, the default value is 1.
 */
#ifndef HPM_LOG_CONFIG_ENABLE_TIMESTAMP
#define HPM_LOG_CONFIG_ENABLE_TIMESTAMP 1
#endif

/**
 * @def HPM_LOG_CONFIG_ENABLE_FUNC_NAME
 * @brief Enable function name in log messages.
 *
 * This macro controls whether function names are included in log messages.
 * If set to 1, function names will be included; if set to 0, they will be omitted.
 * If not defined externally, the default value is 1.
 */
#ifndef HPM_LOG_CONFIG_ENABLE_FUNC_NAME
#define HPM_LOG_CONFIG_ENABLE_FUNC_NAME 1
#endif

/**
 * @def HPM_LOG_CONFIG_LEVEL
 * @brief Log level configuration.
 *
 * This macro defines the current log level. Log messages with a level lower than this
 * configuration will be ignored. If not defined externally, the default value is
 * HPM_LOG_LEVEL_DEBUG.
 */
#ifndef HPM_LOG_CONFIG_LEVEL
#define HPM_LOG_CONFIG_LEVEL HPM_LOG_LEVEL_DEBUG
#endif

/**
 * @def HPM_LOG_FAST_FUNC
 * @brief Indicate fast functions.
 *
 * This macro is used to mark functions that are expected to be fast.
 */
#define HPM_LOG_FAST_FUNC

/**
 * @def HPM_LOG_LEVEL_DEBUG
 * @brief Debug log level.
 *
 * This macro represents the debug log level. It is used for detailed debugging information.
 */
#define HPM_LOG_LEVEL_DEBUG 4

/**
 * @def HPM_LOG_LEVEL_INFO
 * @brief Information log level.
 *
 * This macro represents the information log level. It is used for general information messages.
 */
#define HPM_LOG_LEVEL_INFO  3

/**
 * @def HPM_LOG_LEVEL_WARN
 * @brief Warning log level.
 *
 * This macro represents the warning log level. It is used for non-critical warnings.
 */
#define HPM_LOG_LEVEL_WARN  2

/**
 * @def HPM_LOG_LEVEL_ERR
 * @brief Error log level.
 *
 * This macro represents the error log level. It is used for critical error messages.
 */
#define HPM_LOG_LEVEL_ERR   1

/**
 * @typedef hpm_log_engine_t
 * @brief Forward declaration of the log engine structure.
 */
typedef struct hpm_log_engine hpm_log_engine_t;

/**
 * @struct hpm_log_engine_config_t
 * @brief Configuration structure for the log engine.
 *
 * This structure contains the configuration parameters for a log engine.
 */
typedef struct hpm_log_engine_config {
    /**
     * @brief Function pointer to start data transfer.
     *
     * This function is called to start the transfer of a buffer of data.
     * @param buf Pointer to the buffer containing the data to be transferred.
     * @param len Length of the data in the buffer.
     */
    void (*transfer_start)(const char *buf, uint32_t len);
    /**
     * @brief Function pointer to transfer a block of data.
     *
     * This function is called to transfer a block of data.
     * @param buf Pointer to the buffer containing the data to be transferred.
     * @param len Length of the data in the buffer.
     */
    void (*transfer_block)(const char *buf, uint32_t len);
    /**
     * @brief Pointer to the transfer buffer.
     *
     * This buffer is used to hold the data to be transferred(if DMA is used, the buffer can use to transfer).
     */
    char *transfer_buf;
    /**
     * @brief Maximum size of the transfer buffer.
     *
     * This value specifies the maximum number of bytes that can be transferred at once.
     */
    uint32_t max_transfer_size;
    /**
     * @brief Pointer to the FIFO buffer.
     *
     * This buffer is used as a FIFO queue to store log messages before transfer.
     */
    char *fifo_buf;
    /**
     * @brief Size of the FIFO buffer.
     *
     * This value specifies the maximum number of bytes that the FIFO buffer can hold.
     */
    uint32_t fifo_buf_size;
} hpm_log_engine_config_t;

/**
 * @struct hpm_log_config_t
 * @brief Configuration structure for the log module.
 *
 * This structure contains the configuration parameters for the entire log module.
 */
typedef struct hpm_log_config {
    /**
     * @brief Function pointer to get the current timestamp.
     *
     * This function is called to obtain the current timestamp.
     * @return The current timestamp.
     */
    uint64_t (*get_timestamp)(void);
    /**
     * @brief Function pointer to enter a critical section.
     *
     * This function is called to enter a critical section to protect shared resources.
     * @return A value that can be used to restore the previous state.
     */
    long (*critical_enter)(void);
    /**
     * @brief Function pointer to exit a critical section.
     *
     * This function is called to exit a critical section and restore the previous state.
     * @param val The value obtained from the corresponding critical_enter call.
     */
    void (*critical_exit)(long val);
} hpm_log_config_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a log engine.
 *
 * This function creates a new log engine with the specified configuration.
 * @param cfg Pointer to the log engine configuration structure.
 * @return Pointer to the created log engine, or NULL if creation fails.
 */
hpm_log_engine_t *hpm_log_engine_create(hpm_log_engine_config_t *cfg);

/**
 * @brief Destroy a log engine.
 *
 * This function destroys the specified log engine and releases its resources.
 * @param engine Pointer to the log engine to be destroyed.
 * @return 0 success, or -1 if destroy fails.
 */
int hpm_log_engine_destroy(hpm_log_engine_t *engine);

/**
 * @brief Enable a log engine.
 *
 * This function enables the specified log engine, allowing it to start processing log messages.
 * @param engine Pointer to the log engine to be enabled.
 */
void hpm_log_engine_enable(hpm_log_engine_t *engine);

/**
 * @brief Disable a log engine.
 *
 * This function disables the specified log engine, preventing it from processing log messages.
 * @param engine Pointer to the log engine to be disabled.
 */
void hpm_log_engine_disable(hpm_log_engine_t *engine);

/**
 * @brief Indicate that the transfer of a log message is finished.
 *
 * This function is called to indicate that the transfer of a log message has finished.
 * It may trigger the transfer of the next message in the FIFO buffer.
 * @param engine Pointer to the log engine for which the transfer has finished.
 */
HPM_LOG_FAST_FUNC void hpm_log_engine_transfer_finish(hpm_log_engine_t *engine);

/**
 * @brief Dump all log messages in the FIFO buffer.
 *
 * This function transfers all log messages currently in the FIFO buffer of the specified log engine.
 * @param engine Pointer to the log engine whose FIFO buffer should be dumped.
 */
HPM_LOG_FAST_FUNC void hpm_log_engine_dump(hpm_log_engine_t *engine);

/**
 * @brief Initialize the log module.
 *
 * This function initializes the log module with the specified configuration.
 * @param cfg Pointer to the log module configuration structure. if NULL, the default is used.
 */
void hpm_log_init(hpm_log_config_t *cfg);

/**
 * @brief Print a formatted log message.
 *
 * This function prints a formatted log message using the specified format string and arguments.
 * @param fmt Format string for the log message.
 * @param ... Variable arguments corresponding to the format string.
 */
void hpm_log_printf(const char *fmt, ...);

/**
 * @brief Write a log message with a specified level.
 *
 * This function writes a log message with the specified log level, function name, line number,
 * and format string and arguments.
 * @param level Log level of the message.
 * @param func Name of the function where the log message is generated.
 * @param line Line number in the source file where the log message is generated.
 * @param fmt Format string for the log message.
 * @param ... Variable arguments corresponding to the format string.
 */
void hpm_log_write(int level, const char *func, const int line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

/**
 * @def HPM_LOG_PRINTF
 * @brief Macro to print a formatted log message.
 *
 * This macro calls the hpm_log_printf function with the specified format string and arguments.
 * @param fmt Format string for the log message.
 * @param ... Variable arguments corresponding to the format string.
 */
#define HPM_LOG_PRINTF(fmt, ...) hpm_log_printf(fmt, ##__VA_ARGS__);

/**
 * @def HPM_LOG_DEBUG
 * @brief Macro to log a debug message.
 *
 * This macro logs a debug message if the current log level is set to HPM_LOG_LEVEL_DEBUG or higher.
 * @param fmt Format string for the debug message.
 * @param ... Variable arguments corresponding to the format string.
 */
#if HPM_LOG_CONFIG_LEVEL >= HPM_LOG_LEVEL_DEBUG
#define HPM_LOG_DEBUG(fmt, ...) ({\
    hpm_log_write(HPM_LOG_LEVEL_DEBUG, __func__, __LINE__, fmt, ##__VA_ARGS__);\
})
#else
#define HPM_LOG_DEBUG(fmt, ...)
#endif

/**
 * @def HPM_LOG_INFO
 * @brief Macro to log an information message.
 *
 * This macro logs an information message if the current log level is set to HPM_LOG_LEVEL_INFO or higher.
 * @param fmt Format string for the information message.
 * @param ... Variable arguments corresponding to the format string.
 */
#if HPM_LOG_CONFIG_LEVEL >= HPM_LOG_LEVEL_INFO
#define HPM_LOG_INFO(fmt, ...) ({\
    hpm_log_write(HPM_LOG_LEVEL_INFO, __func__, __LINE__, fmt, ##__VA_ARGS__);\
})
#else
#define HPM_LOG_INFO(fmt, ...)
#endif

/**
 * @def HPM_LOG_WARN
 * @brief Macro to log a warning message.
 *
 * This macro logs a warning message if the current log level is set to HPM_LOG_LEVEL_WARN or higher.
 * @param fmt Format string for the warning message.
 * @param ... Variable arguments corresponding to the format string.
 */
#if HPM_LOG_CONFIG_LEVEL >= HPM_LOG_LEVEL_WARN
#define HPM_LOG_WARN(fmt, ...) ({\
    hpm_log_write(HPM_LOG_LEVEL_WARN, __func__, __LINE__, fmt, ##__VA_ARGS__);\
})
#else
#define HPM_LOG_WARN(fmt, ...)
#endif

/**
 * @def HPM_LOG_ERR
 * @brief Macro to log an error message.
 *
 * This macro logs an error message if the current log level is set to HPM_LOG_LEVEL_ERR or higher.
 * @param fmt Format string for the error message.
 * @param ... Variable arguments corresponding to the format string.
 */
#if HPM_LOG_CONFIG_LEVEL >= HPM_LOG_LEVEL_ERR
#define HPM_LOG_ERR(fmt, ...) ({\
    hpm_log_write(HPM_LOG_LEVEL_ERR, __func__, __LINE__, fmt, ##__VA_ARGS__);\
})
#else
#define HPM_LOG_ERR(fmt, ...)
#endif

#endif