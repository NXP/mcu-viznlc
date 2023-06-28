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
 * @brief smart lock app configuration HAL driver implementation.
 */

#include "board_define.h"
#ifdef ENABLE_OUTPUT_DEV_SmartLockConfig
#include "fwk_platform.h"
#include "fwk_log.h"

#include "fwk_output_manager.h"
#include "hal_output_dev.h"
#include "hal_smart_lock_config.h"
#include "hal_event_descriptor_common.h"
#include "hal_event_descriptor_face_rec.h"
#include "hal_lpm_dev.h"
#include "app_config.h"

/* Temporary fix */
/* TODO: Remove this define */
#if defined(ENABLE_VOICE)
#include "hal_voice_algo_asr_local.h"
#endif

#define APP_CONFIG_VERSION_MINOR 0x4
#define APP_CONFIG_VERSION_MAJOR 0x0
#define APP_CONFIG_VERSION       (((APP_CONFIG_VERSION_MAJOR << 16) & 0xFF00) | (APP_CONFIG_VERSION_MINOR & 0xFF))

static hal_output_status_t _HAL_OutputDev_ConfigInputNotify(const output_dev_t *dev, void *data);
static hal_output_status_t _HAL_OutputDev_ConfigStop(const output_dev_t *dev);
static hal_output_status_t _HAL_OutputDev_ConfigStart(const output_dev_t *dev);

const static output_dev_operator_t s_OutputDev_SmartLockConfigOps = {
    /* For the config we are doing init first before the framework starts. we register just to receive events */
    .init   = NULL,
    .deinit = NULL,
    .start  = _HAL_OutputDev_ConfigStart,
    .stop   = _HAL_OutputDev_ConfigStop,
};

static output_dev_event_handler_t s_OutputDev_SmartLockEventHandler = {
    .inferenceComplete = NULL,
    .inputNotify       = _HAL_OutputDev_ConfigInputNotify,
};

static output_dev_t s_OutputDev_SmartLockConfig = {
    .name         = "APP_CONFIG",
    .attr.type    = kOutputDevType_Other,
    .attr.reserve = NULL,
    .ops          = &s_OutputDev_SmartLockConfigOps,
};

static hal_output_status_t _HAL_OutputDev_ConfigStart(const output_dev_t *dev)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    if (FWK_OutputManager_RegisterEventHandler(dev, &s_OutputDev_SmartLockEventHandler) != 0)
        error = kStatus_HAL_OutputError;
    return error;
}

static hal_output_status_t _HAL_OutputDev_ConfigStop(const output_dev_t *dev)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    if (FWK_OutputManager_UnregisterEventHandler(dev) != 0)
        error = kStatus_HAL_OutputError;
    return error;
}

