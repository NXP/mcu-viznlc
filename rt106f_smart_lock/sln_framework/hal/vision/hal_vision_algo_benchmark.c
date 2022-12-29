/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the license terms that accompany it. By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */

/*
 * @brief vision algorithm benchmark HAL driver implementation for the smart lock application.
 */

#include "board_define.h"
#ifdef ENABLE_VISIONALGO_DEV_Benchmark
#include <FreeRTOS.h>
#include <stdlib.h>
#include <stdio.h>
#include <task.h>
#include "fwk_log.h"
#include "fwk_platform.h"
#include "fwk_vision_algo_manager.h"

// in portrait mode
#define BENCHMARK_RGB_FRAME_HEIGHT         640
#define BENCHMARK_RGB_FRAME_WIDTH          480
#define BENCHMARK_RGB_FRAME_BYTE_PER_PIXEL 3

#define BENCHMARK_IR_FRAME_HEIGHT         640
#define BENCHMARK_IR_FRAME_WIDTH          480
#define BENCHMARK_IR_FRAME_BYTE_PER_PIXEL 3

#define BENCHMARK_FRAME_COUNT 2

AT_FB_SHMEM_SECTION_ALIGN(static uint8_t s_InputFrames[BENCHMARK_FRAME_COUNT][BENCHMARK_RGB_FRAME_HEIGHT]
                                                      [BENCHMARK_RGB_FRAME_WIDTH * BENCHMARK_RGB_FRAME_BYTE_PER_PIXEL],
                          32);

static hal_valgo_status_t HAL_VisionAlgoDev_Benchmark_Init(vision_algo_dev_t *dev,
                                                           valgo_dev_callback_t callback,
                                                           void *param)
{
    hal_valgo_status_t ret = kStatus_HAL_ValgoSuccess;
    LOGD("++HAL_VisionAlgoDev_Benchmark_Init");

    // init the device
    memset(&dev->cap, 0, sizeof(dev->cap));
    dev->cap.callback = callback;

    dev->data.autoStart = 1;

    // init the RGB frame
    dev->data.frames[kVAlgoFrameID_RGB].height       = BENCHMARK_RGB_FRAME_HEIGHT;
    dev->data.frames[kVAlgoFrameID_RGB].width        = BENCHMARK_RGB_FRAME_WIDTH;
    dev->data.frames[kVAlgoFrameID_RGB].pitch        = BENCHMARK_RGB_FRAME_WIDTH * BENCHMARK_RGB_FRAME_BYTE_PER_PIXEL;
    dev->data.frames[kVAlgoFrameID_RGB].is_supported = 1;
    dev->data.frames[kVAlgoFrameID_RGB].rotate       = kCWRotateDegree_0;
    dev->data.frames[kVAlgoFrameID_RGB].flip         = kFlipMode_None;

    dev->data.frames[kVAlgoFrameID_RGB].format    = kPixelFormat_BGR;
    dev->data.frames[kVAlgoFrameID_RGB].srcFormat = kPixelFormat_UYVY1P422_RGB;
    dev->data.frames[kVAlgoFrameID_RGB].data      = s_InputFrames[0];

    dev->data.frames[kVAlgoFrameID_IR].height       = BENCHMARK_IR_FRAME_HEIGHT;
    dev->data.frames[kVAlgoFrameID_IR].width        = BENCHMARK_IR_FRAME_WIDTH;
    dev->data.frames[kVAlgoFrameID_IR].pitch        = BENCHMARK_IR_FRAME_WIDTH * BENCHMARK_IR_FRAME_BYTE_PER_PIXEL;
    dev->data.frames[kVAlgoFrameID_IR].is_supported = 0;
    dev->data.frames[kVAlgoFrameID_IR].rotate       = kCWRotateDegree_0;
    dev->data.frames[kVAlgoFrameID_IR].flip         = kFlipMode_None;

    dev->data.frames[kVAlgoFrameID_IR].format    = kPixelFormat_BGR;
    dev->data.frames[kVAlgoFrameID_IR].srcFormat = kPixelFormat_UYVY1P422_Gray;
    dev->data.frames[kVAlgoFrameID_IR].data      = s_InputFrames[1];

    LOGD("--HAL_VisionAlgoDev_Benchmark_Init");
    return ret;
}

// deinitialize the dev
static hal_valgo_status_t HAL_VisionAlgoDev_Benchmark_Deinit(vision_algo_dev_t *dev)
{
    hal_valgo_status_t ret = kStatus_HAL_ValgoSuccess;
    LOGD("++HAL_VisionAlgoDev_Benchmark_Deinit");

    LOGD("--HAL_VisionAlgoDev_Benchmark_Deinit");
    return ret;
}

// start the dev
static hal_valgo_status_t HAL_VisionAlgoDev_Benchmark_Run(const vision_algo_dev_t *dev, void *data)
{
    hal_valgo_status_t ret = kStatus_HAL_ValgoSuccess;
    // LOGD("+-HAL_VisionAlgoDev_Benchmark_Run");

    int frameSize = BENCHMARK_RGB_FRAME_HEIGHT * BENCHMARK_RGB_FRAME_WIDTH * BENCHMARK_RGB_FRAME_BYTE_PER_PIXEL;
    memset(dev->data.frames[kVAlgoFrameID_RGB].data, 0, frameSize);

    return ret;
}

static hal_valgo_status_t HAL_VisionAlgoDev_Benchmark_InputNotify(const vision_algo_dev_t *receiver, void *data)
{
    hal_valgo_status_t ret = kStatus_HAL_ValgoSuccess;
    LOGD("++HAL_VisionAlgoDev_Benchmark_InputNotify");

    LOGD("--HAL_VisionAlgoDev_Benchmark_InputNotify");
    return ret;
}

const static vision_algo_dev_operator_t s_VisionAlgoDev_BenchmarkOps = {
    .init        = HAL_VisionAlgoDev_Benchmark_Init,
    .deinit      = HAL_VisionAlgoDev_Benchmark_Deinit,
    .run         = HAL_VisionAlgoDev_Benchmark_Run,
    .inputNotify = HAL_VisionAlgoDev_Benchmark_InputNotify,
};

static vision_algo_dev_t s_VisionAlgoDev_Benchmark = {
    .id   = 0,
    .name = "VALGO_Benchmark",
    .ops  = (vision_algo_dev_operator_t *)&s_VisionAlgoDev_BenchmarkOps,
    .cap  = {.param = NULL},
};

int HAL_VisionAlgoDev_Benchmark_Register()
{
    int error = 0;
    LOGD("HAL_VisionAlgoDev_Benchmark_Register");
    error = FWK_VisionAlgoManager_DeviceRegister(&s_VisionAlgoDev_Benchmark);
    return error;
}

#endif /*ENABLE_VISIONALGO_DEV_Benchmark*/
