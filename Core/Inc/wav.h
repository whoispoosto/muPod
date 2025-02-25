/*
 * wav.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_WAV_H_
#define INC_WAV_H_

#include "codec.h"

codec_ret_t WAV_Open(const file_t *file);
codec_ret_t WAV_Decode(void *buffer, size_t length);
codec_ret_t WAV_DecodeFrom(void *buffer, size_t start, size_t length);
// codec_ret_t WAV_Encode(uint8_t *dst, const uint8_t *src, size_t length);

extern const codec_t wav_codec;

#endif /* INC_WAV_H_ */
