/***************************************************************************
 *
 * Add by Jay in 2023.
 * Copyright 2023 CMT.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of CMT.
 *
 * Use of this work is governed by a license granted by CMT.
 * This work contains confidential and proprietary information of
 * CMT. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifdef CMT_008_BLE_ENABLE

#include "tota_ble_custom.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "list.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"
#include "hal_sleep.h"
#include "pmu.h"
#include "audioflinger.h"
#include "apps.h"
#include "app_thread.h"
#include "app_key.h"
#include "bluetooth_bt_api.h"
#include "app_bt_media_manager.h"
#include "app_audio.h"
#include "app_overlay.h"
#include "app_battery.h"
#include "app_trace_rx.h"
#include "app_utils.h"
#include "app_status_ind.h"
#include "bt_drv_interface.h"
#include "besbt.h"
#include "nvrecord_appmode.h"
#include "nvrecord_bt.h"
#include "nvrecord_dev.h"
#include "nvrecord_env.h"
#include "a2dp_api.h"
#include "me_api.h"
#include "btapp.h"
#include "app_bt.h"
#include "app_hfp.h"
#include "bt_if.h"
#include "app_media_player.h"
#include "app_tota_cmd_code.h"
#include "app_tota.h"
#include "app_user.h"

#if defined(IBRT)
#include "app_ibrt_internal.h"
#include "app_ibrt_customif_ui.h"
#include "app_ibrt_voice_report.h"

#if defined(IBRT_UI)
#include "app_tws_ibrt_ui_test.h"
#include "app_ibrt_tws_ext_cmd.h"
#include "app_tws_ibrt_conn_api.h"
#include "app_ibrt_auto_test.h"
#include "app_tws_ctrl_thread.h"
#endif
#include "earbud_ux_api.h"
#endif

#ifdef ANC_APP
#include "app_anc.h"
#endif

bool user_custom_tota_ble_send_response(uint8_t cmdType, uint8_t cmdID, TOTA_BLE_STATUS_E rsp_status, uint8_t* ptrData, uint32_t ptrData_len)
{
	PACKET_STRUCTURE *rsp_ptrData = NULL;
	uint32_t rsp_len;

	if(rsp_status != NO_NEED_STATUS_RESP) {
		rsp_len = sizeof(PACKET_STRUCTURE) + ptrData_len;
	} else{
		rsp_len = sizeof(PACKET_STRUCTURE) - 1 + ptrData_len; //don't need response status
	}

	//TODO: can use alloc here
	uint8_t buffer[100] = {0};
	
	rsp_ptrData = (PACKET_STRUCTURE *)buffer;
	rsp_ptrData->bootCode = BOOTCODE;
	rsp_ptrData->cmdType = cmdType;
	rsp_ptrData->cmdID = cmdID;
	
    if(rsp_status != NO_NEED_STATUS_RESP)
    {
        //Add the first byte in payload for response status
        rsp_ptrData->payload[0] = rsp_status;
		rsp_ptrData->payloadLen = ptrData_len + 1;

		if(ptrData) memcpy(&rsp_ptrData->payload[1], ptrData, ptrData_len);	
    } else{
		//don't need response status
		rsp_ptrData->payloadLen = ptrData_len;
		
		if(ptrData) memcpy(&rsp_ptrData->payload[0], ptrData, ptrData_len);
	}

	return user_custom_app_tota_send_via_datapath((uint8_t *)rsp_ptrData, rsp_len);
}

TOTA_BLE_ANC_MAP anc_map_get_via_anc_mode(app_anc_mode_t mode)
{
	TOTA_BLE_ANC_MAP map = ANC_INVALID;
	
	switch(mode)
	{
		case APP_ANC_MODE_OFF:
			map = ANC_OFF;
		break;

		case APP_ANC_MODE1:
			map = ANC_ON;
		break;

		case APP_ANC_MODE2:
			map = TRANSPARENT;
		break;

		//TODO: APP_ANC_MODE3
		
		default:
			map = ANC_INVALID;
		break;
	}

	return map;
}

static void user_custom_tota_ble_command_set_handle(PACKET_STRUCTURE *ptrPacket)
{
	TOTA_BLE_STATUS_E rsp_status  = SUCCESS_STATUS;

	TOTA_LOG_DBG(0, "[%s] Set CMD:0x%X", __func__, ptrPacket->cmdID);
	
    switch (ptrPacket->cmdID)
    {
		case TOTA_BLE_CMT_COMMAND_SET_NOISE_CANCELLING_MODE_AND_LEVEL:
			/* define Noise Cancelling mode.
             *  +----------------+--------------+
             *  |     NC mode    |   payload[0] |
             *  +----------------+--------------+
             *  |     ANC OFF    |     0x00     |
             *  +----------------+--------------+
             *  | Default-ANC ON |     0x01     |
             *  +----------------+--------------+
             *  |   Transparent  |     0x02     |
             *  +----------------+--------------+
             *  |     MODE 1     |     0x03     |
             *  +----------------+--------------+
             *  |     MODE 2     |     0x04     |
             *  +----------------+--------------+
             */

			switch(ptrPacket->payload[0])
			{
				case ANC_OFF:
					app_ble_anc_switch(APP_ANC_MODE_OFF, true);
					rsp_status = SUCCESS_STATUS;
				break;

				case ANC_ON:
					app_ble_anc_switch(APP_ANC_MODE1, true);
					rsp_status = SUCCESS_STATUS;
				break;

				case TRANSPARENT:
					app_ble_anc_switch(APP_ANC_MODE2, true);
					rsp_status = SUCCESS_STATUS;
				break;
				
				default:
					rsp_status = PARAMETER_ERROR_STATUS;
				break;
			}

			user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_SET, ptrPacket->cmdID, rsp_status, NULL, 0);
        break;
		
    	case TOTA_BLE_CMT_COMMAND_SET_SOUND_PROMPTS_LEVEL:
