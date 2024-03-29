/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include <stdio.h>
#include "hal_trace.h"
#include "norflash_api.h"
#include "customparam_section.h"
#include <string.h>

extern uint32_t __custom_parameter_start[];
/* Add by lewis */
extern uint32_t __custom_parameter_end[];

static const uint32_t nv_custom_parameter_flash_start_addr = (uint32_t)__custom_parameter_start;
static const uint32_t nv_custom_parameter_flash_end_addr =  (uint32_t)__custom_parameter_end;
/* End Add by lewis */

static uint8_t isCustomParamSectionValid = false;

/* Add by lewis */
void _nv_custom_parameter_section_init(void)
{
    uint32_t block_size = 0;
    uint32_t sector_size = 0;
    uint32_t page_size = 0;
    uint32_t buffer_len = 0;
    enum NORFLASH_API_RET_T result;
    enum HAL_FLASH_ID_T flash_id;

    flash_id = norflash_api_get_dev_id_by_addr((uint32_t)__custom_parameter_start);
    hal_norflash_get_size(flash_id, NULL, &block_size, &sector_size, &page_size);
    buffer_len = CUSTOMPARAM_SECTION_SIZE;
	TRACE(4,"%s: start_addr = 0x%x, end_addr = 0x%x, buff_len = %d",
                    __func__,
                    nv_custom_parameter_flash_start_addr,
                    nv_custom_parameter_flash_end_addr,
                    buffer_len);
	
    result = norflash_api_register(
                    NORFLASH_API_MODULE_ID_CUSTOM_PARAMETER,
                    flash_id,
                    nv_custom_parameter_flash_start_addr,
                    nv_custom_parameter_flash_end_addr - nv_custom_parameter_flash_start_addr,
                    block_size,
                    sector_size,
                    page_size,
                    buffer_len,
                    NULL
                    );

    if(result == NORFLASH_API_OK)
    {
        TRACE(0,"%s ok", __func__);
    }
    else
    {
        TRACE(0,"%s failed,result = %d", __func__, result);
    }
}
/* End Add by lewis */

void nv_custom_parameter_section_init(void)
{
    isCustomParamSectionValid = false;

    norflash_api_flash_operation_start((uint32_t) __custom_parameter_start);

	_nv_custom_parameter_section_init(); //Add by lewis
	
    if (CUSTOMPARAM_MAGIC_CODE ==
        ((CUSTOM_PARAM_SECTION_HEADER_T *)__custom_parameter_start)->magic_code)
    {
        // check whether the length is correct
        uint8_t* startLogicAddr = (uint8_t *)__custom_parameter_start;

        CUSTOM_PARAM_SECTION_HEADER_T* pSectoinHeader = 
            (CUSTOM_PARAM_SECTION_HEADER_T *)startLogicAddr;

        uint32_t totalDataLen = OFFSETOF(CUSTOM_PARAM_SECTION_HEADER_T, entryCount) +
            pSectoinHeader->length;

        if (totalDataLen > CUSTOMPARAM_SECTION_SIZE)
        {
            norflash_api_flash_operation_end((uint32_t) __custom_parameter_start);
            return;
        }

        uint32_t offset = sizeof(CUSTOM_PARAM_SECTION_HEADER_T);
        for (uint32_t entry = 0;entry < pSectoinHeader->entryCount;entry++)
        {
            CUSTOM_PARAM_ENTRY_HEADER_T* pEntryHeader = 
                (CUSTOM_PARAM_ENTRY_HEADER_T *)((uint8_t *)__custom_parameter_start + offset);
            offset += (sizeof(CUSTOM_PARAM_ENTRY_HEADER_T) + pEntryHeader->paramLen);
            if (offset > CUSTOMPARAM_SECTION_SIZE)
            {
                norflash_api_flash_operation_end((uint32_t)__custom_parameter_start);
                return;
            }
        }

        isCustomParamSectionValid = true;
        norflash_api_flash_operation_end((uint32_t) __custom_parameter_start);

        uint8_t serialNumber[CUSTOM_PARAM_SERIAL_NUM_LEN];
        uint32_t serialNumParamLen = 0;
        bool isSuccessfullyLoaded = nv_custom_parameter_section_get_entry(CUSTOM_PARAM_SERIAL_NUM_INDEX, serialNumber, 
            &serialNumParamLen);
        if (isSuccessfullyLoaded)
        {
            TRACE(0,"Serial number is:");
            DUMP8("%02x ", serialNumber, sizeof(serialNumber));
        }
    }
    else
    {
        norflash_api_flash_operation_end((uint32_t) __custom_parameter_start);
    }
}

bool nv_custom_parameter_section_get_entry(uint16_t paramIndex, uint8_t* pParamVal, uint32_t* pParamLen)
{
    if (isCustomParamSectionValid)
    {
        norflash_api_flash_operation_start((uint32_t) __custom_parameter_start);
        uint8_t* startLogicAddr = (uint8_t *)__custom_parameter_start;

        CUSTOM_PARAM_SECTION_HEADER_T* pSectoinHeader = 
            (CUSTOM_PARAM_SECTION_HEADER_T *)startLogicAddr;

        uint32_t offset = sizeof(CUSTOM_PARAM_SECTION_HEADER_T);
        for (uint32_t entry = 0;entry < pSectoinHeader->entryCount;entry++)
        {
            CUSTOM_PARAM_ENTRY_HEADER_T* pEntryHeader = 
                (CUSTOM_PARAM_ENTRY_HEADER_T *)((uint8_t *)__custom_parameter_start + offset);
            if (paramIndex == pEntryHeader->paramIndex)
            {
                memcpy(pParamVal, ((uint8_t *)pEntryHeader)+sizeof(CUSTOM_PARAM_ENTRY_HEADER_T), 
                    pEntryHeader->paramLen);
                *pParamLen = pEntryHeader->paramLen;
                norflash_api_flash_operation_end((uint32_t)__custom_parameter_start);
                return true;
            }
            offset += (sizeof(CUSTOM_PARAM_ENTRY_HEADER_T) + pEntryHeader->paramLen);
        }
        norflash_api_flash_operation_end((uint32_t) __custom_parameter_start);
    }

    return false;
}

