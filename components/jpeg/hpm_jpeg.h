/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __HPM_JPEG_H
#define __HPM_JPEG_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifndef HPM_JPEG_LOG_LEVEL
#define HPM_JPEG_LOG_LEVEL HPM_JPEG_LOG_LEVEL_WARN
#endif

#define HPM_JPEG_LOG_LEVEL_DEBUG 4
#define HPM_JPEG_LOG_LEVEL_INFO  3
#define HPM_JPEG_LOG_LEVEL_WARN  2
#define HPM_JPEG_LOG_LEVEL_ERR   1

#define HPM_JPEG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__);

#if HPM_JPEG_LOG_LEVEL >= HPM_JPEG_LOG_LEVEL_DEBUG
#define HPM_JPEG_DLOG(fmt, ...) ({\
    printf("HPM_JPEG: [D: %s:%u]: ", __func__, __LINE__);\
    printf(fmt, ##__VA_ARGS__);\
})
#else
#define HPM_JPEG_DLOG(fmt, ...)
#endif

#if HPM_JPEG_LOG_LEVEL >= HPM_JPEG_LOG_LEVEL_INFO
#define HPM_JPEG_ILOG(fmt, ...) ({\
    printf("HPM_JPEG: [I: %s:%u]: ", __func__, __LINE__);\
    printf(fmt, ##__VA_ARGS__);\
})
#else
#define HPM_JPEG_ILOG(fmt, ...)
#endif

#if HPM_JPEG_LOG_LEVEL >= HPM_JPEG_LOG_LEVEL_WARN
#define HPM_JPEG_WLOG(fmt, ...) ({\
    printf("HPM_JPEG: [W: %s:%u]: ", __func__, __LINE__);\
    printf(fmt, ##__VA_ARGS__);\
})
#else
#define HPM_JPEG_WLOG(fmt, ...)
#endif

#if HPM_JPEG_LOG_LEVEL >= HPM_JPEG_LOG_LEVEL_ERR
#define HPM_JPEG_ELOG(fmt, ...) ({\
    printf("HPM_JPEG: [E: %s:%u]: ", __func__, __LINE__);\
    printf(fmt, ##__VA_ARGS__);\
})
#else
#define HPM_JPEG_ELOG(fmt, ...)
#endif

#define HPM_JPEG_RET_OK               0
#define HPM_JPEG_RET_MALLOC_ERR      -1
#define HPM_JPEG_RET_PARA_ERR        -2
#define HPM_JPEG_RET_JPEG_FILE_ERROR -3
#define HPM_JPEG_RET_STATUS_ERR      -4
#define HPM_JPEG_RET_MARKER_ERR      -5

typedef void hpm_jpeg_job_t;

typedef enum hpm_jpeg_sampling_format {
    HPM_JPEG_SAMPLING_FORMAT_420 = 0, /**< hy=2, vy=2, hc=1, vc=1 */
    HPM_JPEG_SAMPLING_FORMAT_422H,    /**< hy=2, vy=1, hc=1, vc=1 */
    HPM_JPEG_SAMPLING_FORMAT_422V,    /**< hy=1, vy=2, hc=1, vc=1 */
    HPM_JPEG_SAMPLING_FORMAT_444,     /**< hy=1, vy=1, hc=1, vc=1 */
    HPM_JPEG_SAMPLING_FORMAT_400,     /**< hy=2, vy=2, hc=0, vc=0 */
} hpm_jpeg_sampling_format_t;

typedef enum hpm_jpeg_image_format {/**< memory layout: byte0, byte1, byte2...*/
    HPM_JPEG_IMAGE_FORMAT_RGB565,   /**< memory: [G2:0,B4:0], [R4:0,G5:3], [G2:0,B4:0], [R4:0,G5:3], ... */
    HPM_JPEG_IMAGE_FORMAT_ARGB8888, /**< memory: B, G, R, A, B, G, ... */
    HPM_JPEG_IMAGE_FORMAT_BGRA8888, /**< memory: A, R, G, B, A, R, ... */
    HPM_JPEG_IMAGE_FORMAT_YUYV422,  /**< memory: Y0, U0, Y1, V1, ... */
    HPM_JPEG_IMAGE_FORMAT_UYVY422,  /**< memory: U0, Y0, V1, Y1, ... */
    HPM_JPEG_IMAGE_FORMAT_Y8,       /**< memory: Y0, Y1, Y2, Y3, ... */
} hpm_jpeg_image_format_t;

