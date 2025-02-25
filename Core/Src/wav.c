/*
 * wav.c
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#include "wav.h"

#include <string.h>

// https://en.wikipedia.org/wiki/WAV
// ** Data is stored in little-endian byte order. **

typedef struct {
    uint32_t file_size;         // Overall file size minus 8 bytes
    uint16_t nbr_channels;      // Number of channels
    uint32_t frequency;         // Sample rate (in hertz)
    uint32_t bytes_per_sec;     // Number of bytes to read per second (Frequency * BytePerBloc)
    uint16_t bytes_per_bloc;    // Number of bytes per block (NbrChannels * BitsPerSample / 8)
    uint16_t bits_per_sample;   // Number of bits per sample

} wav_metadata_t;

#define WAV_HEADER_SIZE 44

// Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
#define WAV_HEADER_RIFF_SIZE 4
#define WAV_HEADER_RIFF { 0x52, 0x49, 0x46, 0x46 }

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

static file_t curr_file;
static wav_metadata_t metadata;
static int file_already_opened = 0;

/*
 * Macros are really interesting!
 *
 * You can do curr_file.field, and it automatically expands to get the actual field
 *
 * the do {} while (0) loop ensures that this block always works
 * for example, if there wasn't the do while, and this was used in a single-line if,
 * only the first line would run inside the if statement. The rest would always run!
 * The do {} while (0) loop also scopes things nicely for the sake of local variables
 *
 * Linux kernel even does stuff like #define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
 */
#define STORE_METADATA_FIELD(field) \
    do { \
        uint8_t _STORE_METADATA_BUFFER[sizeof(metadata.field)]; \
        curr_file.Read(curr_file.handle, _STORE_METADATA_BUFFER, sizeof(metadata.field)); \
        memcpy(&metadata.field, _STORE_METADATA_BUFFER, sizeof(metadata.field)); \
    } while (0)

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

    // Read the RIFF identifier
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_RIFF_SIZE);
    const uint8_t riff[WAV_HEADER_RIFF_SIZE] = WAV_HEADER_RIFF;

    // memcmp returns 0 when all bytes match
    if (memcmp(buffer, riff, WAV_HEADER_RIFF_SIZE) != 0)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    // Read the file size
    // curr_file.Read(curr_file.handle, buffer, sizeof(metadata.file_size));
    // metadata.file_size = ((uint32_t *)buffer)[0];
    // TODO: make sure macro works
    STORE_METADATA_FIELD(file_size);

    // Read the file format identifier
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_FILEFORMATID_SIZE);
    const uint8_t fileformatid[WAV_HEADER_FILEFORMATID_SIZE] = WAV_HEADER_FILEFORMATID;

    if (memcmp(buffer, fileformatid, WAV_HEADER_FILEFORMATID_SIZE) != 0)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    // Read the fmt identifier
    curr_file.Read(curr_file.handle, buffer, WAV_HEADER_FMT_SIZE);
    const uint8_t fmt[WAV_HEADER_FMT_SIZE] = WAV_HEADER_FMT;

    if (memcmp(buffer, fmt, WAV_HEADER_FMT_SIZE) != 0)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
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
    curr_file.Read(curr_file.handle, buffer, sizeof(metadata.nbr_channels));
    metadata.nbr_channels = ((typeof(metadata.nbr_channels) *)buffer)[0];

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
