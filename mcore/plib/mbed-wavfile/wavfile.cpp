/**
 * @file wavfile.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavfile.h"

#define DEBUG printf

#define CHUNK_ID_RIFF           (('R' << 24) | ('I' << 16) | ('F' << 8) | ('F' << 0))
#define CHUNK_ID_FMT            (('f' << 24) | ('m' << 16) | ('t' << 8) | (' ' << 0))
#define CHUNK_ID_DATA           (('d' << 24) | ('a' << 16) | ('t' << 8) | ('a' << 0))

#define RIFF_CHUNK_FORMAT_WAVE  (('W' << 24) | ('A' << 16) | ('V' << 8) | ('E' << 0))

#define BITS_PER_SAMPLE_8           (8)
#define BITS_PER_SAMPLE_16          (16)
#define BITS_PER_SAMPLE_24          (24)

#define CHUNK_SIZE_FMT_PCM          (16)
#define CHUNK_SIZE_FMT_EXTENSIBLE   (40)

#define CHANNEL_MASK_SPEAKER_FRONT_LEFT             (0x00000001)
#define CHANNEL_MASK_SPEAKER_FRONT_RIGHT            (0x00000002)
#define CHANNEL_MASK_SPEAKER_FRONT_CENTER           (0x00000004)
#define CHANNEL_MASK_SPEAKER_LOW_FREQUENCY          (0x00000008)
#define CHANNEL_MASK_SPEAKER_BACK_LEFT              (0x00000010)
#define CHANNEL_MASK_SPEAKER_BACK_RIGHT             (0x00000020)
#define CHANNEL_MASK_SPEAKER_FRONT_LEFT_OF_CENTER   (0x00000040)
#define CHANNEL_MASK_SPEAKER_FRONT_RIGHT_OF_CENTER  (0x00000080)
#define CHANNEL_MASK_SPEAKER_BACK_CENTER            (0x00000100)
#define CHANNEL_MASK_SPEAKER_SIDE_LEFT              (0x00000200)
#define CHANNEL_MASK_SPEAKER_SIDE_RIGHT             (0x00000400)
#define CHANNEL_MASK_SPEAKER_TOP_CENTER             (0x00000800)
#define CHANNEL_MASK_SPEAKER_TOP_FRONT_LEFT         (0x00001000)
#define CHANNEL_MASK_SPEAKER_TOP_FRONT_CENTER       (0x00002000)
#define CHANNEL_MASK_SPEAKER_TOP_FRONT_RIGHT        (0x00004000)
#define CHANNEL_MASK_SPEAKER_TOP_BACK_LEFT          (0x00008000)
#define CHANNEL_MASK_SPEAKER_TOP_BACK_CENTER        (0x00010000)
#define CHANNEL_MASK_SPEAKER_TOP_BACK_RIGHT         (0x00020000)
#define CHANNEL_MASK_SPEAKER_RESERVED               (0x80000000)

struct WAVFILE {
    FILE *fp;
    char filename[BUFSIZ];
    WavFileMode mode;
    bool info_checked;
    bool data_checked;
    uint32_t data_byte_count;
    wavfile_info_t info;
};

static int WRITE_U32_BE(FILE *fp, const uint32_t value) {
    for (int i = 0; i < 4; i++) {
        if (fputc((value >> (8 * (3 - i))), fp) == EOF) {
            return -1;
        }
    }
    return 0;
}

static int WRITE_U32_LE(FILE *fp, const uint32_t value) {
    for (int i = 0; i < 4; i++) {
        if (fputc((value >> (8 * i)), fp) == EOF) {
            return -1;
        }
    }
    return 0;
}

static int WRITE_U16_LE(FILE *fp, const uint16_t value) {
    for (int i = 0; i < 2; i++) {
        if (fputc((value >> (8 * i)), fp) == EOF) {
            return -1;
        }
    }
    return 0;
}

static int READ_U32_BE(FILE *fp, uint32_t *value) {
    int raw[4];
    for (int i = 0; i < (int)(sizeof(raw) / sizeof(raw[0])); i++) {
        raw[i] = fgetc(fp);
        if (raw[i] == EOF) {
            *value = 0x00000000;
            return -1;
        }
    }
    *value =
        ((uint32_t)raw[0] << 24) |
        ((uint32_t)raw[1] << 16) |
        ((uint32_t)raw[2] <<  8) |
        ((uint32_t)raw[3] <<  0);
    return 0;
}

static int READ_U32_LE(FILE *fp, uint32_t *value) {
    int raw[4];
    for (int i = 0; i < (int)(sizeof(raw) / sizeof(raw[0])); i++) {
        raw[i] = fgetc(fp);
        if (raw[i] == EOF) {
            *value = 0x00000000;
            return -1;
        }
    }
    *value =
        ((uint32_t)raw[3] << 24) |
        ((uint32_t)raw[2] << 16) |
        ((uint32_t)raw[1] <<  8) |
        ((uint32_t)raw[0] <<  0);
    return 0;
}

static int READ_U16_LE(FILE *fp, uint16_t *value) {
    int raw[2];
    for (int i = 0; i < (int)(sizeof(raw) / sizeof(raw[0])); i++) {
        raw[i] = fgetc(fp);
        if (raw[i] == EOF) {
            *value = 0x00000000;
            return -1;
        }
    }
    *value =
        ((uint16_t)raw[1] <<  8) |
        ((uint16_t)raw[0] <<  0);
    return 0;
}

static WavFileResult chunk_reader_unknown(
    const uint32_t chunk_id,
    const uint32_t chunk_size,
    FILE *fp) {
    for (int i = 0; i < (int)chunk_size; i++) {
        int c = fgetc(fp);
        if (c == EOF) {
            return WavFileResultErrorBrokenChunkData;
        }
    }
    return WavFileResultOK;
}

static WavFileResult chunk_reader_riff(
    const uint32_t chunk_id,
    const uint32_t chunk_size,
    FILE *fp,
    uint32_t *format_id) {
    if (READ_U32_BE(fp, format_id) != 0) {
        return WavFileResultErrorBrokenFormatId;
    }
    return WavFileResultOK;
}

static WavFileResult chunk_reader_fmt(
    const uint32_t chunk_id,
    const uint32_t chunk_size,
    FILE *fp,
    uint16_t *audio_format,
    uint16_t *num_channels,
    uint32_t *sample_rate,
    uint32_t *byte_rate,
    uint16_t *block_align,
    uint16_t *bits_per_sample) {
    uint32_t read_byte_count = 0;

    /*
     * 2
     */
    if (read_byte_count < chunk_size) {
        if (READ_U16_LE(fp, audio_format) != 0) {
            return WavFileResultErrorBrokenAudioFormat;
        }
    }
    read_byte_count+=2;

    /*
     * 2 + 2
     */
    if (read_byte_count < chunk_size) {
        if (READ_U16_LE(fp, num_channels) != 0) {
            return WavFileResultErrorBrokenNumChannels;
        }
    }
    read_byte_count+=2;

    /*
     * 2 + 2 + 4
     */
    if (read_byte_count < chunk_size) {
        if (READ_U32_LE(fp, sample_rate) != 0) {
            return WavFileResultErrorBrokenSampleRate;
        }
    }
    read_byte_count+=4;

    /*
     * 2 + 2 + 4 + 4
     */
    if (read_byte_count < chunk_size) {
        if (READ_U32_LE(fp, byte_rate) != 0) {
            return WavFileResultErrorBrokenByteRate;
        }
    }
    read_byte_count+=4;

    /*
     * 2 + 2 + 4 + 4 + 2
     */
    if (read_byte_count < chunk_size) {
        if (READ_U16_LE(fp, block_align) != 0) {
            return WavFileResultErrorBrokenBlockAlign;
        }
    }
    read_byte_count+=2;

    /*
     * 2 + 2 + 4 + 4 + 2 + 2
     */
    if (read_byte_count < chunk_size) {
        if (READ_U16_LE(fp, bits_per_sample) != 0) {
            return WavFileResultErrorBrokenBitsPerSample;
        }
    }
    read_byte_count+=2;

    /*
     * 2 + 2 + 4 + 4 + 2 + 2
     */
    while (read_byte_count < chunk_size) {
        if (fgetc(fp) == EOF) {
            return WavFileResultErrorBrokenChunkData;
        }
        read_byte_count++;
    }

    return WavFileResultOK;
}