static hal_output_status_t _HAL_OutputDev_ConfigInputNotify(const output_dev_t *dev, void *data)
{
    hal_output_status_t error = kStatus_HAL_OutputSuccess;
    event_base_t eventBase    = *(event_base_t *)data;

    switch (eventBase.eventId)
    {
        case kEventID_GetLogLevel:
        {
            log_level_event_t logLevel;
            event_status_t eventResponseStatus = kEventStatus_Ok;
            logLevel.logLevel                  = FWK_Config_GetLogLevel();

            if (eventBase.respond != NULL)
            {
                eventBase.respond(eventBase.eventId, &logLevel, eventResponseStatus, true);
            }
        }
        break;
        case kEventID_SetLogLevel:
        {
            event_status_t eventResponseStatus = kEventStatus_Ok;
            event_common_t event               = *(event_common_t *)data;
            log_level_t logLevel               = FWK_Config_GetLogLevel();
            if (logLevel != event.logLevel.logLevel)
            {
                hal_config_status_t status;
                status = FWK_Config_SetLogLevel(event.logLevel.logLevel);
                if (status == kSLNConfigStatus_Error)
                {
                    LOGE("Failed to write log level config.");
                    eventResponseStatus = kEventStatus_Error;
                }
                else
                {
                    LOGI("Log level config set successfully.");
                }
            }
            else
            {
                LOGI("Log level specified is the same as the current setting.");
            }

            if (eventBase.respond != NULL)
            {
                eventBase.respond(eventBase.eventId, &event.logLevel, eventResponseStatus, true);
            }
        }
        break;

        case kEventID_GetDisplayOutput:
        {
            display_output_event_t displayOutput;
            event_status_t eventResponseStatus = kEventStatus_Ok;
            displayOutput.displayOutput        = FWK_Config_GetDisplayOutput();

            if (eventBase.respond != NULL)
            {
                eventBase.respond(eventBase.eventId, &displayOutput, eventResponseStatus, true);
            }
        }
        break;

        case kEventID_SetDisplayOutput:
        {
            event_status_t eventResponseStatus = kEventStatus_Ok;
            event_common_t event               = *(event_common_t *)data;
            display_output_t displayMode       = FWK_Config_GetDisplayOutput();
            if (displayMode != event.displayOutput.displayOutput)
            {
                hal_config_status_t status;
                status = FWK_Config_SetDisplayOutput(event.displayOutput.displayOutput);
                if (status == kSLNConfigStatus_Error)
                {
                    LOGE("Failed to write display config");
                    eventResponseStatus = kEventStatus_Error;
                }
                else
                {
                    LOGI("Display config set successfully");
                }
            }
            else
            {
                LOGI("Display config specified is the same as the current setting.");
            }

            if (eventBase.respond != NULL)
            {
                eventBase.respond(eventBase.eventId, &event.displayOutput, eventResponseStatus, true);
            }
        }
        break;
        case kEventID_SetConnectivityType:
        {
            event_status_t eventResponseStatus   = kEventStatus_Ok;
            event_common_t event                 = *(event_common_t *)data;
            connectivity_type_t connectivityType = FWK_Config_GetConnectivityType();
            if (connectivityType != event.connectivity.connectivityType)
            {
                hal_config_status_t status;
                status = FWK_Config_SetConnectivityType(event.connectivity.connectivityType);
                if (status == kSLNConfigStatus_Error)
                {
                    LOGE("Failed to write connectivity type config");
                    eventResponseStatus = kEventStatus_Error;
                }
                else
                {
                    LOGI("Connectivity type set successfully");
                }
            }
            else
            {
                LOGI("Connectivity type specified is the same as the current setting.");
            }

            if (eventBase.respond != NULL)
            {
                eventBase.respond(eventBase.eventId, &event.connectivity, eventResponseStatus, true);
            }
        }
        break;
        case kEventID_GetConnectivityType:
        {
            connectivity_event_t connectivity;
            event_status_t eventResponseStatus = kEventStatus_Ok;
            connectivity.connectivityType      = FWK_Config_GetConnectivityType();
            if (eventBase.respond != NULL)
            {
                eventBase.respond(eventBase.eventId, &connectivity, eventResponseStatus, true);
            }
        }
        break;
        case kEventFaceRecID_GetFaceRecThreshold:
        {
            faceRecThreshold_event_t faceRecThreshold;
            event_status_t eventResponseStatus = kEventStatus_Error;
            hal_config_status_t ret            = kSLNConfigStatus_Success;
            faceRecThreshold.min               = MINIMUM_FACE_REC_THRESHOLD;
            faceRecThreshold.max               = MAXIMUM_FACE_REC_THRESHOLD;
            ret = HAL_OutputDev_SmartLockConfig_GetFaceRecThreshold(&faceRecThreshold.value);
            if (eventBase.respond != NULL)
            {
                if (ret == kSLNConfigStatus_Success)
                {
                    eventResponseStatus = kEventStatus_Ok;
                }
                eventBase.respond(eventBase.eventId, &faceRecThreshold, eventResponseStatus, true);
            }
        }
        break;
        case kEventFaceRecID_SetFaceRecThreshold:
        {
            event_status_t eventResponseStatus = kEventStatus_Error;
            event_face_rec_t *pEvent           = (event_face_rec_t *)data;
            hal_config_status_t ret            = kSLNConfigStatus_Success;
            if (pEvent)
            {
                ret = HAL_OutputDev_SmartLockConfig_SetFaceRecThreshold(pEvent->faceRecThreshold.value);
                if (eventBase.respond != NULL)
                {
                    if (ret == kSLNConfigStatus_Success)
                    {
                        eventResponseStatus = kEventStatus_Ok;
                    }
                    eventBase.respond(eventBase.eventId, &(pEvent->faceRecThreshold), eventResponseStatus, true);
                }
            }
        }
        break;
        default:
            break;
    }
    return error;
}

