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

typedef enum
{
    CODEC_SUCCESS = 0,
    CODEC_ERROR_GENERIC = -128
} codec_ret_t; 

// TODO:
typedef struct
{
    codec_ret_t (*Encode)(uint8_t *dst, const uint8_t *src, size_t length);
    codec_ret_t (*Decode)(uint8_t *dst, const uint8_t *src, size_t length);
} codec_t;

#endif /* INC_CODEC_H_ */
