/*
 * audio.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_WAV_H_
#define INC_WAV_H_

/*
 * TODO: ditch this design sort of
 * wav shouldn't be an "audio driver" type, it should be a codec type
 * and then the "audio driver" type will be something like I2S, direct DAC, etc.
 */
#include "audio.h"

audio_ret_t Wav_Init(void);
audio_ret_t Wav_Close(void);
audio_ret_t Wav_Play(uint8_t *buffer, size_t length);

extern const audio_driver_t wav_driver;

#endif /* INC_WAV_H_ */
