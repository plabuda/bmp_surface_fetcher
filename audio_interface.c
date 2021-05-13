#define MINIAUDIO_IMPLEMENTATION
#include "audio_interface.h"
#include "webaudio.h"
#include <SDL2/SDL.h>
#include <stdio.h>

ma_device device;
ma_device_config deviceConfig;
ma_decoder music_decoder;
ma_decoder crash_decoder;
ma_decoder beep_decoder;

Uint8 audio_is_ready()
{
    return (crash_decoder.pUserData != NULL)    //
           && (music_decoder.pUserData != NULL) //
           && (beep_decoder.pUserData != NULL); //
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
    if (music_decoder.pUserData != NULL)
    {

        // for now just hardcode 16 bit data
        // In future look for a method to determine the format and select an 'adder' based on it
        static ma_int16 tempBuffer[4096];
        ma_int16 *output = (ma_int16 *)pOutput;

        const ma_uint32 max_frames = 4096 / pDevice->playback.channels;

        char play_beep = MA_TRUE;
        char play_crash = MA_TRUE;

        while (frameCount > 0)
        {
            ma_uint32 frames_to_read = frameCount < max_frames ? frameCount : max_frames;

            // Read background music
            ma_uint64 frames_read;

            ma_data_source_read_pcm_frames(&music_decoder, tempBuffer, frames_to_read, &frames_read, MA_TRUE);

            for (int i = 0; i < frames_read * music_decoder.outputChannels; i++)
            {
                output[i] += tempBuffer[i] / 8;
            }

            // Read crash
            ma_data_source_read_pcm_frames(&crash_decoder, tempBuffer, frames_to_read, &frames_read, MA_FALSE);

            for (int i = 0; i < frames_read * music_decoder.outputChannels; i++)
            {
                output[i] += tempBuffer[i];
            }

            // Read beep
            ma_data_source_read_pcm_frames(&beep_decoder, tempBuffer, frames_to_read, &frames_read, MA_FALSE);

            for (int i = 0; i < frames_read * music_decoder.outputChannels; i++)
            {
                output[i] += tempBuffer[i];
            }

            output += frames_to_read;
            frameCount -= frames_to_read;
        }
    }
}

void audio_init(char *buf, unsigned size, ma_decoder *decoder)
{
    ma_decoder_init_memory_wav(buf, size, NULL, decoder);
    decoder->pUserData = audio_start_playback;
}

void audio_init_background_music(char *buf, unsigned size)
{
    audio_init(buf, size, &music_decoder);
}

void audio_init_crash(char *buf, unsigned size)
{
    audio_init(buf, size, &crash_decoder);
}

void audio_init_beep(char *buf, unsigned size)
{
    audio_init(buf, size, &beep_decoder);
}

void audio_play_beep()
{
    if (beep_decoder.pUserData != NULL)
    {
        ma_decoder_seek_to_pcm_frame(&beep_decoder, 0);
    }
}

void audio_play_crash()
{
    if (crash_decoder.pUserData != NULL)
    {
        ma_decoder_seek_to_pcm_frame(&crash_decoder, 0);
    }
}

void audio_start_playback()
{
    if (music_decoder.pUserData != NULL && deviceConfig.pUserData == NULL)
    {
        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = music_decoder.outputFormat;
        deviceConfig.playback.channels = music_decoder.outputChannels;
        deviceConfig.sampleRate = music_decoder.outputSampleRate;
        deviceConfig.dataCallback = data_callback;
        deviceConfig.pUserData = &music_decoder;

        if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS)
        {
            printf("Failed to open playback device.\n");
            return;
        }

        printf("Device Name: %s\n", device.playback.name);
        if (ma_device_start(&device) != MA_SUCCESS)
        {
            printf("Failed to start playback device.\n");
            ma_device_uninit(&device);
            return;
        }
    }
}

void audio_uninit()
{
    ma_device_uninit(&device);
}