WAVFILE *wavfile_open(const char *filename, WavFileMode mode, WavFileResult *result) {
    /*
     * Verify the filename.
     */
    if (filename == NULL) {
        *result = WavFileResultErrorInvalidFileName;
        return NULL;
    }

    /*
     * Open the file.
     */
    FILE *fp = NULL;
    switch (mode) {
        case WavFileModeRead:
            fp = fopen(filename, "rb");
            break;
        case WavFileModeWrite:
            fp = fopen(filename, "wb");
            break;
        default:
            fp = NULL;
            break;
    }
    if (fp == NULL) {
        *result = WavFileResultErrorFileOpen;
        return NULL;
    }

    /*
     * Allocate the handler.
     */
    WAVFILE *p = (WAVFILE *)malloc(sizeof(WAVFILE));
    if (p == NULL) {
        *result = WavFileResultErrorMemoryAllocation;
        return NULL;
    }

    /*
     * Fill the fields.
     */
    p->fp = fp;
    strcpy(p->filename, filename);
    p->mode = mode;
    p->info_checked = false;
    p->data_checked = false;
    p->data_byte_count = 0;
    WAVFILE_INFO_AUDIO_FORMAT(&(p->info)) = 0;
    WAVFILE_INFO_NUM_CHANNELS(&(p->info)) = 0;
    WAVFILE_INFO_SAMPLE_RATE(&(p->info)) = 0;
    WAVFILE_INFO_BYTE_RATE(&(p->info)) = 0;
    WAVFILE_INFO_BLOCK_ALIGN(&(p->info)) = 0;
    WAVFILE_INFO_BITS_PER_SAMPLE(&(p->info)) = 0;

    *result = WavFileResultOK;
    return p;
}