#ifdef CODEC_DAC_PROMPT_ALONE_VOLUME_TABLE
			if(ptrPacket->payload[0] < TGT_PROMPT_VOL_LEVEL_QTY)
#else
			if(ptrPacket->payload[0] < TGT_VOLUME_LEVEL_QTY)
#endif
			{
				user_custom_set_prompt_volume_level(ptrPacket->payload[0], true);
				rsp_status = SUCCESS_STATUS;
			}
			else{
				rsp_status = PARAMETER_ERROR_STATUS;
			}
			
			user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_SET, ptrPacket->cmdID, rsp_status, NULL, 0);
        break;
		
		case TOTA_BLE_CMT_COMMAND_SET_TOUCH_FUNC_ON_OFF:
			if(ptrPacket->payload[0] == 0x00) {
				user_custom_lock_unlock_touch(false, true);
				rsp_status = SUCCESS_STATUS;
			} else if(ptrPacket->payload[0] == 0x01) {
				user_custom_lock_unlock_touch(true, true);
				rsp_status = SUCCESS_STATUS;
			} else{
				rsp_status = PARAMETER_ERROR_STATUS;
			}
			
            user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_SET, ptrPacket->cmdID, rsp_status, NULL, 0);
        break;
		
		default:
			user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_SET, ptrPacket->cmdID, NOT_SUPPORT_STATUS, NULL, 0);
        break;
    }
}

static void user_custom_tota_ble_command_get_handle(PACKET_STRUCTURE *ptrPacket)
{
	TOTA_BLE_STATUS_E rsp_status;

	TOTA_LOG_DBG(0, "[%s] Get CMD:0x%X", __func__, ptrPacket->cmdID);
	
    switch (ptrPacket->cmdID)
    {
    	case TOTA_BLE_CMT_COMMAND_GET_NOISE_CANCELLING_MODE_AND_LEVEL:
			{
				uint8_t temp[2] = {0};

				temp[0] = anc_map_get_via_anc_mode(app_anc_get_curr_mode());
				temp[1] = 100; //TODO: get Noise Cancelling level via curr_anc_mode
				rsp_status = NO_NEED_STATUS_RESP;
				
				user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_GET, ptrPacket->cmdID, rsp_status, temp, sizeof(temp));
			}
		break;

		case TOTA_BLE_CMT_COMMAND_GET_TOUCH_FUNC_ON_OFF:
        {
            uint8_t temp[1] = {0};
			
            temp[0] = user_custom_is_touch_locked();
            rsp_status = NO_NEED_STATUS_RESP;

			user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_GET, ptrPacket->cmdID, rsp_status, temp, sizeof(temp));
        } 
        break;
		
    	case TOTA_BLE_CMT_COMMAND_GET_SOUND_PROMPTS_LEVEL:
        	{
        		uint8_t temp[1] = {0};
				
        		temp[0] = user_custom_get_prompt_volume_level();
				rsp_status = NO_NEED_STATUS_RESP;

				user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_GET, ptrPacket->cmdID, rsp_status, temp, sizeof(temp));
			}
        break;

		case TOTA_BLE_CMT_COMMAND_GET_BATTERY_LEVEL:
            {
            	uint8_t percent = app_battery_current_level() * 10;
				uint8_t temp[3] = {percent, percent, 0xFF};
				
                rsp_status = NO_NEED_STATUS_RESP;
				
				user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_GET, ptrPacket->cmdID, rsp_status, temp, sizeof(temp));
            }
        break;
		
		default:
			//if the cmd is unsupport, not need to response
        break;
    }
}