/* Add by lewis */
int32_t nv_custom_parameter_section_write(uint32_t start_write_addr, const uint8_t* ptrData, uint32_t len)
{
    enum NORFLASH_API_RET_T ret;
    uint32_t i;
	uint32_t nv_custom_parameter_flash_addr_offset = start_write_addr;

    TRACE(2,"%s: start_write_addr:0x%x, len:%d", __func__, start_write_addr, len);
    
    //write by byte to avoid cross flash sector boundary
    for (i = 0; i < len; i++)
    {
        ret = norflash_api_write(NORFLASH_API_MODULE_ID_CUSTOM_PARAMETER,
                    nv_custom_parameter_flash_addr_offset++,
                    ptrData++,
                    1,
                    false);
		
        if(ret != NORFLASH_API_OK)
        {
            TRACE(4,"%s offset:0x%x ret:%d ch:0x%02x",
                            __func__, nv_custom_parameter_flash_addr_offset, ret, *(ptrData-1));
        }
    }
    return 0;
}

bool nv_custom_parameter_section_write_entry(uint16_t paramIndex, uint8_t* pParamVal, uint16_t paramLen)
{
	TRACE(2,"%s paramIndex:%d, len:%d", __func__, paramIndex, paramLen);

    if (isCustomParamSectionValid)
    {		
        uint8_t* startLogicAddr = (uint8_t *)__custom_parameter_start;
        CUSTOM_PARAM_SECTION_HEADER_T* pSectoinHeader = 
            (CUSTOM_PARAM_SECTION_HEADER_T *)startLogicAddr;
        uint32_t offset = sizeof(CUSTOM_PARAM_SECTION_HEADER_T);
		CUSTOM_PARAM_ENTRY_HEADER_T* pEntryHeader = 
			(CUSTOM_PARAM_ENTRY_HEADER_T *)((uint8_t *)__custom_parameter_start + offset);
		uint16_t entry; //entry should be uint16_t, same as pSectoinHeader->entryCount
			
        for (entry = 0; entry < pSectoinHeader->entryCount; entry++)
        {
            if (paramIndex == pEntryHeader->paramIndex)
            {
				if(paramLen != pEntryHeader->paramLen)
				{
					TRACE(2,"!!!paramIndex:%d, error paramLen:%d != %d", paramIndex, paramLen, pEntryHeader->paramLen);
                	return false;
				}
			
                //write paramVal
                nv_custom_parameter_section_write((uint32_t)((uint8_t *)pEntryHeader + 4),   //4: .paramIndex size + .paramLen size
                								   pParamVal, 
                                                   paramLen);
                return true;
            }
			
            offset += (sizeof(CUSTOM_PARAM_ENTRY_HEADER_T) + pEntryHeader->paramLen);
			pEntryHeader = 
				(CUSTOM_PARAM_ENTRY_HEADER_T *)((uint8_t *)__custom_parameter_start + offset);
        }
    }

    return false;
}

/**
  * function: get earphone color whose length is CUSTOM_PARAM_COLOR_LEN
 **/
bool Get_EarphoneColor(uint8_t *earphone_color)
{
	uint8_t color;
	uint32_t colorLen = 0;
	bool ret;

	ret = nv_custom_parameter_section_get_entry(CUSTOM_PARAM_COLOR_INDEX,
                                             &color, &colorLen);
	//make sure that colorLen is equal to CUSTOM_PARAM_COLOR_LEN
	if(ret)
	{
		*earphone_color = color;
		return true;
	}

	return false;
}

/**
  * function: get serial number whose length is CUSTOM_PARAM_SERIAL_NUM_LEN
 **/
bool Get_EarphoneSN(char *earphone_sn, uint32_t *earphone_snLen)
{
	char sn[CUSTOM_PARAM_SERIAL_NUM_LEN] = {0};
	uint32_t snLen = 0;
	bool ret;

	ret = nv_custom_parameter_section_get_entry(CUSTOM_PARAM_SERIAL_NUM_INDEX,
                                             (uint8_t *)sn, &snLen);
	//make sure that snLen is equal to CUSTOM_PARAM_SERIAL_NUM_LEN
	if(ret)
	{
		memcpy(earphone_sn, sn, snLen);
		*earphone_snLen = snLen;
		return true;
	}

	return false;
}
/* End Add by lewis */

#if defined(BISTO_ENABLED)||defined(GFPS_ENABLED)
uint32_t Get_ModelId(void)
{
    uint8_t Model_ID[CUSTOM_PARAM_Model_ID_LEN];
    uint32_t ModelIdValue, Model_ID_ParamLen = 0;
    if(nv_custom_parameter_section_get_entry( CUSTOM_PARAM_Mode_ID_INDEX,
                                             Model_ID, &Model_ID_ParamLen))
    {
        ModelIdValue = (Model_ID[0]<<16) | (Model_ID[1]<<8) | Model_ID[2];
        TRACE(1,"Model id is :0x%08x\n",ModelIdValue);
        return ModelIdValue;
    }
    return 0;
}
#endif