int HAL_OutputDev_SmartLockConfig_Register()
{
    int error = 0;
    LOGD("HAL_OutputDev_SmartLockConfig_Register");
    error = FWK_OutputManager_DeviceRegister(&s_OutputDev_SmartLockConfig);
    return error;
}

oasis_lite_mode_t HAL_OutputDev_SmartLockConfig_GetMode()
{
    oasis_lite_mode_t mode = kOASISLiteMode_Count;

    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        mode = smartLockConfig->mode;
        FWK_Config_UnlockAppData(false);
    }

    return mode;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetMode(oasis_lite_mode_t mode)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        smartLockConfig->mode = mode;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

uint8_t HAL_OutputDev_SmartLockConfig_GetIrPwm()
{
    uint8_t irPwm = -1;

    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        irPwm = smartLockConfig->irPwm;
        FWK_Config_UnlockAppData(false);
    }

    return irPwm;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetIrPwm(uint8_t brightness)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if ((brightness < 0) || (100 < brightness))
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->irPwm = brightness;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

uint8_t HAL_OutputDev_SmartLockConfig_GetWhitePwm()
{
    uint8_t whitePwm = -1;

    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        whitePwm = smartLockConfig->whitePwm;
        FWK_Config_UnlockAppData(false);
    }

    return whitePwm;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetWhitePwm(uint8_t brightness)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if ((brightness < 0) || (100 < brightness))
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->whitePwm = brightness;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_GetPassword(uint8_t *password)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        memcpy(password, smartLockConfig->password, sizeof(smartLockConfig->password));
        FWK_Config_UnlockAppData(false);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetPassword(uint8_t *password)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        memcpy(smartLockConfig->password, password, sizeof(smartLockConfig->password));
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetSpeakerVolume(uint32_t speakerVolume)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if ((speakerVolume < 0) || (100 < speakerVolume))
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->speakerVolume = speakerVolume;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

uint32_t HAL_OutputDev_SmartLockConfig_GetSpeakerVolume()
{
    uint32_t speakerVolume               = -1;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        speakerVolume = smartLockConfig->speakerVolume;
        FWK_Config_UnlockAppData(false);
    }

    return speakerVolume;
}

uint8_t HAL_OutputDev_SmartLockConfig_GetSleepMode()
{
    hal_lpm_manager_status_t sleepMode   = -1;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        sleepMode = smartLockConfig->sleepMode;
        FWK_Config_UnlockAppData(false);
    }

    return sleepMode;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetSleepMode(uint8_t sleepMode)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if ((sleepMode != kLPMManagerStatus_SleepDisable) && (sleepMode != kLPMManagerStatus_SleepEnable))
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->sleepMode = sleepMode;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_GetFaceRecThreshold(unsigned int *pThreshold)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        *pThreshold = smartLockConfig->faceRecThreshold;
        ret         = kSLNConfigStatus_Success;
        FWK_Config_UnlockAppData(false);
    }

    return ret;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetFaceRecThreshold(unsigned int threshold)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if ((smartLockConfig != NULL) &&
        ((threshold >= MINIMUM_FACE_REC_THRESHOLD) && (threshold <= MAXIMUM_FACE_REC_THRESHOLD)))
    {
        smartLockConfig->faceRecThreshold = threshold;
        ret                               = kSLNConfigStatus_Success;
        FWK_Config_UnlockAppData(true);
    }

    return ret;
}

