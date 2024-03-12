/***************************************************************************
 *
 * Copyright 2023 Add by lewis
 * Handle custom application
 * All rights reserved. All unpublished rights reserved.
 *
 ****************************************************************************/

#ifdef CMT_008_SPP_TOTA_V2

#include "tota_spp_custom.h"
#include <stdint.h>
#include "app_tota_cmd_code.h"
#include "app_utils.h"
#include "cmsis_os.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "app_tota_flash_program.h"
#include "app_tota.h"
#include "app_tota_cmd_handler.h"
#include "app_tota_common.h"
#include "app_bt.h"
#include "app_bt_cmd.h"

#include "app_anc.h"
#include "apps.h"
#include "cmsis_os.h"

static AUD_IO_PATH_T mic_select_spp_cmd = AUD_INPUT_PATH_MAINMIC;
static const char* image_info_build_data = "BUILD_DATE=";
extern const char sys_build_info[];

AUD_IO_PATH_T current_select_mic(void)
{
    return mic_select_spp_cmd;
}

bool custom_tota_spp_send_response(APP_TOTA_CMD_CODE_E rsp_opCode, APP_TOTA_CMD_RET_STATUS_E rsp_status, uint8_t* ptrData, uint16_t ptrData_len)
{
	APP_TOTA_CMD_PAYLOAD_T rsp_Data = {0};
	uint16_t rsp_len;

	if(rsp_status != TOTA_CMT_008_NOT_NEED_STATUS) {
		rsp_len = TOTA_DATA_HEAD_SIZE + 1 + ptrData_len;
	} else{
		rsp_len = TOTA_DATA_HEAD_SIZE + ptrData_len; //don't need response status
	}

	rsp_Data.cmdCode = rsp_opCode;
	
	if(rsp_status != TOTA_CMT_008_NOT_NEED_STATUS)
    {
        /* Add the last byte in data for response status. */
		rsp_Data.paramLen = ptrData_len + 1;
		if(ptrData) memcpy(&rsp_Data.param[0], ptrData, ptrData_len);
		rsp_Data.param[rsp_Data.paramLen - 1] = rsp_status;
    } else{
		rsp_Data.paramLen = ptrData_len;
		if(ptrData) memcpy(&rsp_Data.param[0], ptrData, ptrData_len);
	}

	return user_custom_app_tota_send_via_datapath((uint8_t *)&rsp_Data, rsp_len);
}

int32_t find_key_word_me(uint8_t* targetArray, uint32_t targetArrayLen,
    uint8_t* keyWordArray,
    uint32_t keyWordArrayLen)
{
    if ((keyWordArrayLen > 0) && (targetArrayLen >= keyWordArrayLen))
    {
        uint32_t index = 0, targetIndex = 0;
        for (targetIndex = 0;targetIndex < targetArrayLen;targetIndex++)
        {
            for (index = 0;index < keyWordArrayLen;index++)
            {
                if (targetArray[targetIndex + index] != keyWordArray[index])
                {
                    break;
                }
            }

            if (index == keyWordArrayLen)
            {
                return targetIndex;
            }
        }

        return -1;
    }
    else
    {
        return -1;
    }
}

APP_TOTA_CMD_RET_STATUS_E tota_get_buidl_data_me(uint8_t *buildData) 
{
    if(NULL == buildData){
        return TOTA_CMD_HANDLING_FAILED;
    }
    int32_t found = find_key_word_me((uint8_t*)&sys_build_info,
        LEN_OF_ARRAY,
        (uint8_t*)image_info_build_data,
        strlen(image_info_build_data));
    if (-1 == found){
        return TOTA_CMD_HANDLING_FAILED;
    }

    memcpy(buildData, (uint8_t*)&sys_build_info+found+strlen(image_info_build_data), 20);

    TRACE(1,"buildData is 0x%s", buildData);

    return TOTA_NO_ERROR;
}

