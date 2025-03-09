/*
 * i2s.c
 *
 *  Created on: Feb 4, 2025
 *      Author: prestonmeek
 */
#include "i2s.h"
#include "main.h"

// TODO:
// extern I2S_HandleTypeDef hi2s2;

audio_ret_t I2S_Open(void) {
    // TODO

    return AUDIO_SUCCESS;
}

audio_ret_t I2S_Close(void) {
    // TODO

    return AUDIO_SUCCESS;
}

audio_ret_t I2S_Stream(void *buffer, size_t length) {
    if (buffer == NULL) {
        return AUDIO_ERROR_NULL_BUFFER;
    }

    // uint16_t *data = (uint16_t *)buffer;

    // TODO
    /* if (HAL_I2S_Transmit_IT(&hi2s2, data, length) != HAL_OK) {
        return AUDIO_ERROR_UNABLE_TO_STREAM_BUFFER;
    } */

    return AUDIO_SUCCESS;
}

const audio_driver_t i2s_driver =
{ .Open = I2S_Open, .Close = I2S_Close, .Stream = I2S_Stream };