WavFileResult wavfile_read_info(WAVFILE *p, wavfile_info_t *info) {
    WavFileResult result = WavFileResultOK;

    if (p == NULL) {
        result = WavFileResultErrorInvalidHandler;
        goto finalize;
    }

    if (p->info_checked) {
        result = WavFileResultErrorAlreadyInfoChecked;
        goto finalize;
    }
    if (p->data_checked) {
        result = WavFileResultErrorAlreadyDataChecked;
        goto finalize;
    }
    if (p->mode != WavFileModeRead) {
        result = WavFileResultErrorInvalidMode;
        goto finalize;
    }

    while (1) {
        uint32_t chunk_id;
        uint32_t chunk_size;

        /*
         * Get the chunk ID.
         */
        if (READ_U32_BE(p->fp, &chunk_id) != 0) {
            if (feof(p->fp)) {
                /*
                 * 
                 */
                result = WavFileResultErrorNoDataChunk;
                goto finalize;
            } else {
                result = WavFileResultErrorBrokenChunkId;
                goto finalize;
            }
        }

        /*
         * Verify the chunk size.
         */
        if (READ_U32_LE(p->fp, &chunk_size) != 0) {
            result = WavFileResultErrorBrokenChunkSize;
            goto finalize;
        }

#if WAVFILE_DEBUG_ENABLED
        /*
         * 
         */
        DEBUG("chunk_id(0x%04X-%c%c%c%c), chunk_size(%d bytes)\n",
              chunk_id,
              (chunk_id >> (8 * 3)),
              (chunk_id >> (8 * 2)),
              (chunk_id >> (8 * 1)),
              (chunk_id >> (8 * 0)),
              chunk_size);
#endif

        /*
         */
        switch (chunk_id) {
            case CHUNK_ID_RIFF: {
                uint32_t format_id;
                result = chunk_reader_riff(
                             chunk_id,
                             chunk_size,
                             p->fp,
                             &format_id);

#if WAVFILE_DEBUG_ENABLED
                /*
                 */
                DEBUG("\tformat_id(%d)\n", format_id);
#endif

                if (format_id != RIFF_CHUNK_FORMAT_WAVE) {
                    return WavFileResultErrorInvalidFormatId;
                }
                if (result != WavFileResultOK) {
                    goto finalize;
                }
            }
            break;
            case CHUNK_ID_FMT: {
                result = chunk_reader_fmt(
                             chunk_id,
                             chunk_size,
                             p->fp,
                             &(p->info.audio_format),
                             &(p->info.num_channels),
                             &(p->info.sample_rate),
                             &(p->info.byte_rate),
                             &(p->info.block_align),
                             &(p->info.bits_per_sample));

                info->audio_format = p->info.audio_format;
                info->num_channels = p->info.num_channels;
                info->sample_rate = p->info.sample_rate;
                info->byte_rate = p->info.byte_rate;
                info->block_align = p->info.block_align;
                info->bits_per_sample = p->info.bits_per_sample;

#if WAVFILE_DEBUG_ENABLED
                /*
                 */
                DEBUG("\taudio_format(%d)\n", p->info.audio_format);
                DEBUG("\tnum_channels(%d)\n", p->info.num_channels);
                DEBUG("\tsample_rate(%d)\n", p->info.sample_rate);
                DEBUG("\tbyte_rate(%d)\n", p->info.byte_rate);
                DEBUG("\tblock_align(%d)\n", p->info.block_align);
                DEBUG("\tbits_per_sample(%d)\n", p->info.bits_per_sample);
#endif

                if ((p->info.audio_format != WAVFILE_AUDIO_FORMAT_PCM) && (info->audio_format != WAVFILE_AUDIO_FORMAT_EXTENSIBLE)) {
                    return WavFileResultErrorInvalidAudioFormat;
                }
                if (result != WavFileResultOK) {
                    goto finalize;
                }
            }
            break;
            case CHUNK_ID_DATA: {
                p->info_checked = true;
                p->data_byte_count = chunk_size;
                goto finalize;
            }
            break;
            default: {
                result = chunk_reader_unknown(chunk_id, chunk_size, p->fp);
                if (result != WavFileResultOK) {
                    goto finalize;
                }
            }
            break;
        }
    }

finalize:
    return result;
}

