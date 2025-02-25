/*
 * codec.h
 *
 *  Created on: Jan 31, 2025
 *      Author: prestonmeek
 */

#ifndef INC_CODEC_H
#define INC_CODEC_H

#include <stdint.h>
#include <stddef.h>

#include "fs.h"

typedef enum
{
    CODEC_SUCCESS = 0,
    CODEC_ERROR_FILE_ALREADY_OPENED = -1,
    CODEC_ERROR_FILE_IS_NULL = -2,
    CODEC_ERROR_UNABLE_TO_DECODE = -3,
    CODEC_ERROR_NO_FILE_OPENED = -4,
    CODEC_ERROR_INVALID_FILE_FORMAT = -5,
    CODEC_ERROR_AUDIO_FORMAT_NOT_SUPPORTED = -6,
    CODEC_ERROR_GENERIC = -128
} codec_ret_t; 

typedef struct
{
    codec_ret_t (*Open)(const file_t *file);
    codec_ret_t (*Close)(void);
    codec_ret_t (*Decode)(void *buffer, size_t length);
    codec_ret_t (*DecodeFrom)(void *buffer, size_t start, size_t length);
    // codec_ret_t (*Encode)(uint8_t *dst, const uint8_t *src, size_t length);
} codec_t;

#endif /* INC_CODEC_H_ */
