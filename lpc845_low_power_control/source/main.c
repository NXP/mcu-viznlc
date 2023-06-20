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
#include "fsl_ctimer.h"

#define RT_POWER_ON_DELAY_MS       100
#define RT_POWER_ON_DURATION_MS    30000
#define RT_POWER_OFF_RSP_WAIT_MS   5000


typedef enum
{
    LPC_STATE_POWER_ON,
    LPC_STATE_NORMAL_WORK,
    LPC_STATE_PRE_DEEP_POWER_DOWN,
    LPC_STATE_DEEP_POWER_DOWN,

} lpc_state_t;


#define CTIMER          CTIMER0         /* Timer 0 */
#define CTIMER_MAT0_OUT kCTIMER_Match_0 /* Match output 0 */
#define CTIMER_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)

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


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void ctimer_match0_callback(uint32_t flags);


/* Array of function pointers for callback for each channel */
ctimer_callback_t ctimer_callback_table[] = {
    ctimer_match0_callback, NULL, NULL, NULL, NULL, NULL, NULL, NULL};


void ctimer_match0_callback(uint32_t flags)
{

    is_wkt_alarmed = true;

}

void CTimer_init()
{
	ctimer_config_t config;
	//CTimer code
	CTIMER_GetDefaultConfig(&config);

	CTIMER_Init(CTIMER, &config);

	CTIMER_RegisterCallBack(CTIMER, &ctimer_callback_table[0], kCTIMER_SingleCallback);

}


void CTimer_start(uint32_t ms)
{

	ctimer_match_config_t matchConfig0;
	/* Configuration 0 */
	matchConfig0.enableCounterReset = true;
	matchConfig0.enableCounterStop  = true;
	//default timeout is 1 second
	matchConfig0.matchValue         = CTIMER_CLK_FREQ/1000 * ms;
	matchConfig0.outControl         = kCTIMER_Output_NoAction;
	matchConfig0.outPinInitState    = false;
	matchConfig0.enableInterrupt    = true;

	//reset the counter
//	CTIMER_StopTimer(CTIMER);
	CTIMER_Reset(CTIMER);
	CTIMER_SetupMatch(CTIMER, CTIMER_MAT0_OUT, &matchConfig0);
	CTIMER_StartTimer(CTIMER);

}



int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();

    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    //disable TX because if impact RT106F power up
    Board_DisableFaceIDUartTx();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("\r\n");
    PRINTF("~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    PRINTF("LPC845 Control Start Up!\r\n");
    PRINTF("~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
    PRINTF("\r\n");

    CTimer_init();

//    lpc_state_t lpc_state  = LPC_STATE_POWER_ON;

    while (1)
    {
        switch (lpc_state)
        {
            case LPC_STATE_POWER_ON:
            {
                PRINTF("@@@ FACEID Power On\r\n");

                APP_FACEID_Init();
                is_human_detected = false;
                is_wkt_alarmed = false;
                CTimer_start(RT_POWER_ON_DELAY_MS);
                while(!is_wkt_alarmed);
                lpc_state = LPC_STATE_NORMAL_WORK;
                break;
            }
            case LPC_STATE_NORMAL_WORK:
            {
                    PRINTF("@@@ FACEID is ready after %d ms delay\r\n", RT_POWER_ON_DELAY_MS);
                    is_wkt_alarmed = false;
                    CTimer_start(RT_POWER_ON_DURATION_MS);
                    PRINTF("@@@ Start %d ms timer for face rec\r\n", RT_POWER_ON_DURATION_MS);
                    while (!is_wkt_alarmed)
                    {
                        if (APP_FACEID_Task() == FACEIDUNLOCK)
                        {
                            break;
                        }else if (is_human_detected)
                        {
                        	CTIMER_StopTimer(CTIMER);
                        	is_wkt_alarmed = false;
                        	is_human_detected = false;
                        	CTimer_start(RT_POWER_ON_DURATION_MS);
                        }
                    }

                    lpc_state      = LPC_STATE_PRE_DEEP_POWER_DOWN;
                break;
            }
            case LPC_STATE_PRE_DEEP_POWER_DOWN:
            {

                //send power off request
                Board_EnableFaceIDUartTx();
                APP_FACEID_RequestPowerOff();
                Board_DisableFaceIDUartTx();

				is_wkt_alarmed = false;
				CTIMER_StopTimer(CTIMER);
				CTimer_start(RT_POWER_OFF_RSP_WAIT_MS);


            	do{

            			//loop the response from
						uint8_t ret = APP_FACEID_Task();
						if (ret == FACEIDPWROFFACK)
						{
							break;
						}
						else
						{
							//for other cases, do nothing
						}

            	}while(!is_wkt_alarmed);

           		lpc_state = LPC_STATE_DEEP_POWER_DOWN;
            	break;


            }
            case LPC_STATE_DEEP_POWER_DOWN:
            {
                PRINTF("@@@ FACEID Power Off\r\n");
                /* power off FACEID */
                APP_FACEID_Deinit();

                if (is_human_detected)
                {
                	is_human_detected = false;

                }else
                {

					PRINTF("LPC enters into deep down power mode.\r\n");
					/* prepare to enter low power mode */
					Board_PreEnterLowPower();
					/* Enter deep power down mode. */
					POWER_EnterDeepPowerDownMode();
					/* Restore the active mode configurations */
					Board_LowPowerWakeup();
                }

                lpc_state = LPC_STATE_POWER_ON;
                break;
            }
            default:
                break;
        }
    }

    return 0;
}
