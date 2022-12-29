/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief camera dev csi mt9m114 HAL driver implementation.
 */

#include "board_define.h"
#ifdef ENABLE_CAMERA_DEV_CsiMt9m114
#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <time.h>

#include "fsl_camera.h"
#include "fsl_camera_receiver.h"
#include "fsl_camera_device.h"
#include "fsl_csi.h"
#include "fsl_csi_camera_adapter.h"

#include "fsl_mt9m114.h"

#include "board.h"

#include "fwk_log.h"
#include "fwk_camera_manager.h"
#include "hal_camera_dev.h"

#define CAMERA_NAME             "CSI_MT9M114"
#define CAMERA_RGB_CONTROL_FLAGS (kCAMERA_HrefActiveHigh | kCAMERA_DataLatchOnRisingEdge)

//AT_NONCACHEABLE_SECTION_ALIGN(
SDK_ALIGN(
    static uint8_t s_FrameBuffers[CAMERA_DEV_CsiMt9m114_BUFFER_COUNT][CAMERA_DEV_CsiMt9m114_HEIGHT]
                                 [CAMERA_DEV_CsiMt9m114_WIDTH * CAMERA_DEV_CsiMt9m114_BPP], 32);

static uint8_t *s_pCurrentFrameBuffer = NULL;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
void BOARD_CSICameraPullResetPin(bool pullUp);
void BOARD_CSICameraPullPowerDownPin(bool pullUp);
void BOARD_CSICameraI2CInit(void);
status_t BOARD_CSICameraI2CSend(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
status_t BOARD_CSICameraI2CReceive(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
#if defined(__cplusplus)
}
#endif /* __cplusplus */

static csi_resource_t s_CsiResource = {
    .csiBase = CSI,
};

static csi_private_data_t s_CsiPrivateData;

static camera_receiver_handle_t s_CameraReceiver = {
    .resource    = &s_CsiResource,
    .ops         = &csi_ops,
    .privateData = &s_CsiPrivateData,
};

static mt9m114_resource_t s_Mt9m114Resource = {
    .i2cSendFunc       = BOARD_CSICameraI2CSend,
    .i2cReceiveFunc    = BOARD_CSICameraI2CReceive,
    .pullResetPin      = BOARD_CSICameraPullResetPin,
    .pullPowerDownPin  = BOARD_CSICameraPullPowerDownPin,
    .inputClockFreq_Hz = 24000000,
    .i2cAddr           = MT9M114_I2C_ADDR,
};

static camera_device_handle_t cameraDevice = {
    .resource = &s_Mt9m114Resource,
    .ops      = &mt9m114_ops,
};

static void _HAL_CameraDev_InitResources(void)
{
    clock_root_config_t rootCfg = {
        .mux = kCLOCK_CSI_ClockRoot_MuxOscRc48MDiv2,
        .div = 1,
    };

    /* Configure CSI using OSC_RC_48M_DIV2 */
    CLOCK_SetRootClock(kCLOCK_Root_Csi, &rootCfg);

    BOARD_CSICameraI2CInit();
}

static void _HAL_CameraDev_InitInterface(void)
{
    CLOCK_EnableClock(kCLOCK_Video_Mux);

    /* CSI Sensor data is from Parallel CSI . */
    VIDEO_MUX->VID_MUX_CTRL.CLR = VIDEO_MUX_VID_MUX_CTRL_CSI_SEL_MASK;
}

static void HAL_CameraDev_CsiMt9m114_ReceiverCallback(camera_receiver_handle_t *handle, status_t status, void *userData)
{
    camera_dev_t *dev = (camera_dev_t *)userData;

    if (dev->cap.callback != NULL)
    {
        uint8_t fromISR = __get_IPSR();
        dev->cap.callback(dev, kCameraEvent_SendFrame, dev->cap.param, fromISR);
    }
}

static hal_camera_status_t HAL_CameraDev_CsiMt9m114_InputNotify(const camera_dev_t *dev, void *data)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;

    return ret;
}

