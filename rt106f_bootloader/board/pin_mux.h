/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_
#include "board_define.h"

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
#if TARGET_BOARD == SLN_VIZNLC_IOT_BOARD
#define BOARD_INITCSIPINS_IOMUXC_GPR_GPR26_GPIO_MUX1_GPIO_SEL_MASK 0x03U /*!< GPIO1 and GPIO6 share same IO MUX function, GPIO_MUX1 selects one GPIO function: affected bits mask */
#define BOARD_INITLCDPINS_IOMUXC_GPR_GPR26_GPIO_MUX1_GPIO_SEL_MASK 0x8000U /*!< GPIO1 and GPIO6 share same IO MUX function, GPIO_MUX1 selects one GPIO function: affected bits mask */
#define BOARD_INITLCDPINS_IOMUXC_GPR_GPR27_GPIO_MUX2_GPIO_SEL_MASK 0x08D00000U /*!< GPIO2 and GPIO7 share same IO MUX function, GPIO_MUX2 selects one GPIO function: affected bits mask */
#define BOARD_INITBUTPINS_IOMUXC_GPR_GPR28_GPIO_MUX3_GPIO_SEL_MASK 0x0CU /*!< GPIO3 and GPIO8 share same IO MUX function, GPIO_MUX3 selects one GPIO function: affected bits mask */
#else
#define BOARD_INITCSIPINS_IOMUXC_GPR_GPR26_GPIO_MUX1_GPIO_SEL_MASK 0x10U /*!< GPIO1 and GPIO6 share same IO MUX function, GPIO_MUX1 selects one GPIO function: affected bits mask */
#define BOARD_INITLCDPINS_IOMUXC_GPR_GPR26_GPIO_MUX1_GPIO_SEL_MASK 0x04U /*!< GPIO1 and GPIO6 share same IO MUX function, GPIO_MUX1 selects one GPIO function: affected bits mask */
#define BOARD_INITLCDPINS_IOMUXC_GPR_GPR27_GPIO_MUX2_GPIO_SEL_MASK 0x80000000U /*!< GPIO2 and GPIO7 share same IO MUX function, GPIO_MUX2 selects one GPIO function: affected bits mask */
#endif

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitDEBUG_UARTPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSDRAMPins(void);


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitCsiCameraPins(void);
void BOARD_InitFlexioCameraPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLCDPins(void);

#if RTVISION_BOARD
void BOARD_InitLPSPI3Pins(void);
void BOARD_InitLPSPI4Pins(void);
void BOARD_InitA71CHPins(void);
void BOARD_InitWifi(void);
void BOARD_InitBluetooth(void);
void BOARD_InitFlash(void);
void BOARD_InitAudio(void);
void BOARD_InitPIRSensorPin(void);
#endif

void BOARD_InitLPUART2Pins(void);
void BOARD_InitLPUART5Pins(void);
void BOARD_InitLPUART6Pins(void);
void BOARD_InitWhiteIrLedPins(void);
void BOARD_InitLedPins(void);
void BOARD_InitButtonPins(void);
void BOARD_InitBLEResetPin(void);
#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
