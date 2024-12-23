/*
 * audio.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_WAV_H_
#define INC_WAV_H_


#include "audio.h"

audio_ret_t Wav_Init(void);
audio_ret_t Wav_Close(void);
audio_ret_t Wav_Play(uint8_t *buffer, size_t length);

extern const audio_driver_t wav_driver;

#endif /* INC_WAV_H_ */