static void _custom_spp_cmd_handle(APP_TOTA_CMD_CODE_E funcCode, uint8_t* ptrParam, uint32_t paramLen)
{
	TOTA_LOG_DBG(2,"Func code 0x%x, param len %d", funcCode, paramLen);
	TOTA_LOG_DBG(0,"Param content:");
    DUMP8("%02x ", ptrParam, paramLen);
	
	switch(funcCode)
	{
#ifdef BQB_TEST
		case OP_TOTA_CMT008_BQB_TEST_CMD:
			app_bt_cmd_line_handler((char *)ptrParam, paramLen);
		break;
#endif
	
		case OP_TOTA_CMT008_SPP_TEST_CMD:
			if(ptrParam[0] == MIC_SELECT_TEST) 
			{
				switch(ptrParam[1])
				{
                    case TALK_MIC:
                    	{
	                        TOTA_LOG_DBG(0,"TALK MIC");
	                        mic_select_spp_cmd = AUD_INPUT_PATH_MAINMIC;
							uint8_t temp[2] = {MIC_SELECT_TEST, TALK_MIC};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
					break;
					
                    case L_FF_MIC:
						{
	                        TOTA_LOG_DBG(0,"L FF MIC");
                       	    mic_select_spp_cmd = AUD_INPUT_PATH_LFFMIC_SPP;
							uint8_t temp[2] = {MIC_SELECT_TEST, L_FF_MIC};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    case R_FF_MIC:
						{
	                        TOTA_LOG_DBG(0,"R FF MIC");
                        	mic_select_spp_cmd = AUD_INPUT_PATH_RFFMIC_SPP;
							uint8_t temp[2] = {MIC_SELECT_TEST, R_FF_MIC};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}     
                    break;
					
                    case L_FB_MIC:
						{
	                        TOTA_LOG_DBG(0,"L FB MIC");
                      		mic_select_spp_cmd = AUD_INPUT_PATH_LFBMIC_SPP;
							uint8_t temp[2] = {MIC_SELECT_TEST, L_FB_MIC};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    case R_FB_MIC:
						{
	                        TOTA_LOG_DBG(0,"R FB MIC");
                        	mic_select_spp_cmd = AUD_INPUT_PATH_RFBMIC_SPP;
							uint8_t temp[2] = {MIC_SELECT_TEST, R_FB_MIC};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    case MIC_RESET:
						{
	                        TOTA_LOG_DBG(0,"Reset into TALK MIC");
                        	mic_select_spp_cmd = AUD_INPUT_PATH_MAINMIC;
							uint8_t temp[2] = {MIC_SELECT_TEST, MIC_RESET};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    default:
                        TOTA_LOG_DBG(0,"Unsupported command ID of 'Mic select'");
						custom_tota_spp_send_response(funcCode, TOTA_INVALID_CMD, NULL, 0);
                    return;
                }
			}
			else if(ptrParam[0] == ANC_MODE_TEST)
			{
				switch(ptrParam[1])
				{
                    case ANC_MODE_CMD:
						{
	                        TOTA_LOG_DBG(0,"'ANC MODE'");
                       		app_anc_switch(APP_ANC_MODE1);
							uint8_t temp[2] = {ANC_MODE_TEST, ANC_MODE_CMD};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    case AWARENESS_MODE_CMD:
						{
	                        TOTA_LOG_DBG(0,"'Awareness MODE'");
                       	    app_anc_switch(APP_ANC_MODE2);
							uint8_t temp[2] = {ANC_MODE_TEST, AWARENESS_MODE_CMD};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    case ANC_OFF_CMD:
						{
	                        TOTA_LOG_DBG(0,"'ANC OFF'");
                        	app_anc_switch(APP_ANC_MODE_OFF);
							uint8_t temp[2] = {ANC_MODE_TEST, ANC_OFF_CMD};
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
                    	}
                    break;
					
                    default:
                        TOTA_LOG_DBG(0,"Unsupported command ID of 'ANC mode test'");
                        custom_tota_spp_send_response(funcCode, TOTA_INVALID_CMD, NULL, 0);
                    return;
                }
			}
			else if(ptrParam[0] == FUNCTION_TEST)
			{
				switch(ptrParam[1])
				{
                    case POWER_OFF_CMD:
						{
	                        TOTA_LOG_DBG(0,"'Shut down'");
							uint8_t temp[2] = {FUNCTION_TEST, POWER_OFF_CMD};
							/* Send response before asking to shutdown, to ensure response gets sent. */
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
							app_shutdown();
						}                        
                    break;
					
                    case DISCONNECT_BT_CMD:
						{
	                        TOTA_LOG_DBG(0,"'Disconnect BT'");
							uint8_t temp[2] = {FUNCTION_TEST, DISCONNECT_BT_CMD};
							/* Send response before asking to disconnect, to ensure response gets sent. */
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
							osDelay(100);
							app_disconnect_all_bt_connections(false);
						}
                    break;
					
                    case ANC_WIRELESS_DEBUG_EN:
						{
	                        TOTA_LOG_DBG(0,"ANC_WIRELESS_DEBUG_EN");
							uint8_t temp[2] = {FUNCTION_TEST, ANC_WIRELESS_DEBUG_EN};
							//ANC wireless test function always turn on, so do nothing
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
						}
                    break;
					
                    case ANC_WIRELESS_DEBUG_DIS:
						{
	                        TOTA_LOG_DBG(0,"ANC_WIRELESS_DEBUG_DIS");
							uint8_t temp[2] = {FUNCTION_TEST, ANC_WIRELESS_DEBUG_DIS};
							//ANC wireless test function always turn on, so do nothing
							custom_tota_spp_send_response(funcCode, TOTA_NO_ERROR, temp, 2);
						}
                    break;
					
#ifdef CMT_008_SPP_GET_FW
                    case GET_FIRMWARE_VER:
                        {
                            TOTA_LOG_DBG(0,"'Get Firmware version'");
                            uint8_t ver_len = strlen(app_tota_get_fw_version());
							uint8_t temp[20] = {FUNCTION_TEST, GET_FIRMWARE_VER};
							memcpy(&temp[2], (uint8*)app_tota_get_fw_version(), ver_len);
							custom_tota_spp_send_response(funcCode, TOTA_CMT_008_NOT_NEED_STATUS, temp, ver_len + 2);
                        }
                    break;
					
                    case GET_FIRMWARE_VER_BUILD_DATE:
                        {
                            TOTA_LOG_DBG(0,"'Get Firmware version build date'");
							
                            uint8_t build_date[32];
							uint8_t temp[34] = {FUNCTION_TEST, GET_FIRMWARE_VER_BUILD_DATE};
							
                            if(tota_get_buidl_data_me(build_date) == TOTA_NO_ERROR) {
								memcpy(&temp[2], build_date, 32);
								custom_tota_spp_send_response(funcCode, TOTA_CMT_008_NOT_NEED_STATUS, temp, 34);
							} else{
								custom_tota_spp_send_response(funcCode, TOTA_CMD_HANDLING_FAILED, NULL, 0);
							}
						}
                    break;
#endif /*CMT_008_SPP_GET_FW*/

                    default:
                        TOTA_LOG_DBG(0,"Unsupported command ID of 'function test'");
                        custom_tota_spp_send_response(funcCode, TOTA_INVALID_CMD, NULL, 0);
                    return;
                }
			}
		break;
		
		default:	
		break;
	}	
}

TOTA_COMMAND_TO_ADD(OP_TOTA_CMT008_SPP_TEST_CMD, _custom_spp_cmd_handle, false, 0, NULL );
#ifdef BQB_TEST
TOTA_COMMAND_TO_ADD(OP_TOTA_CMT008_BQB_TEST_CMD, _custom_spp_cmd_handle, false, 0, NULL );
#endif

#endif
