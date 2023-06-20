/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.d
 *
 * Created by: NXP IoT Solutions Team.
 */

/*
 * @brief  board type define for all needed macros, please all place here before compiling.
 *
 * @Author jianfeng.qin@nxp.com
 */

#ifndef BOARD_DEFINE_H_
#define BOARD_DEFINE_H_

/*
 *  Board define
 */

#define DEBUG_CONSOLE_UART_INDEX 2
#define CAMERA_DIFF_I2C_BUS           1

/*
 * Enablement of the HAL devices
 */
#define ENABLE_GFX_DEV_Pxp
#define ENABLE_DISPLAY_DEV_LcdifRk024hh298
#define ENABLE_DISPLAY_DEV_UsbUvc
//#define ENABLE_DISPLAY_DEV_UsbCdc2D
#define ENABLE_CSI_SHARED_DUAL_CAMERA
#define ENABLE_FLASH_DEV_Littlefs
#define ENABLE_VISIONALGO_DEV_OasisLite2D
#define ENABLE_FACEDB
#define OASIS_FACE_DB_DIR "faceDB"
#define ENABLE_OUTPUT_DEV_SmartLockConfig
#define ENABLE_INPUT_DEV_PushButtons_VIZNLC
#define ENABLE_OUTPUT_DEV_IrWhiteLeds
#define ENABLE_OUTPUT_DEV_UiSmartlock_VIZNLC
#define ENABLE_OUTPUT_DEV_MqsAudio_VIZNLC
#define ENABLE_INPUT_DEV_BleWuartQn9090
#define ENABLE_INPUT_DEV_ShellUsb
#define ENABLE_LPM_DEV_Standby
#define ENABLE_INPUT_DEV_Lpc845uart
//#define ENABLE_INPUT_DEV_WiFiAWAM510


//wifi board
#define WIFI_IW416_BOARD_MURATA_1XK_M2
/* Murata 1XK */
#if defined(WIFI_IW416_BOARD_MURATA_1XK_M2)
#ifdef ENABLE_INPUT_DEV_WiFiAWAM510
#define WIFI_ENABLED 1
#else
#define WIFI_ENABLED 0
#endif
#define ENABLE_WIFI_CREDENTIALS
// #define WIFI_BT_TX_PWR_LIMITS "wlan_txpwrlimit_cfg_murata_1XK_CA.h"
// #define WIFI_BT_TX_PWR_LIMITS "wlan_txpwrlimit_cfg_murata_1XK_EU.h"
// #define WIFI_BT_TX_PWR_LIMITS "wlan_txpwrlimit_cfg_murata_1XK_JP.h"
// #define WIFI_BT_TX_PWR_LIMITS "wlan_txpwrlimit_cfg_murata_1XK_US.h"
#define WIFI_BT_TX_PWR_LIMITS "wlan_txpwrlimit_cfg_murata_1XK_WW.h"
#define SD8978
#define SDMMCHOST_OPERATION_VOLTAGE_3V3
#define SD_TIMING_MAX kSD_TimingSDR25HighSpeedMode
#define WIFI_BT_USE_M2_INTERFACE
#define WLAN_ED_MAC_CTRL                                                               \
    {                                                                                  \
        .ed_ctrl_2g = 0x1, .ed_offset_2g = 0x0, .ed_ctrl_5g = 0x1, .ed_offset_5g = 0x6 \
    }
#include "wifi_config.h"
#endif /* WIFI_IW416_BOARD_MURATA_1XK_M2 */

/* Memory regions definitions */
#define AT_NONCACHEABLE_SECTION_ALIGN_DTC(var, alignbytes) \
    __attribute__((section(".bss.$SRAM_DTC_cm7,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_CACHEABLE_SECTION_ALIGN_OCRAM(var, alignbytes) \
    __attribute__((section(".bss.$SRAM_OCRAM_CACHED,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION_ALIGN_OCRAM(var, alignbytes) \
    __attribute__((section(".bss.$SRAM_OCRAM_NCACHED,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION_ALIGN_SDRAM(var, alignbytes) \
    __attribute__((section(".bss.$NCACHE_REGION,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))


/* OASIS definitions*/
#define OASIS_RGB_FRAME_WIDTH          480
#define OASIS_RGB_FRAME_HEIGHT         640
#define OASIS_RGB_FRAME_BYTE_PER_PIXEL 3

#define OASIS_IR_FRAME_WIDTH          480
#define OASIS_IR_FRAME_HEIGHT         640
#define OASIS_IR_FRAME_BYTE_PER_PIXEL 3

/* Display definitions*/
#define DISPLAY_LCDIF_BASE       LCDIF
#define DISPLAY_LCDIF_IRQn       LCDIF_IRQn
#define DISPLAY_LCDIF_IRQHandler LCDIF_IRQHandler

/* BLE UART definitions*/
#define BLE_UART_BASE  LPUART5
#define BLE_UART_IRQn  LPUART5_IRQn

/*PWM definitions*/
#define IR_PWM_BASE_ADDR       PWM4
#define IR_PWM_SUBMODULE       kPWM_Module_2
#define IR_PWM_CHANNEL         kPWM_PwmA
#define IR_PWM_CTLMODULE       kPWM_Control_Module_2
#define IR_PWM_FAULTINPUT      kPWM_Fault_1
#define IR_PWM_FAULTCHANNEL    kPWM_faultchannel_1
#define IR_PWM_FAULTDISABLE    kPWM_FaultDisable_1

#define RGB_PWM_BASE_ADDR      PWM4
#define RGB_PWM_SUBMODULE      kPWM_Module_3
#define RGB_PWM_CHANNEL        kPWM_PwmA
#define RGB_PWM_CTLMODULE      kPWM_Control_Module_3
#define RGB_PWM_FAULTINPUT     kPWM_Fault_2
#define RGB_PWM_FAULTCHANNEL   kPWM_faultchannel_0
#define RGB_PWM_FAULTDISABLE   kPWM_FaultDisable_2

#define XBARA1_OUPUT_0    kXBARA1_OutputFlexpwm4Fault0
#define XBARA1_OUPUT_1    kXBARA1_OutputFlexpwm4Fault1
#define XBARA1_OUPUT_2    kXBARA1_OutputFlexpwm1234Fault2
#define XBARA1_OUPUT_3	  kXBARA1_OutputFlexpwm1234Fault3

#define PWM_SRC_CLK_FREQ             CLOCK_GetFreq(kCLOCK_IpgClk)

#endif
