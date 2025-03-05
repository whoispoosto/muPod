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
    uint32_t data_size;         // SampledData size
} wav_metadata_t;

#define WAV_HEADER_SIZE 44 // TODO: delete?

// Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
static const uint8_t WAV_HEADER_RIFF[] = { 0x52, 0x49, 0x46, 0x46 };

// Format = « WAVE »
static const uint8_t WAV_HEADER_FILEFORMATID[] = { 0x57, 0x41, 0x56, 0x45 };

// FormatBlocID
// Identifier « fmt␣ »
static const uint8_t WAV_HEADER_FMT[] = { 0x66, 0x6D, 0x74, 0x20 };

// BlocSize
// Chunk size minus 8 bytes, which is 16 bytes here  (0x10)
// Remember: data is stored in little-endian order!
// static const uint8_t WAV_HEADER_BLOCSIZE[] = { 0x10, 0x0, 0x0, 0x0 };
static const uint32_t WAV_HEADER_BLOCSIZE = 0x10;

// AudioFormat
// Audio format (1: PCM integer, 3: IEEE 754 float)
static const uint16_t WAV_HEADER_AUDIOFORMAT_PCM = 1;
// static const uint16_t WAV_HEADER_AUDIOFORMAT_IEEE754 = 3;

// DataBlocID
// Identifier « data »
static const uint8_t WAV_HEADER_DATABLOCID[] = { 0x64, 0x61, 0x74, 0x61 };

/*
 * Macros are really interesting!
 *
 * You can do curr_file.field, and it automatically expands to get the actual field
 *
 * #define STORE_METADATA_FIELD(meta, field, file) \
    do { \
        uint8_t _STORE_METADATA_BUFFER[sizeof(meta.field)]; \
        file.Read(file.handle, _STORE_METADATA_BUFFER, sizeof(meta.field)); \
        memcpy(&meta.field, _STORE_METADATA_BUFFER, sizeof(meta.field)); \
    } while (0)
 *
 * the do {} while (0) loop ensures that this block always works
 * for example, if there wasn't the do while, and this was used in a single-line if,
 * only the first line would run inside the if statement. The rest would always run!
 * The do {} while (0) loop also scopes things nicely for the sake of local variables
 *
 * Linux kernel even does stuff like #define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
 *
 * Inline functions are very similar but don't have cool features like parameter substitution.
 * They have type safety though!
 */


#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))

// Inspired by RCCHECK from ROS
#define WAV_ERR(call) { codec_ret_t res = call; if (res != CODEC_SUCCESS) { return res; } }

#define FILE_NOT_OPENED 0
#define FILE_OPENED 1

#define BUFFERS_MATCH 0

static file_t curr_file;
static wav_metadata_t metadata;
static int file_opened = FILE_NOT_OPENED;

static inline codec_ret_t VALIDATE_IDENTIFIER(const uint8_t *identifier, size_t length, const file_t *file)
{
    // We can't use sizeof(header) within this function
    // because C passes arrays by pointer.
    // So, the compile-time logic of sizeof() is only available in the stack frame
    // where the array is actually declared.
    // Thus, we explicitly pass in length.

    uint8_t buffer[length];

    if (file->Read(file->handle, buffer, length) != FS_SUCCESS)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    if (memcmp(buffer, identifier, length) != BUFFERS_MATCH)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    return CODEC_SUCCESS;
}

/*
 * We have two options: void pointers or specific pointers with overloading
 * Since we KNOW the size will be one of two pointers (uint16_t or uint32_t -- WAV specification isn't changing),
 * it makes way more sense to ensure type safety and use specific pointers.
 * If the pointer could be one of many, many types, then void pointers with memcpy makes sense.
 * Since the endianness of WAV and STM32 match, memcpy works fine.
 */
static inline codec_ret_t STORE_METADATA_FIELD_32(uint32_t *field, const file_t *file)
{
    if (file->Read(file->handle, &field, sizeof(uint32_t)) != FS_SUCCESS)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    return CODEC_SUCCESS;
}

static inline codec_ret_t STORE_METADATA_FIELD_16(uint16_t *field, const file_t *file)
{
    if (file->Read(file->handle, &field, sizeof(uint16_t)) != FS_SUCCESS)
    {
        return CODEC_ERROR_INVALID_FILE_FORMAT;
    }

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Open(const file_t *file)
{
    if (file_opened) {
        return CODEC_ERROR_FILE_ALREADY_OPENED;
    }

    if (file == NULL) {
        return CODEC_ERROR_FILE_IS_NULL;
    }

    curr_file = *file;
    file_opened = FILE_OPENED;

    // Read the RIFF identifier
    WAV_ERR(VALIDATE_IDENTIFIER(WAV_HEADER_RIFF, LEN(WAV_HEADER_RIFF), &curr_file));

    // Read the file size
    WAV_ERR(STORE_METADATA_FIELD_32(&metadata.file_size, &curr_file));

    // Read the file format identifier
    WAV_ERR(VALIDATE_IDENTIFIER(WAV_HEADER_FILEFORMATID, LEN(WAV_HEADER_FILEFORMATID), &curr_file));

    // Read the fmt identifier
    WAV_ERR(VALIDATE_IDENTIFIER(WAV_HEADER_FMT, LEN(WAV_HEADER_FMT), &curr_file));

    // Read BlocSize
    // Cast the uint32_t value to a uint8_t pointer
    // This is fine because everything is still tied to the length parameter
    WAV_ERR(VALIDATE_IDENTIFIER((const uint8_t *)&WAV_HEADER_BLOCSIZE, sizeof(WAV_HEADER_BLOCSIZE), &curr_file));

    // Read AudioFormat
    // Only support PCM for now
    // TODO: add IEEE. Would change this call to instead store the metadata field
    WAV_ERR(VALIDATE_IDENTIFIER((const uint8_t *)&WAV_HEADER_AUDIOFORMAT_PCM, sizeof(WAV_HEADER_AUDIOFORMAT_PCM), &curr_file));

    // Read the number of channels
    WAV_ERR(STORE_METADATA_FIELD_16(&metadata.nbr_channels, &curr_file));

    // Read the frequency
    WAV_ERR(STORE_METADATA_FIELD_32(&metadata.frequency, &curr_file));

    // Read the number of bytes per second
    WAV_ERR(STORE_METADATA_FIELD_32(&metadata.bytes_per_sec, &curr_file));

    // Read the number of bytes per block
    WAV_ERR(STORE_METADATA_FIELD_16(&metadata.bytes_per_bloc, &curr_file));

    // Read the number of bits per sample
    WAV_ERR(STORE_METADATA_FIELD_16(&metadata.bits_per_sample, &curr_file));

    // Read DataBlocID
    WAV_ERR(VALIDATE_IDENTIFIER(WAV_HEADER_DATABLOCID, LEN(WAV_HEADER_DATABLOCID), &curr_file));

    // Read DataSize
    WAV_ERR(STORE_METADATA_FIELD_32(&metadata.data_size, &curr_file));

    return CODEC_SUCCESS;
}

codec_ret_t WAV_Close(void)
{
    if (!file_opened) {
        return CODEC_ERROR_NO_FILE_OPENED;
    }

    // No need to actually do anything to the curr_file object
    // We can simply change this flag so the Open function knows it can open new files
    file_opened = FILE_NOT_OPENED;

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
