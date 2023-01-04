/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_common.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_power.h"
#include "pin_mux.h"
#include "fsl_swm.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Clock rate on the CLKIN pin */
const uint32_t ExtClockIn = BOARD_EXTCLKINRATE;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* Initialize debug console. */
status_t BOARD_InitDebugConsole(void)
{
#if ((SDK_DEBUGCONSOLE == DEBUGCONSOLE_REDIRECT_TO_SDK) || defined(SDK_DEBUGCONSOLE_UART))
    status_t result;
    /* Select the main clock as source clock of USART0 (debug console) */
    CLOCK_Select(BOARD_DEBUG_USART_CLK_ATTACH);
    RESET_PeripheralReset(BOARD_DEBUG_USART_RST);
    result = DbgConsole_Init(BOARD_DEBUG_USART_INSTANCE, BOARD_DEBUG_USART_BAUDRATE, BOARD_DEBUG_USART_TYPE,
                             BOARD_DEBUG_USART_CLK_FREQ);
    assert(kStatus_Success == result);
    return result;
#else
    return kStatus_Success;
#endif
}

void Board_PullFaceIdPwrCtlPin(bool up)
{
    if (up)
    {
        GPIO_PinWrite(BOARD_INITPINS_SYS_PWR_CTL_GPIO, BOARD_INITPINS_SYS_PWR_CTL_PORT, BOARD_INITPINS_SYS_PWR_CTL_PIN, 1);
    }
    else
    {
        GPIO_PinWrite(BOARD_INITPINS_SYS_PWR_CTL_GPIO, BOARD_INITPINS_SYS_PWR_CTL_PORT, BOARD_INITPINS_SYS_PWR_CTL_PIN, 0);
    }
}

void Board_EnableFaceIDUartTx(void)
{
    CLOCK_EnableClock(kCLOCK_Swm);

    SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_P1_8);

    CLOCK_DisableClock(kCLOCK_Swm);
}

void Board_DisableFaceIDUartTx(void)
{
    CLOCK_EnableClock(kCLOCK_Swm);

    SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_Reset);

    CLOCK_DisableClock(kCLOCK_Swm);
}

void Board_PreEnterLowPower(void)
{
    /* Enable wakeup for PinInt0. */
    EnableDeepSleepIRQ(PIN_INT0_IRQn);
    DisableDeepSleepIRQ(WKT_IRQn);
    POWER_EnableWakeupPinForDeepPowerDown(true, true);

    /* switch main clock source to FRO18M */
    POWER_DisablePD(kPDRUNCFG_PD_FRO_OUT);
    POWER_DisablePD(kPDRUNCFG_PD_FRO);
    CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcFro);
    CLOCK_SetFroOscFreq(kCLOCK_FroOscOut18M);
    CLOCK_SetFroOutClkSrc(kCLOCK_FroSrcFroOsc);

    /* system osc power down
     * application should decide if more part need to power down to achieve better power consumption
     * */
    POWER_EnablePD(kPDRUNCFG_PD_SYSOSC);
    CLOCK_DisableClock(kCLOCK_Iocon);
    CLOCK_DisableClock(kCLOCK_Uart0);
    CLOCK_DisableClock(kCLOCK_Uart1);
    CLOCK_DisableClock(kCLOCK_Gpio0);
    CLOCK_DisableClock(kCLOCK_Gpio1);
    CLOCK_DisableClock(kCLOCK_I2c0);
}

void Board_LowPowerWakeup(void)
{
    /* clock configurations restore */
    BOARD_InitBootClocks();

    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_Uart0);
    CLOCK_EnableClock(kCLOCK_Uart1);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_I2c0);
}
