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
#define BYTES_TO_MEGABYTES 1024 * 1024
#define DELAYED_MOUNT 0

// NOTE: this should be called after the auto-generated STM32 code is run, i.e., in MX_SDIO_SD_Init
fs_ret_t MicroSD_Init(void)
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
    if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
        return FS_ERROR_UNABLE_TO_INIT;
    }

    // Mount the FatFS file system
    // DELAYED_MOUNT (= 0) is default, so I just went with that
    // See http://elm-chan.org/fsw/ff/doc/mount.html
    if (f_mount(&SDFatFS, (TCHAR const*) SDPath, DELAYED_MOUNT) != FR_OK)
    {
        return FS_ERROR_UNABLE_TO_MOUNT_FATFS;
    }

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

// TODO: consider making void *buffer
fs_ret_t MicroSD_Read(char filename[], uint8_t *buffer, size_t length)
{
    if (hsd.State != HAL_SD_STATE_READY)
    {
        return FS_ERROR_UNINITIALIZED;
    }

    // Open the file
    FIL file;
    if (f_open(&file, filename, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    {
        return FS_ERROR_UNABLE_TO_OPEN_FILE;
    }

    // Read the file
    // Length will equal *bytes_read when the return code is FR_OK
    // See http://elm-chan.org/fsw/ff/doc/read.html
    size_t bytes_read;
    if (f_read(&file, buffer, length, &bytes_read) != FR_OK)
    {
        return FS_ERROR_UNABLE_TO_READ_FILE;
    }

    return FS_SUCCESS;
}

fs_ret_t MicroSD_GetInfo(fs_info_t *info)
{
    if (hsd.State != HAL_SD_STATE_READY)
    {
        return FS_ERROR_UNINITIALIZED;
    }

    fs_info_t info_temp;

    info_temp.block_size_b = hsd.SdCard.BlockSize;
    info_temp.num_blocks = hsd.SdCard.BlockNbr;
    info_temp.fs_size_mb =
            (double) (info_temp.block_size_b / BYTES_TO_MEGABYTES)
                    * info_temp.num_blocks;

    *info = info_temp;

    return FS_SUCCESS;
}

const fs_driver_t microsd_driver =
{ .Init = MicroSD_Init, .Close = MicroSD_Close, .Read = MicroSD_Read, .GetInfo =
        MicroSD_GetInfo, };