/**
 */
WavFileResult wavfile_read_data(WAVFILE *p, wavfile_data_t *data) {
    if (p == NULL) {
        return WavFileResultErrorInvalidHandler;
    }

    if (!p->info_checked) {
        return WavFileResultErrorNeedInfoChecked;
    }

    if (p->mode != WavFileModeRead) {
        return WavFileResultErrorInvalidMode;
    }

    if (p->data_byte_count == 0) {
        data->num_channels = 0;
        for (int i = 0; i < p->info.num_channels; i++) {
            data->channel_data[i] = 0.5;
        }
        return WavFileResultOK;
    }

    data->num_channels = p->info.num_channels;
    for (int i = 0; i < p->info.num_channels; i++) {
        switch (p->info.bits_per_sample) {
            case BITS_PER_SAMPLE_8: {
                int c = fgetc(p->fp);
                if (c == EOF) {
                    return WavFileResultErrorBrokenChunkData;
                }
                data->channel_data[i] = (double)c / 0xFF;
            }
            p->data_byte_count-=1;
            break;
            case BITS_PER_SAMPLE_16: {
#if 0
                int c1 = fgetc(p->fp);
                if (c1 == EOF) {
                    return WavFileResultErrorBrokenChunkData;
                }
                int c2 = fgetc(p->fp);
                if (c2 == EOF) {
                    return WavFileResultErrorBrokenChunkData;
                }
                uint16_t n = (((uint16_t)c2 << 8) | ((uint16_t)c1 << 0)) ^ (1 << 15);
                data->channel_data[i] = (double)n / 0xFFFF;
#else
                char tmp[2];
                fread(tmp, 2, 1, p->fp);
                uint16_t n = (((uint16_t)tmp[1] << 8) | ((uint16_t)tmp[0] << 0)) ^ (1 << 15);
                data->channel_data[i] = (double)n / 0xFFFF;
#endif
            }
            p->data_byte_count-=2;
            break;
            case BITS_PER_SAMPLE_24: {
            #if 0
                int c1 = fgetc(p->fp);
                if (c1 == EOF) {
                    return WavFileResultErrorBrokenChunkData;
                }
                int c2 = fgetc(p->fp);
                if (c2 == EOF) {
                    return WavFileResultErrorBrokenChunkData;
                }
                int c3 = fgetc(p->fp);
                if (c3 == EOF) {
                    return WavFileResultErrorBrokenChunkData;
                }
                uint32_t n = (((uint32_t)c3 << 16) | ((uint32_t)c2 << 8) | ((uint32_t)c1 << 0)) ^ (1 << 23);
                data->channel_data[i] = (double)n / 0xFFFFFF;
#else
                char tmp[3];
                fread(tmp, 3, 1, p->fp);
                uint32_t n = (((uint32_t)tmp[2] << 16) | ((uint32_t)tmp[1] << 8) | ((uint32_t)tmp[0] << 0)) ^ (1 << 23);
                data->channel_data[i] = (double)n / 0xFFFFFF;
#endif
            }
            p->data_byte_count-=3;
            break;
            default:
                return WavFileResultErrorUnsupportedBitsPerSample;
        }
    }
    return WavFileResultOK;
}

