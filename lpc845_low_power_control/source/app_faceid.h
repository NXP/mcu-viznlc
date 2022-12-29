/*
 * Copyright (c) 2017 - 2022 , NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __APP_FACEID_H__
#define __APP_FACEID_H__

#include "fsl_common.h"

/*******************************************************************************
 * Instructions
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FACEIDTASKIDLE     0x00 /* FACEID Task is IDEL */
#define FACEIDTASKCHECKING 0x01

#define FACEIDUNLOCK       0x00000001 /* FACEID Command is Unlock the door */
#define FACEIDLOCK         0x00000002 /* FACEID Command is Lock the door */
#define FACEIDSTATUSOK     0x00000003
#define FACEIDSTATUSFAILED 0x00000004
#define FACEIDVALIDE       0x00000005 /* FACEID valid */
#define FACEIDUNVALIDE     0x00000006 /* FACEID un-valid */
#define FACEIDPWROFFACK    0x00000007 /* FACEID can be powered off */
#define FACEIDPWROFFNACK   0x00000008 /* FACEID can't be powered off */
#define FACEIDIDLE         0x00000000

#define FACEIDSTATUSIDLE 0xFFFFFFFF

extern void APP_FACEID_Init(void);
extern uint8_t APP_FACEID_Task(void);
extern void APP_FACEID_Deinit(void);
extern uint8_t APP_FACEID_GetDeviceStatus(void);
extern status_t APP_FACEID_RequestPowerOff(void);

#endif /* __APP_FACEID_H__ */
