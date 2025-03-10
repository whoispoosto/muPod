/*
 * fs.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_FS_H_
#define INC_FS_H_

// TODO: is any of this really a "driver" ?

// Modeled off of the idea of using interfaces in C for embedded
// See https://www.reddit.com/r/embedded/comments/1fraemy/writing_interfaces_in_c/
// TODO: Consider pimpl (opaque ptr) pattern in the future

/*
 * Drivers allow hardware peripherals to interact with software.
 *
 * In Linux, in user-space, all drivers can be interacted with via the common file interface.
 * This includes open, read, write, etc.
 * This is for user-driver interaction.
 *
 * In kernel-space, drivers have different interfaces.
 * For example, network drivers vs. serial ports, etc.
 * However, the interfaces tend to be pretty similar with functions like write, read, etc.
 * BUT they often have specialized interfaces for what they are doing.
 * See struct file_operations vs. struct net_device_ops
 * This is for kernel-hardware interaction.
 *
 * For this project, we are essentially at the kernel-hardware level (although we don't have a full-on kernel).
 * So, we have slightly altered interfaces for each type of driver!
 */

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    FS_SUCCESS = 0,
    FS_ERROR_UNABLE_TO_INIT = -1,
    FS_ERROR_UNABLE_TO_CLOSE = -2,
    FS_ERROR_UNINITIALIZED = -3,
    FS_ERROR_UNABLE_TO_MOUNT = -4,
    FS_ERROR_UNABLE_TO_OPEN_FILE = -5,
    FS_ERROR_UNABLE_TO_READ_FILE = -6,
    FS_ERROR_UNABLE_TO_CLOSE_FILE = -7,
    FS_ERROR_GENERIC = -128
} fs_ret_t;

// TODO: see where we can make instances of/ptrs to file_t const :)
typedef struct file file_t;
struct file
{
    void *handle;           // ex: FIL (fatfs)
    char *filename;
};

struct fs_operations;

// For info on fields like block size and number of blocks,
// see https://www.disca.upv.es/aperles/arm_cortex_m3/llibre/st/STM32F439xx_User_Manual/structhal__sd__cardinfotypedef.html

typedef struct
{
    uint32_t block_size_b;  // bytes
    uint32_t num_blocks;
    uint32_t fs_size_mb;    // megabytes
    const struct fs_operations *ops;
} fs_driver_t;

// see https://elixir.bootlin.com/linux/v6.12.6/source/include/linux/fs.h#L2062, file_operations
// and https://stackoverflow.com/questions/32347910/oop-in-c-implicitly-pass-self-as-parameter
// for design inspiration.
// ex: file->f_op->read(file, buffer, size, &offset);   'this' parameter is explicit in C unlike other languages
// ex: typedef struct codec codec_t;   then can use codec_t in definition of the struct functions
struct fs_operations {
    fs_ret_t (*Open)(fs_driver_t *fs);
    fs_ret_t (*Close)(void);
    fs_ret_t (*OpenFile)(file_t *file, char *filename);
    fs_ret_t (*CloseFile)(file_t *file);
    fs_ret_t (*ReadFile)(file_t *file, void *buffer, size_t length);
    // TODO: ReadFileFrom
    // TODO: SeekFile
    // TODO: fs_ret_t (*Write)(const uint8_t *buffer, size_t length);
};

#endif /* INC_FS_H_ */