typedef struct hpm_jpeg_image {
    void *image_buf;
    enum hpm_jpeg_image_format format;
    uint32_t width;  /**< uint: pixels */
    uint32_t height; /**< uint: pixels */
    uint32_t stride; /**< uint: byte. The number of bytes in memory between two vertically adjacent pixels */
} hpm_jpeg_image_t;

typedef struct hpm_jpeg_jpeg_file {
    void *jpeg_buf;
    uint32_t len;
} hpm_jpeg_file_t;

typedef enum hpm_jpeg_job_status {
    HPM_JPEG_JOB_STATUS_ALLOCED = 0,
    HPM_JPEG_JOB_STATUS_FILLING,
    HPM_JPEG_JOB_STATUS_FILL_FAILED,
    HPM_JPEG_JOB_STATUS_FILLED,
    HPM_JPEG_JOB_STATUS_STARTING,
    HPM_JPEG_JOB_STATUS_START_FAILED,
    HPM_JPEG_JOB_STATUS_WAITING,
    HPM_JPEG_JOB_STATUS_PROCESSING,
    HPM_JPEG_JOB_STATUS_FINISHED,
    HPM_JPEG_JOB_STATUS_STOPPED,
    HPM_JPEG_JOB_STATUS_ERROR
} hpm_jpeg_job_status_t;

typedef struct hpm_jpeg_decode_info {
    uint32_t hw_status;
    hpm_jpeg_job_status_t status;
    const hpm_jpeg_image_t *image;
} hpm_jpeg_decode_info_t;

typedef struct hpm_jpeg_encode_info {
    uint32_t hw_status;
    hpm_jpeg_job_status_t status;
    const hpm_jpeg_file_t *file;
} hpm_jpeg_encode_info_t;

typedef struct hpm_jpeg_decode_cfg {
    hpm_jpeg_image_format_t out_format;
    uint32_t out_image_stride; /**< 0: auto calculate, other: user specified */
} hpm_jpeg_decode_cfg_t;

typedef struct hpm_jpeg_encode_cfg {
    hpm_jpeg_sampling_format_t jpeg_sampling;
    uint32_t jpeg_quality; /**< 0 - 100 */
} hpm_jpeg_encode_cfg_t;

/**
 * @brief Job't state info notice callback
 *
 * @param [in] job hpm_jpeg job pointer
 *
 * Note: The function is called in isr, long-time or complex operation are not recommend.
 */
typedef void (*hpm_jpeg_job_state_cb_t)(hpm_jpeg_job_t *job);

typedef struct hpm_jpeg_cfg {
    void *jpeg_base;
    void * (*malloc)(uint32_t size);
    void (*free)(void *p);
    uint32_t cache_line_size;
    void (*cache_writeback)(void *start, uint32_t size);
    void (*cache_invalidate)(void *start, uint32_t size);
} hpm_jpeg_cfg_t;

/**
 * @brief Initializte hpm_jpeg component
 *
 * @param [in] cfg hpm_jpeg configuration pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_init(hpm_jpeg_cfg_t *cfg);

/**
 * @brief Deinitializte hpm_jpeg component and free source
 *
 * @return failed code
 */
int hpm_jpeg_deinit(void);

/**
 * @brief jpeg ip interrupt service routine
 *
 */
void hpm_jpeg_isr(void);

/**
 * @brief Alloc a decode job
 *
 * @param [in] cfg job configuration pointer
 *
 * @return decode job pointer
 *
 */
hpm_jpeg_job_t *hpm_jpeg_decode_job_alloc(hpm_jpeg_decode_cfg_t *cfg);

/**
 * @brief Fill jpeg file to job
 *
 * @param [in] job decode job pointer
 * @param [in] file jpeg file data
 * @param [in] direct 1: The buffer of file will be used by decoder.
 *                    0: Firstly, the buffer of file is copied to internal buffer
 *                          and the internal buffer will be used.
 *
 * @return failed code
 *
 */
