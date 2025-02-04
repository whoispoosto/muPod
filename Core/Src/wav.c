/*
 * wav.c
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#include "wav.h"

codec_ret_t WAV_Encode(uint8_t *dst, const uint8_t *src, size_t length)
{
    // Since WAV files are already uncompressed PCM, this is very straightforward
    for (int i = 0; i < length; ++i)
    {
        dst[i] = src[i];
    }

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Decode(uint8_t *dst, const uint8_t *src, size_t length)
{
    // Since WAV files are already uncompressed PCM, this is very straightforward
    for (int i = 0; i < length; ++i)
    {
        dst[i] = src[i];
    }

    return CODEC_SUCCESS;
}

const codec_t wav_codec =
{ .Encode = WAV_Encode, .Decode = WAV_Decode };
