/*
 * microsd.h
 *
 *  Created on: Dec 18, 2024
 *      Author: prestonmeek
 */

#ifndef INC_MICROSD_H_
#define INC_MICROSD_H_

#include <stdint.h>
#include <stddef.h>

#include "fatfs.h"

// SD cards can run in two different modes: SPI or SDIO
// https://stm32world.com/wiki/STM32_SD_card_with_FatFs

// Great tutorial for using SPI (note: author kiwih is Hammond Pearce, a well-regarded figure)
// https://01001000.xyz/2020-08-09-Tutorial-STM32CubeIDE-SD-card/

// For this project, I went with SDIO interface because this board supports it
// Additionally, it's faster and more compatible than SPI
// https://www.youtube.com/watch?v=S78q6SuwnrM

// TODO: see if internal pullups need to be enabled for SDIO GPIO
// This depends on the breakout board I am using -- if it already has pullup resistors then we're good
// see video @ 5 mins


// We are using FatFS, specifically exFAT (versus something like FAT32)

// TODO: purchase https://www.adafruit.com/product/4682

//////

// TODO: add more verbose errors
typedef enum
{
    MICROSD_SUCCESS = 0,
    MICROSD_ERROR_UNABLE_TO_OPEN = -1,
    MICROSD_ERROR = -15
} microsd_ret_t;

microsd_ret_t MicroSD_Open(SD_HandleTypeDef* handle);
microsd_ret_t MicroSD_Close();
microsd_ret_t MicroSD_Read(uint8_t *buffer, size_t length, uint32_t address);
microsd_ret_t MicroSD_Write(const uint8_t *buffer, size_t length, uint32_t address);
// TODO: ioctl?

#endif /* INC_MICROSD_H_ */