WavFileResult wavfile_write_info(WAVFILE *p, const wavfile_info_t *info) {
    WavFileResult result = WavFileResultOK;

    if (p == NULL) {
        result = WavFileResultErrorInvalidHandler;
        goto finalize;
    }

    if (p->info_checked) {
        result = WavFileResultErrorAlreadyInfoChecked;
        goto finalize;
    }

    if (p->mode != WavFileModeWrite) {
        result = WavFileResultErrorInvalidMode;
        goto finalize;
    }

    p->info.audio_format = info->audio_format;
    p->info.num_channels = info->num_channels;
    p->info.sample_rate = info->sample_rate;
    p->info.byte_rate = info->byte_rate;
    p->info.block_align = info->block_align;
    p->info.bits_per_sample = info->bits_per_sample;

    /*
     *
     */

    if ((info->audio_format != WAVFILE_AUDIO_FORMAT_PCM) && (info->audio_format != WAVFILE_AUDIO_FORMAT_EXTENSIBLE)) {
        result = WavFileResultErrorInvalidAudioFormat;
        goto finalize;
    }

    if ((info->bits_per_sample != BITS_PER_SAMPLE_8)
            && (info->bits_per_sample != BITS_PER_SAMPLE_16)
            && (info->bits_per_sample != BITS_PER_SAMPLE_24)) {
        result = WavFileResultErrorUnsupportedBitsPerSample;
        goto finalize;
    }

    if ((info->num_channels * info->sample_rate * (info->bits_per_sample / 8)) != info->byte_rate) {
        result = WavFileResultErrorInvalidByteRate;
        goto finalize;
    }

    /*
     * [RIFF]
     * ------------------------------------------
     * Big endian    4 bytes : Chunk ID
     * Little endian 4 bytes : Chunk size
     * Big endian    4 bytes : Format
     * ------------------------------------------
     */
    if (WRITE_U32_BE(p->fp, CHUNK_ID_RIFF) != 0) {
        result = WavFileResultErrorFileWrite;
        goto finalize;
    }
    if (WRITE_U32_LE(p->fp, 0x00000000) != 0) {
        result = WavFileResultErrorFileWrite;
        goto finalize;
    }
    if (WRITE_U32_BE(p->fp, RIFF_CHUNK_FORMAT_WAVE) != 0) {
        result = WavFileResultErrorFileWrite;
        goto finalize;
    }

    /*
     * [fmt]
     * ------------------------------------------
     * Big endian    4 bytes : Sub chunk ID
     * Little endian 4 bytes : Sub chunk size
     * Little endian 2 bytes : Audio format
     * Little endian 2 bytes : Number of channels
     * Little endian 4 bytes : Sample rate
     * Little endian 4 bytes : Byte rate
     * Little endian 2 bytes : Block align
     * Little endian 2 bytes : Bits per sample
     *  .                                      .
     *  .  Additional bytes here (extensible)  .
     *  .                                      .
     * ------------------------------------------
     */
    switch (info->audio_format) {
        case WAVFILE_AUDIO_FORMAT_PCM: {
            if (WRITE_U32_BE(p->fp, CHUNK_ID_FMT) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, CHUNK_SIZE_FMT_PCM) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->audio_format) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->num_channels) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, info->sample_rate) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, info->byte_rate) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->block_align) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->bits_per_sample) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
        }
        break;
        case WAVFILE_AUDIO_FORMAT_EXTENSIBLE: {
            if (WRITE_U32_BE(p->fp, CHUNK_ID_FMT) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, CHUNK_SIZE_FMT_EXTENSIBLE) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->audio_format) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->num_channels) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, info->sample_rate) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, info->byte_rate) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->block_align) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->bits_per_sample) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            /*
             * Additional bytes for the extensible format.
             *
             *  2 bytes : Size of the extension (0 or 22)
             *  2 bytes : Number of valid bits
             *  4 bytes : Speaker position mask
             * 16 bytes : GUID, including the data format code
             */
            if (WRITE_U16_LE(p->fp, 22) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U16_LE(p->fp, info->bits_per_sample) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            if (WRITE_U32_LE(p->fp, 0x00000000) != 0) {
                result = WavFileResultErrorFileWrite;
                goto finalize;
            }
            static const unsigned char sub_format[16] = {
                0x01, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x10, 0x00,
                0x80, 0x00, 0x00, 0xAA,
                0x00, 0x38, 0x9B, 0x71
            };
            for (int i = 0; i < sizeof(sub_format); i++) {
                fputc((char)sub_format[i], p->fp);
            }
        }
        break;
        default:
            result = WavFileResultErrorInvalidAudioFormat;
            goto finalize;
    }

    /*
     * [data]
     * ------------------------------------------
     * Big endian    4 bytes : Sub chunk ID
     * Little endian 4 bytes : Sub chunk size
     * ------------------------------------------
     * Little endian 2 bytes : Sample 1 (Ch.1)
     * Little endian 2 bytes : Sample 1 (Ch.2)
     *                    .
     *                    .
     *                    .
     * Little endian 2 bytes : Sample 1 (Ch.N)
     * ------------------------------------------
     * Little endian 2 bytes : Sample 2 (Ch.1)
     * Little endian 2 bytes : Sample 2 (Ch.2)
     *                    .
     *                    .
     *                    .
     * Little endian 2 bytes : Sample 2 (Ch.N)
     * ------------------------------------------
     */
    if (WRITE_U32_BE(p->fp, CHUNK_ID_DATA) != 0) {
        result = WavFileResultErrorFileWrite;
        goto finalize;
    }
    if (WRITE_U32_LE(p->fp, 0x00000000) != 0) {
        result = WavFileResultErrorFileWrite;
        goto finalize;
    }

