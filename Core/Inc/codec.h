/*
 * codec.h
 *
 *  Created on: Dec 22, 2024
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

typedef struct
{
    codec_ret_t (*Init)(void);
    codec_ret_t (*Close)(void);
    codec_ret_t (*Decode)(uint8_t *dst, const uint8_t *src, size_t length);
    // TODO: ioctl?
} codec_driver_t;

#endif /* INC_CODEC_H_ */
