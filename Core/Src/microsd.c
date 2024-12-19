/*
 * microsd.c
 *
 *  Created on: Dec 19, 2024
 *      Author: prestonmeek
 */

#include "microsd.h"

static SD_HandleTypeDef * sd_handle;

microsd_ret_t MicroSD_Open(SD_HandleTypeDef * handle) {
	if (HAL_SD_Init(handle) != HAL_OK)
	{
		return MICROSD_ERROR_UNABLE_TO_OPEN;
	}

	sd_handle = handle;

	return MICROSD_SUCCESS;
}
