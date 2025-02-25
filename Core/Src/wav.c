/*
 * wav.c
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#include "wav.h"

static file_t curr_file;
static int file_already_opened = 0;

// https://en.wikipedia.org/wiki/WAV
// ** Data is stored in little-endian byte order. **

#define WAV_HEADER_SIZE 44

// Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
#define WAV_HEADER_RIFF_SIZE 4
#define WAV_HEADER_RIFF { 0x52, 0x49, 0x46, 0x46 }

// Overall file size minus 8 bytes
#define WAV_HEADER_FILESIZE_SIZE 4

// Format = « WAVE »
#define WAV_HEADER_FILEFORMATID_SIZE 4
#define WAV_HEADER_FILEFORMATID { 0x57, 0x41, 0x56, 0x45 }

// FormatBlocID
// Identifier « fmt␣ »
#define WAV_HEADER_FMT_SIZE 4
#define WAV_HEADER_FMT { 0x66, 0x6D, 0x74, 0x20 }

// BlocSize
// Chunk size minus 8 bytes, which is 16 bytes here  (0x10)
#define WAV_HEADER_BLOCSIZE_SIZE 4
#define WAV_HEADER_BLOCSIZE 0x10

// AudioFormat
// Audio format (1: PCM integer, 3: IEEE 754 float)
#define WAV_HEADER_AUDIOFORMAT_SIZE 2
#define WAV_HEADER_FORMAT_PCM 1
#define WAV_HEADER_FORMAT_IEEE754 3

// NbrChannels
// Number of channels
#define WAV_HEADER_NBRCHANNELS_SIZE 2

static codec_ret_t validateIdentifier(const uint8_t bytes[], int length)
{
    // Read the identifier
    uint8_t buffer[length];
    curr_file.Read(curr_file.handle, buffer, length);

    // Store the RIFF bytes
    // const uint8_t riff[WAV_HEADER_RIFF_SIZE] = WAV_HEADER_RIFF;

    // Check if all bytes match
    for (int i = 0; i < length; ++i) {
        if (buffer[i] != bytes[i]) {
            return CODEC_ERROR_INVALID_FILE_FORMAT;
        }
    }

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Open(const file_t *file)
{
    if (file_already_opened) {
        return CODEC_ERROR_FILE_ALREADY_OPENED;
    }

    if (file == NULL) {
        return CODEC_ERROR_FILE_IS_NULL;
    }

    curr_file = *file;
    file_already_opened = 1;

    // We will never read more than the size of the WAV header
    uint8_t buffer[WAV_HEADER_SIZE];
    codec_ret_t res;

    // Read the RIFF identifier
    const uint8_t riff[WAV_HEADER_RIFF_SIZE] = WAV_HEADER_RIFF;
    if ((res = validateIdentifier(riff, WAV_HEADER_RIFF_SIZE)) != CODEC_SUCCESS)
    {
        return res;
    }

    // Read the file size
    // TODO: store this?
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_FILESIZE_SIZE);

    // Read the file format identifier
    const uint8_t fileformatid[WAV_HEADER_FILEFORMATID_SIZE] = WAV_HEADER_FILEFORMATID;
    if ((res = validateIdentifier(fileformatid, WAV_HEADER_FILEFORMATID_SIZE)) != CODEC_SUCCESS)
    {
        return res;
    }

    // Read the fmt identifier
    const uint8_t fmt[WAV_HEADER_FMT_SIZE] = WAV_HEADER_FMT;
    if ((res = validateIdentifier(fmt, WAV_HEADER_FMT_SIZE)) != CODEC_SUCCESS)
    {
        return res;
    }

    // Read BlocSize
    // Remember that data is stored in little endian, so the first byte will have the LSB (idx 0)
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_BLOCSIZE_SIZE);
    if (buffer[0] != WAV_HEADER_BLOCSIZE)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    // Read AudioFormat
    // Only support PCM for now
    // TODO: add IEEE?
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_AUDIOFORMAT_SIZE);
    if (buffer[0] != WAV_HEADER_FORMAT_PCM)
    {
        return CODEC_ERROR_AUDIO_FORMAT_NOT_SUPPORTED;
    }

    // Read the number of channels
    // TODO: store this?
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_NBRCHANNELS_SIZE);

    // TODO: finish WAV header

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Close(void)
{
    if (!file_already_opened) {
        return CODEC_ERROR_NO_FILE_OPENED;
    }

    // No need to actually do anything to the curr_file object
    // We can simply change this flag so the Open function knows it can open new files
    file_already_opened = 0;

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Decode(void *buffer, size_t length)
{
    // Since WAV files are already uncompressed PCM, this is very straightforward
    fs_ret_t res = curr_file.Read(curr_file.handle, buffer, length);

    if (res != FS_SUCCESS) {
        return CODEC_ERROR_UNABLE_TO_DECODE;
    }

    return CODEC_SUCCESS;
}

const codec_t wav_codec =
{ .Open = WAV_Open, .Decode = WAV_Decode, .DecodeFrom = WAV_DecodeFrom };
