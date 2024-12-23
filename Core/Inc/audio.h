/*
 * audio.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    AUDIO_SUCCESS = 0,
    AUDIO_ERROR_GENERIC = -128
} audio_ret_t;

typedef struct
{
    audio_ret_t (*Init)(void);
    audio_ret_t (*Close)(void);
    audio_ret_t (*Play)(uint8_t *buffer, size_t length);
    // TODO: ioctl?
} audio_driver_t;

#endif /* INC_AUDIO_H_ */
