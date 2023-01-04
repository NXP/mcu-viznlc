/*
 * Copyright 2022 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief multicore manager framework declaration.
 */

#ifndef _FWK_MULTICORE_MANAGER_H_
#define _FWK_MULTICORE_MANAGER_H_

#include "hal_multicore_dev.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Init internal structures for Multicore Manager
 * @return int Return 0 if the init process was successful
 */
int FWK_MulticoreManager_Init();

/**
 * @brief Register a Multicore device. Only one multicore device is supported. The dev needs to be registered before
 * FWK_MulticoreManager_Start is called
 * @param dev Pointer to a camera device structure
 * @return int Return 0 if registration was successful
 */
int FWK_MulticoreManager_DeviceRegister(multicore_dev_t *dev);

/**
 * @brief Spawn Multicore manager task which will call init/start for all registered multicore devices
 * @param taskPriority the priority of the Multicore manager task
 * @return int Return 0 if the starting process was successful
 */
int FWK_MulticoreManager_Start(int taskPriority);

int FWK_MulticoreManager_Deinit();

#if defined(__cplusplus)
}
#endif

#endif /*_FWK_MULTICORE_MANAGER_H_*/
