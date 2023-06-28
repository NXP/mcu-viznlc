/*
 * Copyright (c) 2017 - 2022 , NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include "stdio.h"

#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_usart.h"
#include "fsl_gpio.h"
#include "peripherals.h"
#include "board.h"
#include "app_faceid.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define FACEID_RX_BUFFER_SIZE 256
#define FACEID_TX_BUFFER_SIZE 256

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile uint8_t g_FACEIDCmdStatus  = 0;
static uint8_t g_CommandBuf[FACEID_RX_BUFFER_SIZE];

static uint8_t g_FACEIDRecvBuf[FACEID_RX_BUFFER_SIZE];
static uint8_t g_FACEIDTxBuf[FACEID_TX_BUFFER_SIZE];

static volatile uint16_t g_FACEIDrxIndex = 0; /* Index of the memory to save new arrived data. */
static volatile uint8_t *g_FACEIDCmdCmp  = NULL;

/*******************************************************************************
 * Code
 ******************************************************************************/

void FACEID_USART_IRQHANDLER(void)
{
    uint8_t data;

    if (kUSART_RxReady & USART_GetStatusFlags(FACEID_PERIPHERAL))
    {
        data                               = USART_ReadByte(FACEID_PERIPHERAL);
        g_FACEIDRecvBuf[g_FACEIDrxIndex++] = data;
        if (g_FACEIDrxIndex == FACEID_RX_BUFFER_SIZE)
        {
            g_FACEIDrxIndex = 0;
            memset(g_FACEIDRecvBuf,0,sizeof(g_FACEIDRecvBuf));
        }

        if (g_FACEIDrxIndex >= 2 &&
            ((g_FACEIDRecvBuf[g_FACEIDrxIndex - 2] == '\r' && g_FACEIDRecvBuf[g_FACEIDrxIndex - 1] == '\n') ||
             (g_FACEIDRecvBuf[g_FACEIDrxIndex - 1] == '\r' && g_FACEIDRecvBuf[g_FACEIDrxIndex - 2] == '\n')))
        {
            g_FACEIDCmdStatus = 1;
            memcpy(g_CommandBuf,g_FACEIDRecvBuf,sizeof(g_FACEIDRecvBuf));
            g_FACEIDrxIndex = 0;
            memset(g_FACEIDRecvBuf,0,sizeof(g_FACEIDRecvBuf));
        }
    }
}

/**
 * @brief   FACEID_UARTStringSend
 * @param   str -- string, len -- string length
 * @return  USART Send Status
 */
status_t FACEID_UARTStringSend(char *str, uint32_t len)
{
    return USART_WriteBlocking(FACEID_PERIPHERAL, (const uint8_t *)str, len);
}

/**
 * @brief   Initialize FACEID module
 * @param   rst : 0-not reset module, 1-reset module
 * @return  NULL
 */
void APP_FACEID_Init(void)
{

	DisableIRQ(FACEID_USART_IRQN);
    g_FACEIDCmdStatus = 0;
    memset(g_CommandBuf,0,sizeof(g_CommandBuf));
    EnableIRQ(FACEID_USART_IRQN);


    //please don't move this delay
	{
		// code that you do not want to be optimized
		volatile uint32_t counter = 0x8FFFUL;
		while(counter--);
	}


    Board_PullFaceIdPwrCtlPin(1);
}

static char *strupr(char *str)
{
    char *orign = str;

    for (; *str != '=' && *str != '\0'; str++)
    {
        *str = toupper(*str);
    }

    return orign;
}

/**
 * @brief   FACE ID Tasks Loop
 * @param   NULL
 * @return  FACEID Task Status
 */
uint32_t faceid_task(uint8_t *buf, uint32_t *ret)
{
    strupr((char *)buf);

#if 0
    PRINTF("&&&& ");
    PRINTF("%s\r\n", buf);
#endif

    /**************************** result   **************************************/
    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+PWOFFRSP=ACK");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+PWOFFRSP=ACK\r\n");
        return FACEIDPWROFFACK;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+PWOFFRSP=NACK");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+PWOFFRSP=NACK\r\n");
        return FACEIDPWROFFNACK;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACERES=FAIL");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACERES=FAIL\r\n");
        return FACEIDUNVALIDE;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACERES=");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& %s", g_FACEIDCmdCmp);
        return FACEIDVALIDE;
    }

    /**************************** registration  **************************************/
    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEREG=OK");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&&  AT+FACEREG=OK\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEREG=DUPLICATE");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACEREG=DUPLICATE\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEREG=FAIL");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&&  AT+FACEREG=FAIL\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEREG=");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&&  %s", g_FACEIDCmdCmp);
        return 0;
    }

    /**************************** deregistration/delete  **************************************/
    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEDREG=OK");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACEDREG=OK\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEDEL=SUCCESS");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACEDEL=SUCCESS\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACEDEL=FAIL");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACEDEL=FAIL\r\n");
        return 0;
    }

    /**************************** remote registration  **************************************/
    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACERREG=DUPLICATE");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACERREG=DUPLICATE\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACERREG=OK");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACERREG=OK\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACERREG=FAIL");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&& AT+FACERREG=FAIL\r\n");
        return 0;
    }

    g_FACEIDCmdCmp = (volatile uint8_t *)strstr((const char *)buf, (const char *)"AT+FACERREG=");
    if (g_FACEIDCmdCmp != NULL)
    {
        PRINTF("&&&  %s", g_FACEIDCmdCmp);
        return 0;
    }

    return 0;
}

/**
 * @brief   FACE ID Tasks Loop
 * @param   NULL
 * @return  BLE Task Status
 */
uint8_t APP_FACEID_Task(void)
{
    uint32_t ret = FACEIDIDLE, value;
	DisableIRQ(FACEID_USART_IRQN);

    if (g_FACEIDCmdStatus == 1)
    {
        ret = faceid_task((uint8_t *)g_CommandBuf, &value);
        memset(g_CommandBuf, 0x00, sizeof(g_CommandBuf));
        g_FACEIDCmdStatus = 0;
        EnableIRQ(FACEID_USART_IRQN);
    }
    else
    {
    	//there is no message comes from FaceID module
    	EnableIRQ(FACEID_USART_IRQN);
    	return FACEIDIDLE;
    }


    if (ret == FACEIDVALIDE)
    {
        PRINTF("*** Valid User Face, unlock the door\r\n");
        return FACEIDUNLOCK;
    }else if (ret == FACEIDPWROFFACK)
    {
        PRINTF("*** PWR OFF ACK \r\n");
        return FACEIDPWROFFACK;
    }else if (ret == FACEIDPWROFFNACK)
    {
        PRINTF("*** PWR OFF NACK \r\n");
        return FACEIDPWROFFNACK;
    }else
    {

		/* Task Return Idle Status */
		return FACEIDIDLE;
    }
}

void APP_FACEID_Deinit(void)
{
    Board_PullFaceIdPwrCtlPin(0);

}



status_t APP_FACEID_RequestPowerOff(void)
{
    uint32_t len;
    len = snprintf((char *)g_FACEIDTxBuf, FACEID_TX_BUFFER_SIZE, "AT+PWOFFREQ=\r\n");
    return FACEID_UARTStringSend((char *)g_FACEIDTxBuf, len);
}
