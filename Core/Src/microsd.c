/*
 * microsd.c
 *
 *  Created on: Dec 19, 2024
 *      Author: prestonmeek
 */

#include "microsd.h"

static SD_HandleTypeDef *sd_handle = NULL;

microsd_ret_t MicroSD_Init(SD_HandleTypeDef *handle)
{
    // Due to some internal quirks with STM32's implementation of SDIO,
    // we must initialize the bus width to be 1-bit.
    // After initialization, we can change it back to 4-bits.
    handle->Init.BusWide = SDIO_BUS_WIDE_1B;
    if (HAL_SD_Init(handle) != HAL_OK)
    {
        return MICROSD_ERROR_UNABLE_TO_INIT;
    }

    // Now we can switch to 4-bit bus width
    if (HAL_SD_ConfigWideBusOperation(handle, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
        return MICROSD_ERROR_UNABLE_TO_INIT;
    }

    // Store the handle to the internal static variable so that it can be used in other methods
    sd_handle = handle;

    return MICROSD_SUCCESS;
}

microsd_ret_t MicroSD_Close()
{
    if (sd_handle == NULL)
    {
        return MICROSD_ERROR_HANDLE_IS_NULL;
    }

    if (HAL_SD_DeInit(sd_handle) != HAL_OK)
    {
        return MICROSD_ERROR_UNABLE_TO_CLOSE;
    }

    return MICROSD_SUCCESS;
}

microsd_ret_t MicroSD_GetInfo(microsd_info_t *info)
{
    if (sd_handle == NULL)
    {
        return MICROSD_ERROR_HANDLE_IS_NULL;
    }

    microsd_info_t info_temp;

    info_temp.block_size_b = sd_handle->SdCard.BlockSize;
    info_temp.num_blocks = sd_handle->SdCard.BlockNbr;
    info_temp.card_size_mb = (double)(info_temp.block_size_b / BYTES_TO_MEGABYTES) * info_temp.num_blocks;

    *info = info_temp;

    return MICROSD_SUCCESS;
}
