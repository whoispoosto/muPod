/*
 * wav.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_WAV_H_
#define INC_WAV_H_

#include "codec.h"

#define WAV_HEADER_LEN 44

typedef struct {
    uint32_t file_size;         // Overall file size minus 8 bytes
    uint16_t nbr_channels;      // Number of channels
    uint32_t frequency;         // Sample rate (in hertz)
    uint32_t bytes_per_sec;     // Number of bytes to read per second (Frequency * BytePerBloc)
    uint16_t bytes_per_bloc;    // Number of bytes per block (NbrChannels * BitsPerSample / 8)
    uint16_t bits_per_sample;   // Number of bits per sample
    uint32_t data_size;         // SampledData size
} wav_metadata_t;

codec_ret_t WAV_Open(void);
codec_ret_t WAV_Close(void);
codec_ret_t WAV_ValidateHeader(const void *buffer, void *metadata, size_t *bytes_read);
codec_ret_t WAV_Decode(void *buffer, size_t length);
codec_ret_t WAV_DecodeFrom(void *buffer, size_t start, size_t length);
// codec_ret_t WAV_Encode(uint8_t *dst, const uint8_t *src, size_t length);

extern const codec_t wav_codec;

#endif /* INC_WAV_H_ */
