/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief camera dev 3D simulator HAL driver implementation.
 */

#include "board_define.h"
#ifdef ENABLE_CAMERA_DEV_2DSim
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <time.h>

#include "fwk_timer.h"
#include "fwk_log.h"
#include "fwk_camera_manager.h"
#include "hal_camera_dev.h"
#include "hal_camera_2d_sim_rgb_frame.h"
#include "hal_camera_2d_sim_ir_frame.h"

#define CAMERA_NAME               "2d_sim"
#define CAMERA_RGB_PIXEL_FORMAT   kPixelFormat_UYVY1P422_RGB
#define CAMERA_IR_PIXEL_FORMAT    kPixelFormat_UYVY1P422_Gray
#define CAMERA_WIDTH              640
#define CAMERA_HEIGHT             480
#define CAMERA_BYTES_PER_PIXEL    2
#define CAMERA_VSYNC_TIME         30
#define SIM_FRAME_COUNT           2

static fwk_timer_t *s_pVsyncTimer;

static unsigned char s_Frames[SIM_FRAME_COUNT][CAMERA_WIDTH * CAMERA_HEIGHT * CAMERA_BYTES_PER_PIXEL];

static int s_FrameIndex = 0;

static void HAL_CameraDev_2DSim_ReceiverCallback(void *arg)
{
    camera_dev_t *dev = (camera_dev_t *)arg;

    s_FrameIndex++;
    if (s_FrameIndex == SIM_FRAME_COUNT)
    {
        s_FrameIndex = 0;
    }

    LOGI("2D:%d", s_FrameIndex);

    if (dev->cap.callback != NULL)
    {
        uint8_t fromISR = __get_IPSR();
        dev->cap.callback(dev, kCameraEvent_SendFrame, dev->cap.param, fromISR);
    }
}

static hal_camera_status_t HAL_CameraDev_2DSim_Init(
    camera_dev_t *dev, int width, int height, camera_dev_callback_t callback, void *param)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    dev->cap.callback       = callback;
    dev->cap.param          = param;

    for (int i = 0; i < SIM_FRAME_COUNT; i++)
    {
        if (i % 2)
        {
            /* RGB frame 0, 2, 4 */
            memcpy((void *)s_Frames[i], s_480_640_RGB_FRAME,
                   CAMERA_WIDTH * CAMERA_HEIGHT * CAMERA_BYTES_PER_PIXEL);
        }
        else
        {
            /* IR  frame 1, 3, 5 */
            memcpy((void *)s_Frames[i], s_480_640_IR_FRAME,
                   CAMERA_WIDTH * CAMERA_HEIGHT * CAMERA_BYTES_PER_PIXEL);
        }
    }

    return ret;
}

static hal_camera_status_t HAL_CameraDev_2DSim_Deinit(camera_dev_t *dev)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    return ret;
}

static hal_camera_status_t HAL_CameraDev_2DSim_Start(const camera_dev_t *dev)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    int status;
    LOGI("HAL_CameraDev_2DSim_Start");

    status = FWK_Timer_Start("CameraDev2DSim", CAMERA_VSYNC_TIME, 1, HAL_CameraDev_2DSim_ReceiverCallback, (void *)dev,
                             &s_pVsyncTimer);

    if (status)
    {
        LOGE("Failed to start timer \"CameraDev2DSim\"");
        ret = kStatus_HAL_CameraError;
    }

    LOGI("HAL_CameraDev_2DSim_Start");
    return ret;
}

static hal_camera_status_t HAL_CameraDev_2DSim_Enqueue(const camera_dev_t *dev, void *data)
{
    LOGI("++HAL_CameraDev_2DSim_Start");
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;

    LOGI("--HAL_CameraDev_2DSim_Enqueue");
    return ret;
}

static hal_camera_status_t HAL_CameraDev_2DSim_Dequeue(const camera_dev_t *dev, void **data, pixel_format_t *format)
{
    LOGI("++HAL_CameraDev_2DSim_Dequeue: %d", s_FrameIndex);

    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;

    *data = (void *)s_Frames[s_FrameIndex];

    if (s_FrameIndex % 2)
    {
        /* RGB frame 0, 2, 4 */
        LOGI("2D_RGB");
        *format = CAMERA_RGB_PIXEL_FORMAT;
    }
    else
    {
        /* IR  frame 1, 3, 5 */
        LOGI("2D_IR");
        *format = CAMERA_IR_PIXEL_FORMAT;
    }

    LOGI("--HAL_CameraDev_2DSim_Dequeue");
    return ret;
}

const static camera_dev_operator_t s_CameraDev_2DSimOps = {
    .init        = HAL_CameraDev_2DSim_Init,
    .deinit      = HAL_CameraDev_2DSim_Deinit,
    .start       = HAL_CameraDev_2DSim_Start,
    .enqueue     = HAL_CameraDev_2DSim_Enqueue,
    .dequeue     = HAL_CameraDev_2DSim_Dequeue,
    .inputNotify = NULL,
};

static camera_dev_t s_CameraDev_2DSim = {
    .id   = 0,
    .name = CAMERA_NAME,
    .ops  = &s_CameraDev_2DSimOps,
    .config =
        {
            .height   = CAMERA_HEIGHT,
            .width    = CAMERA_WIDTH,
            .pitch    = CAMERA_WIDTH * CAMERA_BYTES_PER_PIXEL,
            .left     = 0,
            .top      = 0,
            .right    = CAMERA_WIDTH - 1,
            .bottom   = CAMERA_HEIGHT - 1,
            .rotate   = kCWRotateDegree_90,
            .flip     = kFlipMode_None,
            .swapByte = 0,
        },
    .cap =
        {

            .callback = NULL,
            .param    = NULL,
        },
};

int HAL_CameraDev_2DSim_Register()
{
    int error = 0;
    LOGD("HAL_CameraDev_2DSim_Register");
    error = FWK_CameraManager_DeviceRegister(&s_CameraDev_2DSim);
    return error;
}
#endif /* ENABLE_CAMERA_DEV_2DSim */
