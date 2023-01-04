/*
 * Copyright 2020-2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef HAL_AUDIO_DEFS_H_
#define HAL_AUDIO_DEFS_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define AUDIO_PDM_MIC_COUNT (2U)

/*******************************************************************************
 * PCM Stream Sample Definitions
 ******************************************************************************/
#define AUDIO_PCM_SINGLE_CH_SMPL_COUNT (160U)
#define AUDIO_PCM_SAMPLE_SIZE_BYTES    (4U) /* 32bit depth */
#define AUDIO_PCM_SAMPLE_COUNT         (AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AUDIO_PDM_MIC_COUNT)
#define AUDIO_PCM_BUFFER_SIZE          (AUDIO_PCM_SAMPLE_COUNT * AUDIO_PCM_SAMPLE_SIZE_BYTES)
#define AUDIO_PCM_BUFFER_COUNT         (2U)
#define AUDIO_PCM_SAMPLE_RATE          (16000U) /*!< Sample rate 16kHz */

/*******************************************************************************
 * AFE / ASR Definitions
 ******************************************************************************/

#ifdef ENABLE_DSMT_ASR
#define ASR_INPUT_FRAMES   3
#define ASR_INPUT_CHANNELS 1
/* Set AFE_INPUT_MIC_SAMPLE_BYTES to 2 for 16 bit depth microphone data.
 * Set AFE_INPUT_MIC_SAMPLE_BYTES to 4 for 32 bit depth microphone data. */
#define AFE_INPUT_MIC_SAMPLE_BYTES 4
#else
#define ASR_INPUT_FRAMES   1
#define ASR_INPUT_CHANNELS 2
/* VIT Engine does not support AFE yet.
 * And so far VIT only support 16bit microphone raw data as input */
#define AFE_INPUT_MIC_SAMPLE_BYTES 2
#endif /* ENABLE_DSMT_ASR */

#define AFE_INPUT_MIC_BUFFER_SIZE (AUDIO_PCM_SAMPLE_COUNT * AFE_INPUT_MIC_SAMPLE_BYTES)
#define AFE_INPUT_AMP_BUFFER_SIZE (AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_INPUT_AMP_SAMPLE_BYTES)
#define AFE_OUTPUT_BUFFER_SIZE    (AUDIO_PCM_SINGLE_CH_SMPL_COUNT * AFE_OUTPUT_SAMPLE_BYTES * ASR_INPUT_CHANNELS)
#define ASR_INPUT_BUFFER_SIZE     (AUDIO_PCM_SINGLE_CH_SMPL_COUNT * ASR_INPUT_SAMPLE_BYTES * ASR_INPUT_FRAMES * ASR_INPUT_CHANNELS)

typedef int32_t pcm_input_t[AUDIO_PCM_BUFFER_COUNT][AUDIO_PCM_SAMPLE_COUNT];

#endif /* HAL_AUDIO_DEFS_H_ */
