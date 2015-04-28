/**
 * @file wavfile.h
 * @author Shinichiro Nakamura
 */

/*
 * ===============================================================
 *  Tiny WAV I/O Module
 *  Version 0.0.1
 * ===============================================================
 * Copyright (c) 2011-2012 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */

#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdio.h>
#include <stdint.h>

#define WAVFILE_AUDIO_FORMAT_PCM            (1)
#define WAVFILE_AUDIO_FORMAT_EXTENSIBLE     (65534)
#define WAVFILE_MAXIMUM_CHANNELS            (32)

#define WAVFILE_INFO_AUDIO_FORMAT(P)        ((P)->audio_format)
#define WAVFILE_INFO_NUM_CHANNELS(P)        ((P)->num_channels)
#define WAVFILE_INFO_SAMPLE_RATE(P)         ((P)->sample_rate)
#define WAVFILE_INFO_BYTE_RATE(P)           ((P)->byte_rate)
#define WAVFILE_INFO_BLOCK_ALIGN(P)         ((P)->block_align)
#define WAVFILE_INFO_BITS_PER_SAMPLE(P)     ((P)->bits_per_sample)

#define WAVFILE_DATA_IS_END_OF_DATA(P)      ((P)->num_channels == 0)
#define WAVFILE_DATA_NUM_CHANNELS(P)        ((P)->num_channels)
#define WAVFILE_DATA_CHANNEL_DATA(P,CH)     ((P)->channel_data[CH])

#define WAVFILE_DEBUG_ENABLED               (0)

typedef struct WAVFILE WAVFILE;

typedef struct {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} wavfile_info_t;

typedef struct {
    uint16_t num_channels;
    double channel_data[WAVFILE_MAXIMUM_CHANNELS];
} wavfile_data_t;

/**
 */
enum WavFileResult {
    WavFileResultOK,
    WavFileResultErrorInvalidFileName,
    WavFileResultErrorMemoryAllocation,
    WavFileResultErrorFileOpen,
    WavFileResultErrorFileWrite,
    WavFileResultErrorBrokenChunkId,
    WavFileResultErrorBrokenChunkSize,
    WavFileResultErrorBrokenChunkData,
    WavFileResultErrorBrokenFormatId,
    WavFileResultErrorInvalidFormatId,
    WavFileResultErrorBrokenAudioFormat,
    WavFileResultErrorInvalidAudioFormat,
    WavFileResultErrorInvalidNumChannels,
    WavFileResultErrorBrokenNumChannels,
    WavFileResultErrorBrokenSampleRate,
    WavFileResultErrorBrokenByteRate,
    WavFileResultErrorInvalidByteRate,
    WavFileResultErrorBrokenBlockAlign,
    WavFileResultErrorBrokenBitsPerSample,
    WavFileResultErrorUnsupportedBitsPerSample,
    WavFileResultErrorAlreadyInfoChecked,
    WavFileResultErrorAlreadyDataChecked,
    WavFileResultErrorNoDataChunk,
    WavFileResultErrorInvalidMode,
    WavFileResultErrorNeedInfoChecked,
    WavFileResultErrorNeedDataChecked,
    WavFileResultErrorInvalidHandler,
};

/**
 */
enum WavFileMode {
    WavFileModeRead,
    WavFileModeWrite,
};

WAVFILE *wavfile_open(const char *filename, WavFileMode mode, WavFileResult *result);
WavFileResult wavfile_read_info(WAVFILE *p, wavfile_info_t *info);
WavFileResult wavfile_read_data(WAVFILE *p, wavfile_data_t *data);
WavFileResult wavfile_write_info(WAVFILE *p, const wavfile_info_t *info);
WavFileResult wavfile_write_data(WAVFILE *p, const wavfile_data_t *data);
WavFileResult wavfile_close(WAVFILE *p);
void wavfile_result_string(const WavFileResult result, char *buf, size_t siz);

#endif

