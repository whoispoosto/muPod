/*
 * wav.c
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#include "wav.h"

static const file_t *curr_file = NULL;

codec_ret_t WAV_Open(const file_t *file)
{
    if (curr_file != NULL) {
        return CODEC_ERROR_FILE_ALREADY_OPENED;
    }

    if (file == NULL) {
        return CODEC_ERROR_FILE_IS_NULL;
    }

    curr_file = file;

    // TODO: process wav header

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Decode(void *buffer, size_t length)
{
    // Since WAV files are already uncompressed PCM, this is very straightforward
    fs_ret_t res = curr_file->Read(curr_file->handle, buffer, length);

    if (res != FS_SUCCESS) {
        return CODEC_ERROR_UNABLE_TO_DECODE;
    }

    return CODEC_SUCCESS;
}

const codec_t wav_codec =
{ .Open = WAV_Open, .Decode = WAV_Decode, .DecodeFrom = WAV_DecodeFrom };
