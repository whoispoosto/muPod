/*
 * microsd.h
 *
 *  Created on: Dec 18, 2024
 *      Author: prestonmeek
 */

#ifndef INC_MICROSD_H_
#define INC_MICROSD_H_

#include "fatfs.h"
#include "fs.h"

// SD cards can run in two different modes: SPI or SDIO
// https://stm32world.com/wiki/STM32_SD_card_with_FatFs

// Great tutorial for using SPI (note: author kiwih is Hammond Pearce, a well-regarded figure)
// https://01001000.xyz/2020-08-09-Tutorial-STM32CubeIDE-SD-card/

// For this project, I went with SDIO interface because this board supports it
// Additionally, it's faster and more compatible than SPI
// https://www.youtube.com/watch?v=S78q6SuwnrM

// https://github.com/afiskon/stm32-fatfs-examples/blob/master/read-write-cubemx-ver/Src/main.c

// TODO: see if internal pullups need to be enabled for SDIO GPIO
// This depends on the breakout board I am using -- if it already has pullup resistors then we're good
// see video @ 5 mins

// TODO: override weak pin detect function? see video @ 14:20

// We are using FatFS, specifically exFAT (versus something like FAT32)

//////

// File system methods
fs_ret_t MicroSD_Open(fs_driver_t *fs);
fs_ret_t MicroSD_Close(void);
fs_ret_t MicroSD_OpenFile(file_t *file, char *filename);
fs_ret_t MicroSD_CloseFile(file_t *file);
fs_ret_t MicroSD_ReadFile(file_t *file, void *buffer, size_t length);

// File methods
fs_ret_t MicroSD_File_Read(file_t *file, void *buffer, size_t length);

extern fs_driver_t microsd_driver;

#endif /* INC_MICROSD_H_ */