static hal_camera_status_t HAL_CameraDev_CsiMt9m114_Init(
    camera_dev_t *dev, int width, int height, camera_dev_callback_t callback, void *param)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    camera_config_t cameraConfig;
    LOGD("++HAL_CameraDev_CsiMt9m114_Init(param[%p])", param);

    dev->config.width  = width;
    dev->config.height = height;
    dev->cap.callback  = callback;
    dev->cap.param     = param;

    // init csi receiver
    memset(&cameraConfig, 0, sizeof(cameraConfig));
    cameraConfig.pixelFormat                = kVIDEO_PixelFormatYUYV;
    cameraConfig.bytesPerPixel              = CAMERA_DEV_CsiMt9m114_BPP;
    cameraConfig.resolution                 = FSL_VIDEO_RESOLUTION(width, height);
    cameraConfig.frameBufferLinePitch_Bytes = width * CAMERA_DEV_CsiMt9m114_BPP;
    cameraConfig.interface                  = kCAMERA_InterfaceGatedClock;
    cameraConfig.controlFlags               = CAMERA_RGB_CONTROL_FLAGS;
    cameraConfig.framePerSec                = 15;

    _HAL_CameraDev_InitResources();

    _HAL_CameraDev_InitInterface();

    NVIC_SetPriority(CSI_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1);
    CAMERA_RECEIVER_Init(&s_CameraReceiver, &cameraConfig, HAL_CameraDev_CsiMt9m114_ReceiverCallback, dev);

    // init camera dev
    CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);

    for (int i = 0; i < CAMERA_DEV_CsiMt9m114_BUFFER_COUNT; i++)
    {
        CAMERA_RECEIVER_SubmitEmptyBuffer(&s_CameraReceiver, (uint32_t)s_FrameBuffers[i]);
    }

    LOGD("--HAL_CameraDev_CsiMt9m114_Init");
    return ret;
}

static hal_camera_status_t HAL_CameraDev_CsiMt9m114_Deinit(camera_dev_t *dev)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    return ret;
}

static hal_camera_status_t HAL_CameraDev_CsiMt9m114_Start(const camera_dev_t *dev)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    LOGD("++HAL_CameraDev_CsiMt9m114_Start");
    CAMERA_DEVICE_Start(&cameraDevice);
    CAMERA_RECEIVER_Start(&s_CameraReceiver);
    LOGD("--HAL_CameraDev_CsiMt9m114_Start");
    return ret;
}

static hal_camera_status_t HAL_CameraDev_CsiMt9m114_Enqueue(const camera_dev_t *dev, void *data)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    LOGI("++HAL_CameraDev_CsiMt9m114_Enqueue");

    if (s_pCurrentFrameBuffer != NULL)
    {
        CAMERA_RECEIVER_SubmitEmptyBuffer(&s_CameraReceiver, (uint32_t)s_pCurrentFrameBuffer);
        s_pCurrentFrameBuffer = NULL;
    }

    LOGI("--HAL_CameraDev_CsiMt9m114_Enqueue");
    return ret;
}

static hal_camera_status_t HAL_CameraDev_CsiMt9m114_Dequeue(const camera_dev_t *dev,
                                                            void **data,
                                                            pixel_format_t *format)
{
    hal_camera_status_t ret = kStatus_HAL_CameraSuccess;
    LOGI("++HAL_CameraDev_CsiMt9m114_Dequeue");

    while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&s_CameraReceiver, (uint32_t *)&s_pCurrentFrameBuffer))
    {
    }

    *data   = s_pCurrentFrameBuffer;
    *format = CAMERA_DEV_CsiMt9m114_FORMAT;

    LOGI("--HAL_CameraDev_CsiMt9m114_Dequeue");
    return ret;
}

const static camera_dev_operator_t s_CameraDev_CsiMt9m114Ops = {.init        = HAL_CameraDev_CsiMt9m114_Init,
                                                                .deinit      = HAL_CameraDev_CsiMt9m114_Deinit,
                                                                .start       = HAL_CameraDev_CsiMt9m114_Start,
                                                                .enqueue     = HAL_CameraDev_CsiMt9m114_Enqueue,
                                                                .dequeue     = HAL_CameraDev_CsiMt9m114_Dequeue,
                                                                .inputNotify = HAL_CameraDev_CsiMt9m114_InputNotify};

static camera_dev_t s_CameraDevCsi_Mt9m114 = {
    .id   = 1,
    .ops  = &s_CameraDev_CsiMt9m114Ops,
    .name = CAMERA_NAME,
    .config =
        {
            .height   = CAMERA_DEV_CsiMt9m114_HEIGHT,
            .width    = CAMERA_DEV_CsiMt9m114_WIDTH,
            .pitch    = CAMERA_DEV_CsiMt9m114_WIDTH * CAMERA_DEV_CsiMt9m114_BPP,
            .left     = CAMERA_DEV_CsiMt9m114_LEFT,
            .top      = CAMERA_DEV_CsiMt9m114_TOP,
            .right    = CAMERA_DEV_CsiMt9m114_RIGHT,
            .bottom   = CAMERA_DEV_CsiMt9m114_BOTTOM,
            .rotate   = CAMERA_DEV_CsiMt9m114_ROTATE,
            .flip     = CAMERA_DEV_CsiMt9m114_FLIP,
            .swapByte = CAMERA_DEV_CsiMt9m114_SWAPBYTE,
        },
    .cap = {.callback = NULL, .param    = NULL, },
};

int HAL_CameraDev_CsiMt9m114_Register()
{
    int error = 0;
    LOGD("HAL_CameraDev_CsiMt9m114_Register");
    error = FWK_CameraManager_DeviceRegister(&s_CameraDevCsi_Mt9m114);
    return error;
}
#endif /* ENABLE_CAMERA_DEV_CsiMt9m114 */