// TODO: Jay
void user_custom_tota_ble_data_handle(uint8_t* ptrData, uint32_t length)
{
	PACKET_STRUCTURE *ptrPacket = (PACKET_STRUCTURE *)ptrData;

	if(ptrPacket->bootCode != BOOTCODE)
	{
		TOTA_LOG_DBG(2 ,"[%s] !!!Invalid boot code:0x%x", __func__, ptrPacket->bootCode);
		user_custom_tota_ble_send_response(ptrPacket->cmdType, ptrPacket->cmdID, FORMAT_ERROR_STATUS, NULL, 0);
		return;
	}

	/*check data length and payload*/
	if((ptrPacket->payloadLen != length - DATA_HEAD_SIZE) || (length < DATA_HEAD_SIZE))
	{
		TOTA_LOG_DBG(2 ,"[%s] !!!Error data length:%d", __func__, ptrPacket->payloadLen);
		user_custom_tota_ble_send_response(ptrPacket->cmdType, ptrPacket->cmdID, FORMAT_ERROR_STATUS, NULL, 0);
		return;
	}
		
    switch (ptrPacket->cmdType)
    {
        case TOTA_BLE_CMT_COMMAND_SET:
            TOTA_LOG_DBG(1 ,"[%s] TOTA_BLE_CMT_COMMAND_SET", __func__);
            user_custom_tota_ble_command_set_handle(ptrPacket);
        break;

        case TOTA_BLE_CMT_COMMAND_GET:
            TOTA_LOG_DBG(1 ,"[%s] TOTA_BLE_CMT_COMMAND_GET", __func__);
            user_custom_tota_ble_command_get_handle(ptrPacket);
        break;

        default:
            TOTA_LOG_DBG(1 ,"[%s] !!!Invalid cmd type", __func__);
			user_custom_tota_ble_send_response(ptrPacket->cmdType, ptrPacket->cmdID, FORMAT_ERROR_STATUS, NULL, 0);
			return;
    }
}

void battery_level_change_notify(uint8_t battery_level)
{
	uint8_t percent = battery_level * 10;
	uint8_t temp[3] = {percent, percent, 0xFF};

	TOTA_LOG_DBG(2 ,"%s: level %d", __func__, battery_level);

	user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_NOTIFY, \
		TOTA_BLE_CMT_COMMAND_NOTIFY_BATTERY_LEVEL, NO_NEED_STATUS_RESP, temp, sizeof(temp));
}

void noise_cancelling_mode_change_notify(app_anc_mode_t mode)
{
	uint8_t temp[2] = {0};

	temp[0] = anc_map_get_via_anc_mode(mode);
	temp[1] = 100;//TODO: get Noise Cancelling level via curr_anc_mode

	TOTA_LOG_DBG(2 ,"%s: mode %d", __func__, temp[0]);
	
	user_custom_tota_ble_send_response(TOTA_BLE_CMT_COMMAND_NOTIFY, \
		TOTA_BLE_CMT_COMMAND_NOTIFY_NOISE_CANCELLING_MODE_AND_LEVEL, NO_NEED_STATUS_RESP, temp, sizeof(temp));
}
#endif /* CMT_008_BLE_ENABLE */
