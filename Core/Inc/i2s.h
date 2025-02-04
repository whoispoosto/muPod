/*
 * i2s.h
 *
 *  Created on: Feb 4, 2025
 *      Author: prestonmeek
 */

#ifndef INC_I2S_H_
#define INC_I2S_H_

#include "audio.h"

audio_ret_t I2S_Open(void);
audio_ret_t I2S_Close(void);
audio_ret_t I2S_Stream(void *buffer, size_t length);

extern const audio_driver_t i2s_driver;

#endif /* INC_I2S_H_ */