finalize:
    if (WavFileResultOK == result) {
        p->info_checked = true;
    }
    return result;
}

/**
 */
WavFileResult wavfile_write_data(WAVFILE *p, const wavfile_data_t *data) {
    WavFileResult result = WavFileResultOK;

    if (p == NULL) {
        result = WavFileResultErrorInvalidHandler;
        goto finalize;
    }

    if (!p->info_checked) {
        result = WavFileResultErrorNeedInfoChecked;
        goto finalize;
    }

    if (p->mode != WavFileModeWrite) {
        result = WavFileResultErrorInvalidMode;
        goto finalize;
    }

    if (p->info.num_channels != data->num_channels) {
        result = WavFileResultErrorInvalidNumChannels;
        goto finalize;
    }

    for (int i = 0; i < p->info.num_channels; i++) {
        switch (p->info.bits_per_sample) {
            case BITS_PER_SAMPLE_8: {
                int n = (int)((double)data->channel_data[i] * 0xFF);
                if (n < 0x00) {
                    n = 0x00;
                }
                if (0xFF < n) {
                    n = 0xFF;
                }
                fputc((char)n, p->fp);
            }
            p->data_byte_count+=1;
            break;
            case BITS_PER_SAMPLE_16: {
                int n = (int)((double)(data->channel_data[i] * 0xFFFF) - 0x8000);
                if (0x7FFF < n) {
                    n = 0x7FFF;
                }
                if (n < -0x8000) {
                    n = -0x8000;
                }
                fputc(((uint16_t)n >> 0) & 0xff, p->fp);
                fputc(((uint16_t)n >> 8) & 0xff, p->fp);
            }
            p->data_byte_count+=2;
            break;
            case BITS_PER_SAMPLE_24: {
                int n = (int)((double)(data->channel_data[i] * 0xFFFFFF) - 0x800000);
                if (0x7FFFFF < n) {
                    n = 0x7FFFFF;
                }
                if (n < -0x800000) {
                    n = -0x800000;
                }
                fputc(((uint32_t)n >>  0) & 0xff, p->fp);
                fputc(((uint32_t)n >>  8) & 0xff, p->fp);
                fputc(((uint32_t)n >> 16) & 0xff, p->fp);
            }
            p->data_byte_count+=3;
            break;
        }
    }
    p->data_checked = true;

finalize:
    return result;
}

