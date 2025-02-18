/*
 * audio.h
 *
 *  Created on: Dec 22, 2024
 *      Author: prestonmeek
 */

#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_

#include <stdint.h>
#include <stddef.h>

/*
 * Workflow of audio streaming:
 *
 * 1) Get a buffer of audio data
 * WAV data is raw PCM (pulse-code modulation): analog data encoded in a digital format
 * MP3 data is compressed, so we must pass it through a codec (coder decoder) before working with it
 * PCM is sort of a higher-fidelity (more accurate) version of PWM
 *
 * 2) Send the audio to some external hardware
 * This can be done through a DAC directly, then some other device receives an analog signal
 * Or, the audio is transmitted via I2S (inter-IC sound) and the other device receives the *digital* audio signal
 * (thus, the other device must have its own DAC of sorts)
 * Either a timer or DMA can be used to continuously stream data from buffer to DAC (e.g., 44.1 kHz)
 *
 * STM32F410RE does *NOT* have an on-board DAC, so I2S it is!
 * I2S sends PCM (so not much needs to be done for WAV files)
 * Or we can send the digital audio signal to an IC that can decode + amplify it for us
 * Will likely need to connect the DAC to an amplifier, such as an LM386
 */

typedef enum
{
    AUDIO_SUCCESS = 0,
    AUDIO_ERROR_UNABLE_TO_STREAM_BUFFER = -1,
    AUDIO_ERROR_NULL_BUFFER = -2,
    AUDIO_ERROR_GENERIC = -128
} audio_ret_t;

/*
 * It's bad practice to have a function pointer with, say, a void *buffer,
 * and then an implementation function pointer that defines it as a uint16_t *buffer.
 *
 * FUNCTION POINTER SIGNATURE SHOULD MATCH EXACTLY.
 *
 * So, we make the template buffer void
 * and implementations use specific sizes (like uint16_t) via CASTING WITHIN THE FUNCTION BODY.
 * The function signatures will still match.
 */

typedef struct
{
    audio_ret_t (*Open)(void);
    audio_ret_t (*Close)(void);
    audio_ret_t (*Stream)(const void *buffer, size_t length);
} audio_driver_t;

#endif /* INC_AUDIO_H_ */
