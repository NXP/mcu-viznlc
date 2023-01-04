/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief hal device registration for all needed here.
 */

#include <FreeRTOS.h>
#include <task.h>

#include "app_config.h"
#include "board.h"

#include "fwk_message.h"
#include "fwk_display_manager.h"
#include "fwk_camera_manager.h"
#include "fwk_input_manager.h"
#include "fwk_output_manager.h"
#include "fwk_vision_algo_manager.h"
#include "fwk_log.h"

#include "hal_smart_lock_config.h"
#include "hal_vision_algo.h"

#include "fonts/font.h"

int APP_RegisterHalDevices(void)
{
    int ret = 0;

    ret     = HAL_GfxDev_Pxp_Register();
    if (ret != 0)
    {
        LOGE("HAL_GfxDev_Pxp_Register error %d", ret);
        return ret;
    }

#if HEADLESS_ENABLE
#else
    display_output_t defaultDisplayOutput = FWK_Config_GetDisplayOutput();
    if ((defaultDisplayOutput >= kDisplayOutput_Panel) && (defaultDisplayOutput < kDisplayOutput_Invalid))
    {
        LOGD("[DisplayOutput]:%d", defaultDisplayOutput);
    }
    else
    {
        LOGE("Invalid display output %d, set to %d", defaultDisplayOutput, kDisplayOutput_Panel);
        defaultDisplayOutput = kDisplayOutput_Panel;
        FWK_Config_SetDisplayOutput(defaultDisplayOutput);
    }

    if (defaultDisplayOutput == kDisplayOutput_Panel)
    {
        ret = HAL_DisplayDev_LcdifRk024hh298_Register();

        if (ret != 0)
        {
            LOGE("Display panel register error %d", ret);
            return ret;
        }
    }
    else
    {

        ret = HAL_DisplayDev_UsbUvc_Register();
        if (ret != 0)
        {
            LOGE("HAL_DisplayDev_UsbUvc_Register error %d", ret);
            return ret;
        }

    }
#endif

#ifdef ENABLE_CAMERA_DEV_FlexioGc0308
    camera_dev_static_config_t gc0308_static_config = {
        .height   = CAMERA_HEIGHT_FLEXIO_GC0308,
        .width    = CAMERA_WIDTH_FLEXIO_GC0308,
        .pitch    = CAMERA_WIDTH_FLEXIO_GC0308 * 2,
        .left     = 0,
        .top      = 0,
        .right    = CAMERA_WIDTH_FLEXIO_GC0308 - 1,
        .bottom   = CAMERA_HEIGHT_FLEXIO_GC0308 - 1,
        .rotate   = CAMERA_ROTATION_FLEXIO_GC0308,
        .flip     = CAMERA_FLIP_FLEXIO_GC0308,
        .swapByte = CAMERA_SWAPBYTE_FLEXIO_GC0308,
    };

    ret = HAL_CameraDev_FlexioGc0308_Register(&gc0308_static_config);
    if (ret != 0)
    {
        LOGE("HAL_CameraDev_FlexioGc0308_Register error %d", ret);
        return ret;
    }
#endif

#ifdef ENABLE_CAMERA_DEV_CsiGc0308
    camera_dev_static_config_t csi_gc0308_static_config = {
        .height   = CAMERA_HEIGHT_CSI_GC0308,
        .width    = CAMERA_WIDTH_CSI_GC0308,
        .pitch    = CAMERA_WIDTH_CSI_GC0308 * 2,
        .left     = 0,
        .top      = 0,
        .right    = CAMERA_WIDTH_CSI_GC0308 - 1,
        .bottom   = CAMERA_HEIGHT_CSI_GC0308 - 1,
        .rotate   = CAMERA_ROTATION_CSI_GC0308,
        .flip     = CAMERA_FLIP_CSI_GC0308,
        .swapByte = CAMERA_SWAPBYTE_CSI_GC0308,
    };

    ret = HAL_CameraDev_CsiGc0308_Register(&csi_gc0308_static_config);
    if (ret != 0)
    {
        LOGE("HAL_CameraDev_CsiGc0308_Register error %d", ret);
        return ret;
    }

#endif

#ifdef ENABLE_CSI_SHARED_DUAL_CAMERA
    camera_dev_static_config_t csi_shared_dual_gc0308_static_config = {
        .height   = CAMERA_CSI_SHARED_DUAL_HEIGHT,
        .width    = CAMERA_CSI_SHARED_DUAL_WIDTH,
        .pitch    = CAMERA_CSI_SHARED_DUAL_WIDTH * CAMERA_CSI_SHARED_DUAL_BYTE_PER_PIXEL,
        .left     = 0,
        .top      = 0,
        .right    = CAMERA_CSI_SHARED_DUAL_WIDTH - 1,
        .bottom   = CAMERA_CSI_SHARED_DUAL_HEIGHT - 1,
        .rotate   = CAMERA_CSI_SHARED_DUAL_ROTATION,
        .flip     = CAMERA_CSI_SHARED_DUAL_FLIP,
        .swapByte = CAMERA_CSI_SHARED_DUAL_SWAPBYTE,
    };

    ret = HAL_CameraDev_CsiSharedDualGC0308_Register(&csi_shared_dual_gc0308_static_config);
    if (ret != 0)
    {
        LOGE("HAL_CameraDev_CsiSharedDualGC0308_Register error %d", ret);
        return ret;
    }

#endif

#if defined(APP_FFI)
    ret = HAL_VisionAlgo_OasisLite2D_Register(kOASISLiteMode_FFI);
#else // default APP_SMARTLOCK
    ret = HAL_VisionAlgo_OasisLite2D_Register(kOASISLiteMode_SmartLock);
#endif
    if (ret != 0)
    {
        LOGE("vision_algo_oasis_lite_register error %d", ret);
        return ret;
    }


    ret = HAL_InputDev_PushButtons_Register();
    if (ret != 0)
    {
        LOGE("HAL_InputDev_PushButtons_Register error %d", ret);
        return ret;
    }

#ifdef ENABLE_INPUT_DEV_ShellUsb
    ret = HAL_InputDev_ShellUsb_Register();
    if (ret != 0)
    {
        LOGE("HAL_InputDev_ShellUsb_Register error %d", ret);
        return ret;
    }
#elif defined(ENABLE_INPUT_DEV_ShellUart)
    ret = HAL_InputDev_ShellUart_Register();
    if (ret != 0)
    {
        LOGE("HAL_InputDev_ShellUart_Register error %d", ret);
        return ret;
    }
#endif

#ifdef ENABLE_OUTPUT_DEV_RgbLed
    ret = HAL_OutputDev_RgbLed_Register();
    if (ret != 0)
    {
        LOGE("HAL_OutputDev_RgbLed_Register error %d", ret);
        return ret;
    }
#endif

    ret = HAL_OutputDev_IrWhiteLeds_Register();
    if (ret != 0)
    {
        LOGE("HAL_OutputDev_IrWhiteLeds_Register error %d", ret);
        return ret;
    }

#if HEADLESS_ENABLE
#else
#if defined(APP_FFI)
    ret = HAL_OutputDev_UiFfi_Register();
#else // default APP_SMARTLOCK
    ret = HAL_OutputDev_UiSmartlock_Register();
#endif
    if (ret != 0)
    {
        LOGE("HAL_OutputDev_UiSmartlock_Register error %d", ret);
        return ret;
    }
#endif

    ret = HAL_OutputDev_SmartLockConfig_Register();
    if (ret != 0)
    {
        LOGE("HAL_OutputDev_SmartLockConfig_Register error %d", ret);
        return ret;
    }

#ifdef ENABLE_OUTPUT_DEV_MqsAudio_VIZNLC
    ret = HAL_OutputDev_MqsAudio_Register();
    if (ret != 0)
    {
        LOGE("HAL_OutputDev_MqsAudio_Register error %d", ret);
        return ret;
    }
#endif

#ifdef ENABLE_INPUT_DEV_Lpc845uart
    ret = HAL_Dev_ATCommands_Register();
    if (ret != 0)
    {
    	LOGE("SLN_DevBLEWUARTK32W61Register() error %d", ret);
    	return ret;
    }
#endif

#ifdef ENABLE_INPUT_DEV_BleWuartQn9090
    ret = HAL_Dev_BleWuartQn9090_Register();
    if (ret != 0)
    {
    	LOGE("SLN_DevBLEWUARTK32W61Register() error %d", ret);
    	return ret;
    }
#endif

    return ret;

}


//Overwrite the definition in hal_graphics_pxp.c
int HAL_GfxDev_Pxp_DrawText(const gfx_dev_t *dev,
                            gfx_surface_t *pOverlay,
                            const int x,
                            const int y,
                            const int text_color,
                            const int bg_color,
                            const int type,
                            const char *pText)
{
    int error = 0;

    if ((dev == NULL) || (pOverlay == NULL))
    {
        return -1;
    }

    if (pOverlay->format != kPixelFormat_RGB565)
    {
        LOGE("PIXEL_RGB565 is currently the only supported overlay surface");
        return -1;
    }

    uint16_t *pCanvasBuffer = (uint16_t *)pOverlay->buf;
    int rgb565Width         = pOverlay->pitch / sizeof(uint16_t);
    put_string_utf8(x, y, (char *)pText, (unsigned short)(text_color & 0xFFFF), (unsigned short)(bg_color & 0xFFFF),
                    (font_t)type, pCanvasBuffer, rgb565Width);

    return error;
}