int hpm_jpeg_decode_job_fill_file(hpm_jpeg_job_t *job, const hpm_jpeg_file_t *file, int direct);

/**
 * @brief Set a buffer that is used by decoder out image to job
 *
 * @param [in] job decode job pointer
 * @param [in] buf Receive decode image data. the buf will be used decoder, so nocacheable buffer is recommend.
 * @param [in] len The buf length
 *
 * @return failed code
 *
 * Note: Internal buffer will be used if the buf have not been set.
 */
int hpm_jpeg_decode_job_force_direct_image_buf(hpm_jpeg_job_t *job, void *buf, uint32_t len);

/**
 * @brief Start decode of the job
 *
 * @param [in] job decode job pointer
 * @param [in] cb job state notice callback
 *
 * @return failed code
 *
 * Note: The cb is called in isr, long-time or complex operation are not recommend.
 */
int hpm_jpeg_decode_job_start(hpm_jpeg_job_t *job, hpm_jpeg_job_state_cb_t cb);

/**
 * @brief Stop decode of the job
 *
 * @param [in] job decode job pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_decode_job_stop(hpm_jpeg_job_t *job);

/**
 * @brief Get job work info
 *
 * @param [in] job decode job pointer
 * @param [out] info job work info pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_decode_job_get_info(hpm_jpeg_job_t *job, hpm_jpeg_decode_info_t *info);

/**
 * @brief Free decode job. All source of job will be free
 *
 * @param [in] job decode job pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_decode_job_free(hpm_jpeg_job_t *job);

/**
 * @brief Alloc a encode job
 *
 * @param [in] cfg job configuration pointer
 *
 * @return encode job pointer
 *
 */
hpm_jpeg_job_t *hpm_jpeg_encode_job_alloc(hpm_jpeg_encode_cfg_t *cfg);

/**
 * @brief Fill image data to job
 *
 * @param [in] job encode job pointer
 * @param [in] image image data for waiting encode
 * @param [in] direct 1: The buffer of file will be used by encoder.
 *                    0: Firstly, the buffer of image is copied to internal buffer
 *                          and the internal buffer will be used.
 *
 * @return failed code
 *
 */
int hpm_jpeg_encode_job_fill_image(hpm_jpeg_job_t *job, const hpm_jpeg_image_t *image, int direct);

/**
 * @brief Set a buffer that is used by encoder out jpeg file to job
 *
 * @param [in] job encode job pointer
 * @param [in] buf Receive encode jpeg file data. the buf will be used encoder, so nocacheable buffer is recommend.
 * @param [in] len The buf length
 *
 * @return failed code
 *
 * Note: Internal buffer will be used if the buf have not been set.
 */
int hpm_jpeg_encode_job_force_direct_file_buf(hpm_jpeg_job_t *job, void *buf, uint32_t len);

/**
 * @brief Start encode of the job
 *
 * @param [in] job encode job pointer
 * @param [in] cb job state notice callback
 *
 * @return failed code
 *
 * Note: The cb is called in isr, long-time or complex operation are not recommend.
 */
int hpm_jpeg_encode_job_start(hpm_jpeg_job_t *job, hpm_jpeg_job_state_cb_t cb);

/**
 * @brief Stop encode of the job
 *
 * @param [in] job encode job pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_encode_job_stop(hpm_jpeg_job_t *job);

/**
 * @brief Get job work info
 *
 * @param [in] job encode job pointer
 * @param [out] info job work info pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_encode_job_get_info(hpm_jpeg_job_t *job, hpm_jpeg_encode_info_t *info);

/**
 * @brief Free encode job. All source of job will be free
 *
 * @param [in] job encode job pointer
 *
 * @return failed code
 *
 */
int hpm_jpeg_encode_job_free(hpm_jpeg_job_t *job);

/**
 * @brief Set user data to job
 *
 * @param [in] job job pointer
 * @param [in] user_data user data pointer
 *
 */
void hpm_jpeg_job_set_user_data(hpm_jpeg_job_t *job, void *user_data);

/**
 * @brief Get user data to job
 *
 * @param [in] job job pointer
 *
 * @return user data pointer
 *
 */
void *hpm_jpeg_job_get_user_data(hpm_jpeg_job_t *job);

#endif