/*
 * Copyright 2019, 2021 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "fsl_common.h"
#include "sln_msc_vfs.h"
#include "flash_ica_driver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint32_t s_startOffset = 0;
static uint32_t s_fileLength  = 0;
static uint32_t s_dataWritten = 0;
static uint32_t s_lbaLength   = 0;

static msc_vfs_state_t s_transferState = TRANSFER_IDLE;

static TaskHandle_t *s_usbAppTaskHandle = NULL;

static const char s_BinFileExtensionStr[] = "BIN";

static const fat_mbr_t s_fatMbrInit = {.jump_instr              = {0xEB, 0x3C, 0x90},
                                       .oem_name                = {'M', 'S', 'D', '0', 'S', '5', '.', '0'},
                                       .bytes_per_sector        = 0x0200,
                                       .sectors_per_cluster     = 0x08,
                                       .reserved_sectors        = 0x0008,
                                       .num_fats                = 0x02,
                                       .root_entries            = 0x0200,
                                       .num_logical_sectors     = 0x5000,
                                       .media_type              = 0xf0,
                                       .logical_sectors_per_fat = 0x0008,
                                       .sectors_per_track       = 0x003F,
                                       .num_heads               = 0x00FF,
                                       .hidden_sectors          = 0x00000000,
                                       .large_sectors           = 0x00000000,
                                       .physical_drive_number   = 0x08,
                                       .not_used                = 0x00,
                                       .boot_record_signature   = 0x29,
                                       .volume_sn               = 0x3FF6BD53,
                                       .volume_label = {'S', 'L', 'N', '-', 'B', 'O', 'O', 'T', 0x00, 0x00, 0x00},
                                       .fat_type     = {'F', 'A', 'T', '1', '6', ' ', ' ', ' '},
                                       .bootstrap    = {0x00},
                                       .signature    = 0xAA55};

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t MSC_VFS_Init(uint8_t *storageDisk, TaskHandle_t *usbAppTask, uint32_t lbaLength)
{
    status_t status = kStatus_Fail;

    if ((NULL != storageDisk) && (NULL != usbAppTask) && (0 != lbaLength))
    {
        status = kStatus_Success;

        memcpy(&storageDisk[0], &s_fatMbrInit, sizeof(fat_mbr_t));

        s_usbAppTaskHandle = usbAppTask;

        s_lbaLength = lbaLength;

        s_fileLength    = 0;
        s_dataWritten   = 0;
        s_transferState = TRANSFER_IDLE;
    }

    return status;
}

msc_vfs_state_t MSC_VFS_GetTransferState(void)
{
    return s_transferState;
}

void MSC_VFS_SetTransferState(msc_vfs_state_t transferState)
{
    s_transferState = transferState;
}

status_t MSC_VFS_WriteResponse(uint32_t offset, uint32_t size, uint8_t *buffer)
{
    int32_t flashError = SLN_FLASH_NO_ERROR;
    status_t error     = kStatus_Success;

    if (0 != size)
    {
        if (TRANSFER_IDLE == s_transferState)
        {

			for (uint32_t idx = 0; idx < size; idx++)
			{
				int ret = memcmp(&buffer[idx], s_BinFileExtensionStr,strlen(s_BinFileExtensionStr));
				if (0 == ret &&
					idx >= 8   /*name format is :  8 + 3 chars*/
					&&strlen((char*)&buffer[idx - 8]) >= 11) /*file name must be a string*/
				{
					//file name format is: "***BIN"
					char* cadidate = (char *)(&buffer[idx - 8]);
					fat_file_t *file   = (fat_file_t *)cadidate;
					if (file->size > 0 && file->size <= FICA_IMG_APP_A_SIZE && file->size <= FICA_IMG_APP_B_SIZE)
					{
						//there is no string end in file's name
						char file_name[sizeof(file->name) + 1];
						memcpy(file_name,file->name,sizeof(file->name));
						file_name[sizeof(file->name)] = 0;

						configPRINTF(
							("[Write Response] File Attributes: Name - %s, Size - %d\r\n", (const char *)file_name, file->size));

						s_fileLength = file->size;
						s_transferState = TRANSFER_START;
						break;
					}

				}
			}

        }else if (TRANSFER_START == s_transferState)
        {
            // Determine the base programming address from the passed image reset vector
        	uint32_t *resetHandler = (uint32_t *)(&buffer[4]);

            bool isFlash = ((FLASH_BYTE4_UPPER_NIBBLE & *resetHandler) == FLEXSPI_AMBA_BASE);
            bool isValid = ((FLASH_BYTE3 & *resetHandler) == FICA_IMG_APP_A_ADDR) ||
                           ((FLASH_BYTE3 & *resetHandler) == FICA_IMG_APP_B_ADDR);

            if (isFlash && isValid)
            {
                int32_t currImgType  = FICA_IMG_TYPE_NONE;
                uint32_t imgBaseAddr = (uint32_t)(*resetHandler & FLASH_BYTE3);


                configPRINTF(("[Write Response] Reset Handler: 0x%X\r\n", *resetHandler));

                flashError = FICA_GetImgTypeFromAddr(imgBaseAddr, &currImgType);

                if ((FICA_IMG_TYPE_NONE >= currImgType) || (FICA_NUM_IMG_TYPES <= currImgType))
                {
                    flashError = SLN_FLASH_ERROR;
                }

                if (SLN_FLASH_NO_ERROR == flashError)
                {
                    // Rest of offsets coming in will be relative to the startoffset
                    s_startOffset = offset;

                    // Init FICA to be ready for the new application, erasing on received every packet
                    flashError = FICA_app_program_ext_init(currImgType,0);
                }

                if (SLN_FLASH_NO_ERROR != flashError)
                {
                    error           = kStatus_Fail;
                    s_transferState = TRANSFER_ERROR;
                    configPRINTF(("[Write Response] Unable to begin transfer of file!\r\n"));

                    // Wake up application task to handle this error
                    vTaskResume(*s_usbAppTaskHandle);
                }
                else
                {
                	//this packet also need be programed
                    s_transferState = TRANSFER_ACTIVE;
                    goto write_packet;
                }
            }
        }else if (TRANSFER_ACTIVE == s_transferState)
        {
write_packet:
            if (offset >= s_startOffset)
            {
                uint32_t imgOffset, imgBaseAddr;

                // Calculate the image address for where to store this data
                imgOffset = ((offset - s_startOffset) * s_lbaLength);

                configPRINTF(("[Write Response] Saving %d of data to 0x%X...\r\n", size, imgOffset));

                //erase the area before programming
                assert(imgOffset%EXT_FLASH_ERASE_PAGE == 0);

                FICA_GetNewAppStartAddr(&imgBaseAddr);
                flashError = FICA_Erase_Bank(imgBaseAddr + imgOffset, size);

                if (SLN_FLASH_NO_ERROR == flashError)
                {

					// Write to external flash
					flashError = FICA_app_program_ext_abs(imgOffset, buffer, size);
                }

                if (SLN_FLASH_NO_ERROR != flashError)
                {
                    error           = kStatus_Fail;
                    s_transferState = TRANSFER_ERROR;
                    configPRINTF(("[Write Response] ...save failed!!!\r\n", size, imgOffset));

                    // Wake up application task to handle this error
                    vTaskResume(*s_usbAppTaskHandle);
                }
                else
                {
                    s_dataWritten += size;
                    configPRINTF(("[Write Response] ...data saved!\r\n", size, imgOffset));
                }
            }

            if ((s_fileLength > 0) && (s_dataWritten >= s_fileLength))
            {
                s_transferState = TRANSFER_FINAL;

                // Wake up the application task to finalize transfer
                vTaskResume(*s_usbAppTaskHandle);
            }
        }
    }
    else
    {
        configPRINTF(("[Write Response] Empty write response!\r\n"));
    }
    return error;
}
