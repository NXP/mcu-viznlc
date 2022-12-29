/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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

#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "board.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

/*******************************************************************************
 * Code
 ******************************************************************************/


/*******************************************************************************
 * Structures and definitions
 ******************************************************************************/

#define ENTRY_MAX_ARGS   3


typedef struct  {
    uint32_t  timestamp;
#if gLoggingWithExtraTs
    uint32_t extra_ts; /* BLE slot count for instance */
 #endif
    const char * function;
    uint8_t   n;
    uint8_t reserved[3];

    const char * format;
    uint32_t  args[ENTRY_MAX_ARGS];
} LogEntry_t;



typedef struct {
    uint16_t read_index;
    uint16_t write_index;
    uint16_t nb_entries;
    uint16_t lock;
    uint16_t entries_max;
    bool blocked_no_wrap;

    LogEntry_t * log_array;
} DbgLog_t ;

#ifndef NOT_USED
#define NOT_USED(a) ((a) = (a))
#endif
/*******************************************************************************
* Local variables
******************************************************************************/

static DbgLog_t log_handle;



/*****************************************************************************
* Local functions
****************************************************************************/
/* Weak */
#ifndef WEAK
#if defined(__IAR_SYSTEMS_ICC__)
#define WEAK __weak
#elif defined(__GNUC__)
#define WEAK __attribute__((weak))
#endif
#endif


WEAK void Dbg_Start32kCounter(void)
{
    return;
}

WEAK uint32_t  Dbg_Get32kTimestamp(void)
{
    return 0UL;
}


/*******************************************************************************
 * Public variables
 *******************************************************************************/

/* Use a variable to loop forever - thus allows to modify the content of this
 * variable by debugger and run on */
volatile int vole = 1;

volatile uint32_t __dwt_count;

/*******************************************************************************
 * Private functions
 *******************************************************************************/
#if	gLoggingActive_d

static void vDbgLogAdd(
#if gLoggingWithExtraTs
                       uint32_t extra_ts,
#endif
                       const char *function,
                       const char *fmt,
                       int n,
                       va_list ap)
{
    DbgLog_t *log = &log_handle;
    LogEntry_t *entry = NULL;

    do {
        if (!log->log_array) break;

        if (log->lock) break;

        if (n >= ENTRY_MAX_ARGS)
        {
            n = ENTRY_MAX_ARGS;
        }

        entry = &log->log_array[log->write_index];
        if (log->nb_entries < log->entries_max)
            log->nb_entries ++;
        else if (log->blocked_no_wrap)
        {
            log->lock = true;
        }
        log->write_index++;
        if (log->write_index == log->entries_max)
        {
            log->write_index = 0;
        }

        entry->timestamp = Dbg_Get32kTimestamp();
#if gLoggingWithExtraTs
        entry->extra_ts = extra_ts;
#endif
        entry->function = (char*)function;
        entry->n = n;
        entry->format = fmt;

        for (int i=0;i<n;i++)
        {
            entry->args[i] = va_arg(ap, uint32_t);
        }

    } while (0);

}
#endif

/*****************************************************************************
 * Public functions
 ****************************************************************************/
void DbgLogSetLock(bool lock)
{
    log_handle.lock = lock;
}

void DbgLogSetWrapPrevention(bool prevent_wrap)
{
    log_handle.nb_entries = 0;
    log_handle.blocked_no_wrap = prevent_wrap;
}

void DbgLogInit(uint32_t log_buffer_addr, uint32_t sz)
{
    if (!log_handle.log_array)
    {
        Dbg_Start32kCounter();
        log_handle.read_index = log_handle.write_index = (uint16_t)0;
        log_handle.nb_entries = (uint16_t)0;
        log_handle.lock = (uint16_t)0;
        log_handle.entries_max = (uint16_t)(sz / sizeof(LogEntry_t));
        if (log_handle.entries_max < 16)
        {
            PRINTF("Log buffer not large enough!\r\n");
            return;
        }

        log_handle.log_array = (LogEntry_t *)log_buffer_addr;
    }
}

void DbgLogGetStartAddrAndSize(uint32_t *addr, uint32_t *sz)
{
	*addr = (uint32_t)&log_handle.log_array[0];
	*sz = log_handle.entries_max * sizeof(LogEntry_t);
}

void DbgLogAdd(const char * function, const char *fmt, int n, ...)
{
#if	gLoggingActive_d
    if (!log_handle.lock)
    {
        va_list args;
        va_start(args,n);
#if gLoggingWithExtraTs
        vDbgLogAdd(0, function, fmt, n, args);
#else
        vDbgLogAdd(function, fmt, n, args);
#endif
        va_end(args);
    }
#else
    NOT_USED(function);
    NOT_USED(fmt);
    NOT_USED(n);
#endif
}

void DbgLogAddExtraTs(const char * function, const char *fmt, uint32_t timestamp, int n, ...)
{
#if	gLoggingActive_d
    if (!log_handle.lock)
    {
        va_list args;
        va_start(args,n);
#if gLoggingWithExtraTs
        vDbgLogAdd(timestamp, function, fmt, n, args);
#else
        vDbgLogAdd(function, fmt, n, args);
        NOT_USED(timestamp);
#endif
        va_end(args);
    }
#else
    NOT_USED(function);
    NOT_USED(fmt);
    NOT_USED(n);
    NOT_USED(timestamp);
#endif
}
#define LOG_ITEM_MAX_SZ  (32+ENTRY_MAX_ARGS*8)

void DbgLogDump(bool stop)
{
#if	gLoggingActive_d

    uint16_t read_index;
    DbgLog_t *log = &log_handle;
    LogEntry_t *entry;
    read_index =  (log->nb_entries == log->entries_max) ?
        log->write_index : log->read_index;
    log_handle.lock = 1;
    static char tab[LOG_ITEM_MAX_SZ];
    memset(tab, 0, LOG_ITEM_MAX_SZ);
    PRINTF("Dumping log time=%d read=%d write=%d\r\n", Dbg_Get32kTimestamp(),
            log->read_index,
            log->write_index);
    while (log->nb_entries > 0)
    {
        entry = &log_handle.log_array[read_index];
#if gLoggingWithExtraTs
        PRINTF("%d:%d: %s", entry->timestamp, entry->extra_ts, entry->function);
#else
        PRINTF("%d: %s", entry->timestamp, entry->function);
#endif

        switch (entry->n)
        {
        case 0:
                if (entry->format[0] != '\0')
                {
                    snprintf(&tab[0], LOG_ITEM_MAX_SZ, (const char*)entry->format);
                    PRINTF(", %s\r\n", tab);
                }
                else
                {
                    PRINTF("\r\n");
                }
            break;
        case 1:
            snprintf(&tab[0], LOG_ITEM_MAX_SZ, (const char*)entry->format, entry->args[0]);
            PRINTF(", %s\r\n", tab);
            break;
        case 2:
            snprintf(&tab[0], LOG_ITEM_MAX_SZ, (const char*)entry->format, entry->args[0], entry->args[1]);
            PRINTF(", %s\r\n", tab);
            break;
        case 3:
            snprintf(&tab[0], LOG_ITEM_MAX_SZ, (const char*)entry->format, entry->args[0], entry->args[1], entry->args[2]);
            PRINTF(", %s\r\n", tab);
            break;
        }
        read_index ++;
        if (read_index == log->entries_max) read_index = 0;
        log->nb_entries --;
    }

    log->read_index = read_index;

    vole = (stop) ? 1 : 0;

    while (vole);
    /* Unlock once consumed */
    log_handle.lock = 0;

#endif

}

