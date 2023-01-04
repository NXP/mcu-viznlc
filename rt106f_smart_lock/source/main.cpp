/*
 * Copyright 2019-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/**
 * @brief   Application entry point.
 */

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "fwk_log.h"
#include "fwk_message.h"
#include "fwk_display_manager.h"
#include "fwk_camera_manager.h"
#include "fwk_input_manager.h"
#include "fwk_output_manager.h"
#include "fwk_vision_algo_manager.h"

/*Smaller the number, higher priority it is, for UVC mode to work normally, please make sure
 * DISPLAY task and INPUT task has same priority*/
#define TASK_PRIORITY_CAMERA 1
#define TASK_PRIORITY_DISPLAY 2
#define TASK_PRIORITY_INPUT 2
#define TASK_PRIORITY_OUTPUT 4
#define TASK_PRIORITY_ALGO 6

/* Logging task configuration */
#define LOGGING_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)
#define LOGGING_TASK_STACK_SIZE 512
#define LOGGING_QUEUE_LENGTH    64
#include "hal_smart_lock_config.h"

const char *g_coreName = "CM7";

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

extern void BOARD_InitHardware(void);
extern int HAL_FlashDev_Littlefs_Register();
int APP_RegisterHalDevices(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

void APP_BoardInit(void)
{
    BOARD_InitHardware();
}


/*
 * Get the current time in us.
 */
unsigned int FWK_CurrentTimeUs()
{
    unsigned int timeValue = Time_Current();
    unsigned int timeUnit  = Time_Unit();
    return (timeValue * timeUnit);
}

int APP_InitFramework(void)
{

    int ret = 0;

    ret = HAL_FlashDev_Littlefs_Register();
    if (ret != 0)
    {
        LOGE("HAL_FlashDev_Littlefs_Init error %d", ret);
        return ret;
    }

    ret = HAL_OutputDev_SmartLockConfig_Init();
    if (ret != 0)
    {
        LOGE("HAL_OutputDev_SmartLockConfig_Init error %d", ret);
        return ret;
    }

    ret = FWK_CameraManager_Init();
    if (ret != 0)
    {
        LOGE("FWK_CameraManager_Init error %d", ret);
        return ret;
    }

    ret = FWK_DisplayManager_Init();
    if (ret != 0)
    {
        LOGE("FWK_DisplayManager_Init error %d", ret);
        return ret;
    }

    ret = FWK_VisionAlgoManager_Init();
    if (ret != 0)
    {
        LOGE("FWK_VisionAlgoManager_Init error %d", ret);
        return ret;
    }

    ret = FWK_OutputManager_Init();
    if (ret != 0)
    {
        LOGE("FWK_OutputManager_Init error %d", ret);
        return ret;
    }

    ret = FWK_InputManager_Init();
    if (ret != 0)
    {
        LOGE("FWK_InputManager_Init error %d", ret);
        return ret;
    }

    return ret;

}

int APP_StartFramework(void)
{
    int ret = 0;

    ret = FWK_CameraManager_Start(TASK_PRIORITY_CAMERA);
    if (ret != 0)
    {
        LOGE("FWK_CameraManager_Start error %d", ret);
        return ret;
    }

    ret = FWK_DisplayManager_Start(TASK_PRIORITY_DISPLAY);
    if (ret != 0)
    {
        LOGE("FWK_DisplayManager_Start error %d", ret);
        return ret;
    }

    ret = FWK_VisionAlgoManager_Start(TASK_PRIORITY_ALGO);
    if (ret != 0)
    {
        LOGE("FWK_VisionAlgoManager_Start error %d", ret);
        return ret;
    }

    ret = FWK_OutputManager_Start(TASK_PRIORITY_OUTPUT);
    if (ret != 0)
    {
        LOGE("FWK_OutputManager_Start error %d", ret);
        return ret;
    }

    ret = FWK_InputManager_Start(TASK_PRIORITY_INPUT);
    if (ret != 0)
    {
        LOGE("FWK_InputManager_Start error %d", ret);
        return ret;
    }

    return ret;
}

/*
 * @brief   Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
    APP_BoardInit();
#if LOG_ENABLE
    xLoggingTaskInitialize(LOGGING_TASK_STACK_SIZE, LOGGING_TASK_PRIORITY, LOGGING_QUEUE_LENGTH);
#endif
    /* init the framework*/
    APP_InitFramework();

    /* register the hal devices*/
    APP_RegisterHalDevices();

    /* start the framework*/
    APP_StartFramework();

    // start
    vTaskStartScheduler();

    while (1)
    {
        LOGD("#");
    }

    return 0;
}