WavFileResult wavfile_close(WAVFILE *p) {
    WavFileResult result = WavFileResultOK;

    switch (p->mode) {
        case WavFileModeRead:
            break;
        case WavFileModeWrite:
            if (p->info_checked && p->data_checked) {
                switch (p->info.audio_format) {
                    case WAVFILE_AUDIO_FORMAT_PCM: {
                        /*
                         * Fill the RIFF chunk size.
                         */
                        if (fseek(p->fp, 4L, SEEK_SET) == -1) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }
                        if (WRITE_U32_LE(p->fp, 4 + (8 + CHUNK_SIZE_FMT_PCM) + (8 + p->data_byte_count)) != 0) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }

                        /*
                         * Fill the data sub chunk size.
                         */
                        if (fseek(p->fp, 12 + (8 + CHUNK_SIZE_FMT_PCM) + 4, SEEK_SET) == -1) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }
                        if (WRITE_U32_LE(p->fp, p->data_byte_count) != 0) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }
                    }
                    break;
                    case WAVFILE_AUDIO_FORMAT_EXTENSIBLE: {
                        /*
                         * Fill the RIFF chunk size.
                         */
                        if (fseek(p->fp, 4L, SEEK_SET) == -1) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }
                        if (WRITE_U32_LE(p->fp, 4 + (8 + CHUNK_SIZE_FMT_EXTENSIBLE) + (8 + p->data_byte_count)) != 0) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }

                        /*
                         * Fill the data sub chunk size.
                         */
                        if (fseek(p->fp, 12 + (8 + CHUNK_SIZE_FMT_EXTENSIBLE) + 4, SEEK_SET) == -1) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }
                        if (WRITE_U32_LE(p->fp, p->data_byte_count) != 0) {
                            result = WavFileResultErrorFileWrite;
                            goto finalize;
                        }
                    }
                    break;
                }
            }
            break;
    }

