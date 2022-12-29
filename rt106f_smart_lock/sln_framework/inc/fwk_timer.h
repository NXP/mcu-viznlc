/*
 * Copyright 2020-2021 NXP.
 * This software is owned or controlled by NXP and may only be used strictly in accordance with the
 * license terms that accompany it. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you
 * agree to comply with and are bound by, such license terms. If you do not agree to be bound by the
 * applicable license terms, then you may not retain, install, activate or otherwise use the software.
 */

/*
 * @brief Framework timer reference declaration.
 */

#ifndef _FWK_TIMER_H_
#define _FWK_TIMER_H_

#include "timers.h"
#include "semphr.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define TIMER_NAME_LENGTH 32

typedef void (*fwk_timer_callback_t)(void *arg);

typedef struct _fwk_timer
{
    TimerHandle_t handle;
    fwk_timer_callback_t function;
    SemaphoreHandle_t lock;
    char name[TIMER_NAME_LENGTH];
    void *arg;
    int autoReload;
} fwk_timer_t;

/**
 * int FWK_Timer_Start(const char* name, int ms, int autoReload, fwk_timer_callback_t func, void* arg, fwk_timer_t**
 * pPTimer);
 * creates and starts a one time timer. It will malloc memory for the handle(fwk_timer_t*) of
 * the timer inside this function. When the timer expires, it will free memory automatically after
 * callback function. If the timer still active and want to stop it, please call FWK_Timer_Stop
 * @param[in] name A readable text name that is assigned to the timer. This is done to assist debugging.
 * @param[in] ms The period(millisecond) of the timer
 * @param[in] autoReload The auto reload flag of the timer
 * @param[in] func The function to call when the timer expires. Callback functions must have
 *             the prototype defined by fwk_timer_callback_t, which is:
 *             void (*fwk_timer_callback_t)( void* arg );
 * @param[in] arg An argument that will be passed to the callback function
 * @param[out] pPtimer The address pointer where records the pointer of the timer struct
 *             Make sure it is valid during the whole period of the timer
 * @return 0 express success, -1 express fail
 */
int FWK_Timer_Start(
    const char *name, int ms, int autoReload, fwk_timer_callback_t func, void *arg, fwk_timer_t **pPTimer);

/**
 * int FWK_Timer_Reset(fwk_timer_t* timer);
 * re-starts a timer that was previously created using the FWK_Timer_Start() API function
 * @param pPTimer The address pointer records the pointer of the timer struct
 * @return 0 express success, -1 express fail
 */
int FWK_Timer_Reset(fwk_timer_t **pPTimer);

/**
 * int FWK_Timer_Stop(fwk_timer_t* timer);
 * stop and delete a timer that was previously created using the FWK_Timer_Start() API function
 * Inside this function, it will free the memory allocated in FWK_Timer_Start()
 * @param timer The address pointer records the pointer of the timer struct
 * @return 0 express success, -1 express fail
 */
int FWK_Timer_Stop(fwk_timer_t **pPTimer);

/**
 * @brief get the remaining time of a timer until it expires
 * @param pPTimer The address pointer records the pointer of the timer struct
 * @return Number of ticks until expires
 */
uint32_t FWK_Timer_GetRemainingTime(fwk_timer_t **pPTimer);

#if defined(__cplusplus)
}
#endif

#endif /* _FWK_TIMER_H_ */
