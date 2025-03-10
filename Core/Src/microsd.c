/*
 * microsd.c
 *
 *  Created on: Dec 19, 2024
 *      Author: prestonmeek
 */

#include "microsd.h"

// Defined in main.c, used as an extern variable (just like here) in the built-in FATFS driver code
// Declare it within the source file for encapsulation purposes
// (the interface should not have any knowledge of the handle)
extern SD_HandleTypeDef hsd;

// Defined in source file for encapsulation
#define MEGABYTES_TO_BYTES (1024 * 1024)

// Delayed mount means that the FatFS library does not check if an SD card is mounted until it's needed.
// Forced mount means that the library immediately checks if there is an SD card mounted.
#define DELAYED_MOUNT 0
#define FORCED_MOUNT 1

// TODO: check for NULL pointers

// NOTE: this should be called after the auto-generated STM32 code is run, i.e., in MX_SDIO_SD_Init
fs_ret_t MicroSD_Open(fs_driver_t *fs)
{
    // Due to some internal quirks with STM32's implementation of SDIO,
    // we must initialize the bus width to be 1-bit.
    // After initialization, we can change it back to 4-bits.
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    if (HAL_SD_Init(&hsd) != HAL_OK)
    {
        return FS_ERROR_UNABLE_TO_INIT;
    }

    // Now we can switch to 4-bit bus width
    // TODO: configure 4-bit on hardware
    /* if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
     {
     return FS_ERROR_UNABLE_TO_INIT;
     } */

    // Mount the FatFS file system
    // DELAYED_MOUNT (= 0) is default, so I just went with that
    // See http://elm-chan.org/fsw/ff/doc/mount.html

    if (f_mount(&SDFatFS, (TCHAR const*) SDPath, FORCED_MOUNT) != FR_OK)
    {
        return FS_ERROR_UNABLE_TO_MOUNT;
    }

    fs->block_size_b = hsd.SdCard.BlockSize;
    fs->num_blocks = hsd.SdCard.BlockNbr;
    fs->fs_size_mb = ((double) (fs->block_size_b) / MEGABYTES_TO_BYTES)
            * fs->num_blocks;

    return FS_SUCCESS;
}

fs_ret_t MicroSD_Close(void)
{
    if (hsd.State != HAL_SD_STATE_READY)
    {
        return FS_ERROR_UNINITIALIZED;
    }

    if (HAL_SD_DeInit(&hsd) != HAL_OK)
    {
        return FS_ERROR_UNABLE_TO_CLOSE;
    }

    return FS_SUCCESS;
}

fs_ret_t MicroSD_OpenFile(file_t *file, char *filename)
{
    if (hsd.State != HAL_SD_STATE_READY)
    {
        return FS_ERROR_UNINITIALIZED;
    }

    if (file == NULL)
    {
        return FS_ERROR_UNABLE_TO_OPEN_FILE;
    }

    // Open the file
    // Since we use a pointer in the file struct for the handle,
    // we can't pass that directly to f_open.
    // We need some sort of allocated FIL structure!
    // Since this is a method that returns a pointer, we can't allocate it on the stack
    // since it'll be cleared once this stack frame is over.
    // Also, we want to be able to open multiple files, so we can't use a static variable.
    // So, we use malloc.
    FIL *handle = malloc(sizeof(FIL));

    if (f_open(handle, filename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    {
        free(handle);
        return FS_ERROR_UNABLE_TO_OPEN_FILE;
    }

    // Store handle pointer and filename
    file->handle = handle;
    file->filename = filename;

    return FS_SUCCESS;
}

fs_ret_t MicroSD_CloseFile(file_t *file)
{
    if (file == NULL || file->handle == NULL)
    {
        return FS_ERROR_UNABLE_TO_CLOSE_FILE;
    }

    // Free memory and remove dangling pointer
    free(file->handle);
    file->handle = NULL;

    return FS_SUCCESS;
}

fs_ret_t MicroSD_ReadFile(file_t *file, void *buffer, size_t length)
{
    if (file == NULL || file->handle == NULL)
    {
        return FS_ERROR_UNABLE_TO_READ_FILE;
    }

    if (buffer == NULL)
    {
        return FS_ERROR_UNABLE_TO_READ_FILE;
    }

    /*
     * Read the file
     * Length will equal *bytes_read when the return code is FR_OK
     * See http://elm-chan.org/fsw/ff/doc/read.html
     *
     * Even if buffer is of type, say, uint32_t (4 bytes),
     * f_read will work as intended.
     * f_read uses a void *buf internally (that is cast to a BYTE *buf so they can do pointer arithmetic).
     * But! Our buffer will simply read 4 bytes at a time.
     * We just need to make sure the length is correct (i.e., it's the number of BYTES)
     */

    // TODO: return this? since it can be less than length of buffer (if buffer is too large)
    size_t bytes_read;

    if (f_read((FIL*) file->handle, buffer, length, &bytes_read) != FR_OK)
    {
        return FS_ERROR_UNABLE_TO_READ_FILE;
    }

    return FS_SUCCESS;
}

const struct fs_operations fs_ops =
{ .Open = MicroSD_Open, .Close = MicroSD_Close, .OpenFile = MicroSD_OpenFile,
        .CloseFile = MicroSD_CloseFile, .ReadFile = MicroSD_ReadFile };

fs_driver_t microsd_driver =
{ .ops = &fs_ops };