finalize:
    fclose(p->fp);
    free(p);
    return result;
}

void wavfile_result_string(const WavFileResult result, char *buf, size_t siz) {
    switch (result) {
        case WavFileResultOK:
            strcpy(buf, "OK.");
            break;
        case WavFileResultErrorInvalidFileName:
            strcpy(buf, "Invalid file name found.");
            break;
        case WavFileResultErrorMemoryAllocation:
            strcpy(buf, "Memory allocation error.");
            break;
        case WavFileResultErrorFileOpen:
            strcpy(buf, "File open error found.");
            break;
        case WavFileResultErrorFileWrite:
            strcpy(buf, "File write error found.");
            break;
        case WavFileResultErrorBrokenChunkId:
            strcpy(buf, "Broken chunk ID found.");
            break;
        case WavFileResultErrorBrokenChunkSize:
            strcpy(buf, "Borken chunk size found.");
            break;
        case WavFileResultErrorBrokenChunkData:
            strcpy(buf, "Borken chunk data found.");
            break;
        case WavFileResultErrorBrokenFormatId:
            strcpy(buf, "Broken format ID found.");
            break;
        case WavFileResultErrorInvalidFormatId:
            strcpy(buf, "Invalid format ID found.");
            break;
        case WavFileResultErrorBrokenAudioFormat:
            strcpy(buf, "Broken audio format found.");
            break;
        case WavFileResultErrorInvalidAudioFormat:
            strcpy(buf, "Invalid audio format found.");
            break;
        case WavFileResultErrorInvalidNumChannels:
            strcpy(buf, "Invalid number of channels found.");
            break;
        case WavFileResultErrorBrokenNumChannels:
            strcpy(buf, "Broken number of channels found.");
            break;
        case WavFileResultErrorBrokenSampleRate:
            strcpy(buf, "Broken sample rate found.");
            break;
        case WavFileResultErrorBrokenByteRate:
            strcpy(buf, "Broken byte rate found.");
            break;
        case WavFileResultErrorInvalidByteRate:
            strcpy(buf, "Invalid byte rate found.");
            break;
        case WavFileResultErrorBrokenBlockAlign:
            strcpy(buf, "Broken block alignment found.");
            break;
        case WavFileResultErrorBrokenBitsPerSample:
            strcpy(buf, "Broken bits per sample found.");
            break;
        case WavFileResultErrorUnsupportedBitsPerSample:
            strcpy(buf, "Unsupported bits per sample found.");
            break;
        case WavFileResultErrorAlreadyInfoChecked:
            strcpy(buf, "Already checked info.");
            break;
        case WavFileResultErrorAlreadyDataChecked:
            strcpy(buf, "Already checked data.");
            break;
        case WavFileResultErrorNoDataChunk:
            strcpy(buf, "No data chunk.");
            break;
        case WavFileResultErrorInvalidMode:
            strcpy(buf, "Invalid mode.");
            break;
        case WavFileResultErrorNeedInfoChecked:
            strcpy(buf, "Need check info.");
            break;
        case WavFileResultErrorNeedDataChecked:
            strcpy(buf, "Need check data.");
            break;
        case WavFileResultErrorInvalidHandler:
            strcpy(buf, "Invalid handler.");
            break;
        default:
            strcpy(buf, "Unkonwn error found.");
            break;
    }
}

