/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC845.h"
#include "fsl_debug_console.h"
#include "app_faceid.h"
#include "fsl_power.h"

#define RT_POWER_ON_DELAY_MS       100
#define RT_POWER_ON_DURATION_MS    30000
#define RT_POWER_OFF_RSP_WAIT_MS   2000
#define RT_POWER_OFF_REQ_MAX_TIMES 5

#define WKT_CLK_FREQ   (CLOCK_GetFreq(kCLOCK_Fro) / 16)

typedef enum
{
    LPC_STATE_POWER_ON,
    LPC_STATE_NORMAL_WORK,
    LPC_STATE_PRE_DEEP_POWER_DOWN,
    LPC_STATE_DEEP_POWER_DOWN,
    LPC_STATE_WAKE_UP,
} lpc_state_t;

static volatile bool is_human_detected = false;
static volatile bool is_wkt_alarmed    = false;
static volatile lpc_state_t lpc_state  = LPC_STATE_POWER_ON;

void pint_interrupt_callback(pint_pin_int_t pintr, uint32_t pmatch_status)
{
    if (GPIO_PinRead(BOARD_INITPINS_PIR_WAKEUP_GPIO, BOARD_INITPINS_PIR_WAKEUP_PORT, BOARD_INITPINS_PIR_WAKEUP_PIN) ==
        0)
    {
        is_human_detected = true;
    }
}

void TIMER_WKT_IRQHANDLER(void)
{
    WKT_ClearStatusFlags(TIMER_PERIPHERAL, kWKT_AlarmFlag);
    is_wkt_alarmed = true;
}

int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();
    Board_DisableFaceIDUartTx();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("\r\n");
    PRINTF("~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    PRINTF("LPC845 Control Start Up!\r\n");
    PRINTF("~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    PRINTF("\r\n");

    while (1)
    {
        switch (lpc_state)
        {
            case LPC_STATE_POWER_ON:
            case LPC_STATE_WAKE_UP:
            {
                PRINTF("@@@ FACEID Power On\r\n");
                APP_FACEID_Init();
                is_human_detected = false;
                is_wkt_alarmed = false;
                WKT_StartTimer(TIMER_PERIPHERAL, MSEC_TO_COUNT(RT_POWER_ON_DELAY_MS, WKT_CLK_FREQ));
                lpc_state = LPC_STATE_NORMAL_WORK;
                break;
            }
            case LPC_STATE_NORMAL_WORK:
            {
                if (is_wkt_alarmed)
                {
                    PRINTF("@@@ FACEID is ready after %d ms delay\r\n", RT_POWER_ON_DELAY_MS);
                    is_wkt_alarmed = false;
                    WKT_StartTimer(TIMER_PERIPHERAL, MSEC_TO_COUNT(RT_POWER_ON_DURATION_MS, WKT_CLK_FREQ));
                    PRINTF("@@@ Start %d ms timer for face rec\r\n", RT_POWER_ON_DURATION_MS);
                    while (!is_wkt_alarmed)
                    {
                        if (APP_FACEID_Task() == FACEIDUNLOCK)
                        {
                            break;
                        }else if (is_human_detected)
                        {
                        	WKT_StopTimer(TIMER_PERIPHERAL);
                        	is_wkt_alarmed = false;
                        	is_human_detected = false;
                        	WKT_StartTimer(TIMER_PERIPHERAL, MSEC_TO_COUNT(RT_POWER_ON_DURATION_MS, WKT_CLK_FREQ));
                        }
                    }

                    lpc_state      = LPC_STATE_PRE_DEEP_POWER_DOWN;

                }
                break;
            }
            case LPC_STATE_PRE_DEEP_POWER_DOWN:
            {
            	bool enter_sleep = false;
            	int prePwdCount = RT_POWER_OFF_REQ_MAX_TIMES;
                uint8_t ret;

            	do{
                    is_wkt_alarmed = false;
                    WKT_StartTimer(TIMER_PERIPHERAL, MSEC_TO_COUNT(RT_POWER_OFF_RSP_WAIT_MS, WKT_CLK_FREQ));
                    Board_EnableFaceIDUartTx();
                    APP_FACEID_RequestPowerOff();
                    Board_DisableFaceIDUartTx();

                    //in fact, if the response is not received immediately, this function always return FACEIDUNVALIDE
                    //give some time for response receiving
                    while(!is_wkt_alarmed)
                    {}

					ret = APP_FACEID_Task();
					if (ret == FACEIDPWROFFACK)
					{
						enter_sleep = true;
						break;
					}
					else if (ret == FACEIDPWROFFNACK)
					{

						prePwdCount = RT_POWER_OFF_REQ_MAX_TIMES;

					}else
					{
						prePwdCount--;
						if (prePwdCount == 0)
						{
							enter_sleep = true;
							break;
						}
					}


            	}while(true);

            	if (enter_sleep)
            	{
            		lpc_state = LPC_STATE_DEEP_POWER_DOWN;
            	}else
            	{
            		lpc_state = LPC_STATE_NORMAL_WORK;
            	}
            	break;


            }
            case LPC_STATE_DEEP_POWER_DOWN:
            {
                PRINTF("@@@ FACEID Power Off\r\n");
                /* power off FACEID */
                APP_FACEID_Deinit();

                PRINTF("LPC enters into deep down power mode.\r\n");
                /* prepare to enter low power mode */
                Board_PreEnterLowPower();
                /* Enter deep power down mode. */
                POWER_EnterDeepPowerDownMode();
                /* Restore the active mode configurations */
                Board_LowPowerWakeup();
                lpc_state = LPC_STATE_WAKE_UP;
                break;
            }
            default:
                break;
        }
    }

    return 0;
}
