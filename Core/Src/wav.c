/*
 * wav.c
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#include "wav.h"

audio_ret_t Wav_Init(void)
{
    // TODO
}

audio_ret_t Wav_Close(void)
{
    // TODO
}

audio_ret_t Wav_Play(uint8_t *buffer, size_t length)
{
    // TODO
}

const audio_driver_t wav_driver =
{ .Init = Wav_Init, .Close = Wav_Close, .Play = Wav_Play };