/* Temporary fix */
/* TODO: Remove this define */
#if defined(ENABLE_VOICE)
asr_voice_config_t HAL_OutputDev_SmartLockConfig_GetAsrConfig()
{
    asr_voice_config_t asrConfig;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    asrConfig.status = READ_FAIL;

    if (smartLockConfig != NULL)
    {
        asrConfig = smartLockConfig->asrConfig;
        FWK_Config_UnlockAppData(false);
    }

    return asrConfig;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetAsrConfig(asr_voice_config_t asrConfig)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        smartLockConfig->asrConfig        = asrConfig;
        smartLockConfig->asrConfig.status = WRITE_SUCCESS;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

uint32_t HAL_OutputDev_SmartLockConfig_GetAsrTimeoutDuration()
{
    uint32_t timeout_duration = -1;

    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        timeout_duration = smartLockConfig->asrConfig.timeout;
        FWK_Config_UnlockAppData(false);
    }

    return timeout_duration;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetAsrTimeoutDuration(uint32_t duration)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (duration < 4000)
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->asrConfig.timeout = duration;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

asr_followup_t HAL_OutputDev_SmartLockConfig_GetAsrFollowupStatus()
{
    asr_followup_t followup_enabled = -1;

    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        followup_enabled = smartLockConfig->asrConfig.followup;
        FWK_Config_UnlockAppData(false);
    }

    return followup_enabled;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetAsrFollowupStatus(asr_followup_t followup)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        smartLockConfig->asrConfig.followup = followup;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

asr_language_t HAL_OutputDev_SmartLockConfig_GetAsrMultilingualConfig()
{
    asr_language_t multilingualConfig    = ASR_ENGLISH;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        multilingualConfig = smartLockConfig->asrConfig.multilingual;
        FWK_Config_UnlockAppData(false);
    }

    return multilingualConfig;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetAsrMultilingualConfig(asr_language_t multilingual_config)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (((multilingual_config) & ~(ASR_ENGLISH | ASR_CHINESE | ASR_GERMAN | ASR_FRENCH)) ||
        (multilingual_config == UNDEFINED_LANGUAGE))
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->asrConfig.multilingual = multilingual_config;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}

asr_inference_t HAL_OutputDev_SmartLockConfig_GetAsrDemo()
{
    asr_inference_t demo                 = ASR_CMD_IOT;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if (smartLockConfig != NULL)
    {
        demo = smartLockConfig->asrConfig.demo;
        FWK_Config_UnlockAppData(false);
    }

    return demo;
}

hal_config_status_t HAL_OutputDev_SmartLockConfig_SetAsrDemo(asr_inference_t demo)
{
    hal_config_status_t ret              = kSLNConfigStatus_Error;
    smart_lock_config_t *smartLockConfig = (smart_lock_config_t *)FWK_Config_LockAppData();

    if ((demo != ASR_CMD_IOT) && (demo != ASR_CMD_ELEVATOR) && (demo != ASR_CMD_ELEVATOR) && (demo != ASR_CMD_AUDIO) &&
        (demo != ASR_CMD_LED) && (demo != ASR_CMD_DIALOGIC_1))
    {
        FWK_Config_UnlockAppData(false);
        return ret;
    }

    if (smartLockConfig != NULL)
    {
        smartLockConfig->asrConfig.demo = demo;
        FWK_Config_UnlockAppData(true);
        ret = kSLNConfigStatus_Success;
    }

    return ret;
}
#endif

hal_config_status_t HAL_OutputDev_SmartLockConfig_Init()
{
    hal_config_status_t ret;
    ret = FWK_Config_Init();

    if (ret == kSLNConfigStatus_Success)
    {
        unsigned int app_version = FWK_Config_GetAppDataVersion();
        if (app_version != APP_CONFIG_VERSION)
        {
            // TODO update mechanism to be decided
            smart_lock_config_t app_config;
            memset(&app_config, 0, sizeof(smart_lock_config_t));
#if defined(SMART_ACCESS_2D) || defined(SMART_ACCESS_3D)
            app_config.mode = 1;
#endif
            app_config.speakerVolume = 100;
#if ENABLE_CSI_3DCAMERA || ENABLE_MIPI_3DCAMERA || ENABLE_3D_SIMCAMERA
            app_config.irPwm = 0;
#else
            app_config.irPwm = 100;
#endif
            memcpy(app_config.password, "000000", sizeof(app_config.password));
            app_config.faceRecThreshold = DEFAULT_FACE_REC_THRESHOLD;
            FWK_Config_SetAppData(&app_config, sizeof(smart_lock_config_t), APP_CONFIG_VERSION);
        }
    }

    return ret;
}
#endif /* ENABLE_OUTPUT_DEV_SmartLockConfig */
