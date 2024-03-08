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
#include "cmsis_os.h"
#include <string.h>
#include "app_ibrt_keyboard.h"
#include "hal_trace.h"
#include "audio_policy.h"
#include "app_tws_ibrt_cmd_handler.h"
#include "app_tws_ctrl_thread.h"
#include "bluetooth_bt_api.h"
#include "btapp.h"
#include "apps.h"
#include "app_bt.h"
#include "app_tws_besaud.h"
#include "app_tws_ibrt_conn_api.h"
#include "earbud_profiles_api.h"
#include "earbud_ux_api.h"
#include "earbud_ux_duplicate_api.h"
/* Add by lewis */
#include "app_media_player.h"
#include "app_anc.h"
#include "tota_ble_custom.h"

#if (defined(BT_USB_AUDIO_DUAL_MODE) || defined(BTUSB_AUDIO_MODE))
#include "usb_audio.h"
#endif
/* End Add by lewis */
#if BLE_AUDIO_ENABLED
#include "ble_audio_test.h"
#endif
#ifdef __AI_VOICE__
#include "ai_manager.h"
#endif

#ifdef BT_HID_DEVICE
#include "app_bt_hid.h"
#endif

#if defined(IBRT)

#ifdef TILE_DATAPATH
extern "C" void app_tile_key_handler(APP_KEY_STATUS *status, void *param);
#endif

extern void app_otaMode_enter(APP_KEY_STATUS *status, void *param);

struct ibrt_ui_switch_a2dp_usser_action {
    uint8_t action;
    uint32_t trigger_btclk;
};

struct ibrt_if_action_header
{
    uint8_t action;
    bt_bdaddr_t remote;
    uint32_t param;
    uint32_t param2;
} __attribute__ ((packed));

static void app_ibrt_send_switch_a2dp_trigger_tick(uint32_t btclk, uint8_t error_code)
{
    uint8_t *action_data = NULL;
    uint8_t action_length = 0;
    struct ibrt_if_action_header action_header;
    action_header.action = IBRT_ACTION_SWITCH_A2DP;
    action_header.remote = app_bt_get_device(0)->remote;
    action_header.param = btclk;
    action_header.param2 = error_code;
    action_data = (uint8_t *)&action_header;
    action_length = sizeof(action_header);
    if (btclk != 0)
    {
        app_ibrt_conn_send_user_action_v2(action_data, action_length);
    }   
}

static void app_ibrt_ui_switch_streaming_a2dp(struct ibrt_if_action_header *received_header)
{
    uint32_t trigger_btclk = 0;
    if (app_tws_ibrt_tws_link_connected() && tws_besaud_is_connected())
    {
        if (received_header)
        {
            if(received_header->param2 == SWITCH_A2DP_NO_ERROR)
            {
                trigger_btclk = app_bt_audio_trigger_switch_streaming_a2dp(received_header->param);
                if(received_header->param == trigger_btclk)
                {
                    TRACE(0,"switch a2dp ok!"); //slave receive
                }
                else
                {
                    app_ibrt_send_switch_a2dp_trigger_tick(SWITCH_A2DP_RSP_ERROR, SWITCH_A2DP_RSP_ERROR); //slave send
                }
            }
            else if(received_header->param2 == SWITCH_A2DP_RSP_ERROR)
            {
                if(app_bt_manager.a2dp_switch_trigger_device != BT_DEVICE_INVALID_ID)
                {
                    app_bt_manager.trigger_a2dp_switch = false;
                    app_bt_manager.a2dp_switch_trigger_btclk = 0;
                    app_bt_manager.a2dp_switch_trigger_device = BT_DEVICE_INVALID_ID;
                    TRACE(0, "Cancel switch trigger peer not ok!");
                }
                else
                {
                    app_ibrt_send_switch_a2dp_trigger_tick(SWITCH_A2DP_NOW, SWITCH_A2DP_NOW);
                }
            }
            else if(received_header->param2 == SWITCH_A2DP_NOW)
            {
                app_bt_audio_switch_streaming_a2dp();
            }
        }
        else
        {
            trigger_btclk = app_bt_audio_trigger_switch_streaming_a2dp(0);
            app_ibrt_send_switch_a2dp_trigger_tick(trigger_btclk, SWITCH_A2DP_NO_ERROR);
        }
    }
    else
    {
        app_bt_audio_switch_streaming_a2dp();
    }
}

void app_ibrt_keyboard_start_perform_a2dp_switching(void)
{
    app_ibrt_ui_switch_streaming_a2dp(NULL);
}

#ifdef APP_KEY_ENABLE
void app_ibrt_handle_longpress_v2(APP_KEY_STATUS *status)
{
    uint8_t playing_sco = app_bt_audio_get_curr_playing_sco();
    uint8_t sco_count = app_bt_audio_count_connected_sco();
    uint8_t a2dp_count = app_bt_audio_count_streaming_a2dp();

    // switch between sco streaming devices
    if (playing_sco != BT_DEVICE_INVALID_ID && (sco_count > 1 || app_bt_audio_select_another_device_to_create_sco(playing_sco) != BT_DEVICE_INVALID_ID))
    {
        app_ibrt_if_switch_streaming_sco();
        return;
    }

    // switch between a2dp streaming devices
    if (playing_sco == BT_DEVICE_INVALID_ID && sco_count == 0 && a2dp_count > 1)
    {
        app_ibrt_keyboard_start_perform_a2dp_switching();
        return;
    }
}

#ifdef IBRT_SEARCH_UI
void app_ibrt_search_ui_handle_key_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
    ibrt_ctrl_t *p_ibrt_ctrl = app_ibrt_if_get_bt_ctrl_ctx();

    TRACE(3,"%s %d,%d",__func__, status->code, status->event);


    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        switch(status->event)
        {
            case APP_KEY_EVENT_CLICK:
                app_ibrt_middleware_handle_click();
                break;

            case APP_KEY_EVENT_DOUBLECLICK:
                TRACE(0,"double kill");
                if(IBRT_UNKNOW==p_ibrt_ctrl->nv_role)
                {
                    app_ibrt_if_init_open_box_state_for_evb();
                    app_start_tws_serching_direactly();
                }
                else
                {
                    bt_key_handle_func_doubleclick();
                }
                break;

            case APP_KEY_EVENT_LONGPRESS:
                break;

            case APP_KEY_EVENT_TRIPLECLICK:
            #ifdef TILE_DATAPATH
                app_tile_key_handler(status,NULL);
            #else
                app_otaMode_enter(NULL,NULL);
            #endif
                break;
            case HAL_KEY_EVENT_LONGLONGPRESS:
                TRACE(0,"long long press");
                app_reset();
                break;

            case APP_KEY_EVENT_ULTRACLICK:
                TRACE(0,"ultra kill");
                break;

            case APP_KEY_EVENT_RAMPAGECLICK:
                TRACE(0,"rampage kill!you are crazy!");
                break;

            case APP_KEY_EVENT_UP:
                break;
        }
    }

#ifdef TILE_DATAPATH
    if(APP_KEY_CODE_TILE == status->code)
        app_tile_key_handler(status,NULL);
#endif
}
#else
void app_ibrt_normal_ui_handle_key_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
    uint8_t conn_devices = 0;
    if (APP_KEY_CODE_GOOGLE != status->code)
    {
        switch(status->event)
        {
            case APP_KEY_EVENT_CLICK:
                TRACE(0,"first blood.");
                app_ibrt_middleware_handle_click();
                break;

            case APP_KEY_EVENT_DOUBLECLICK:
                TRACE(0,"double kill,enter freeman mode");
            //    app_ibrt_if_init_open_box_state_for_evb();
                app_ibrt_internal_enter_freeman_pairing();
                break;
            case APP_KEY_EVENT_LONGPRESS:
                conn_devices = app_bt_count_connected_device();
                TRACE(2,"%s conn_devices %d", __func__, conn_devices);
                if (conn_devices > 0)
                {
                    app_ibrt_handle_longpress_v2(status);
                }
                else
                {
                    app_ibrt_if_init_open_box_state_for_evb();
                    app_ibrt_if_enter_pairing_after_tws_connected();
                }
                break;

            case APP_KEY_EVENT_TRIPLECLICK:
#ifdef TILE_DATAPATH
                app_tile_key_handler(status,NULL);
#else
                app_ibrt_if_init_open_box_state_for_evb();
                app_ibrt_if_enter_pairing_after_tws_connected();
#endif
                break;

            case HAL_KEY_EVENT_LONGLONGPRESS:
                TRACE(0,"long long press");
                app_reset();
                break;

            case APP_KEY_EVENT_ULTRACLICK:
                TRACE(0,"ultra kill");
                break;

            case APP_KEY_EVENT_RAMPAGECLICK:
                TRACE(0,"rampage kill!you are crazy!");
                break;

            case APP_KEY_EVENT_UP:
                break;
        }
    }

#ifdef TILE_DATAPATH
    if(APP_KEY_CODE_TILE == status->code)
        app_tile_key_handler(status,NULL);
#endif
}
#endif

struct ibrt_keyboard_notify_v2_t
{
    bt_bdaddr_t remote;
    APP_KEY_STATUS key_status;
};

int app_ibrt_if_keyboard_notify_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
    struct ibrt_keyboard_notify_v2_t data;
    if (remote && APP_IBRT_SLAVE_IBRT_LINK_CONNECTED(remote)) {
        data.remote = *remote;
    }
    else {
        data.remote = {0};
    }

    data.key_status = *status;
    tws_ctrl_send_cmd(APP_TWS_CMD_KEYBOARD_REQUEST, (uint8_t *)&data, sizeof(struct ibrt_keyboard_notify_v2_t));

    return 0;
}
#endif


void app_ibrt_keyboard_request_handler_v2(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length)
{
#ifdef APP_KEY_ENABLE

    struct ibrt_keyboard_notify_v2_t *req = (struct ibrt_keyboard_notify_v2_t *)p_buff;

    if (app_tws_ibrt_mobile_link_connected(&req->remote))
    {
#ifdef IBRT_SEARCH_UI
        app_ibrt_search_ui_handle_key_v2(&req->remote, &req->key_status, NULL);
#else
        app_ibrt_normal_ui_handle_key_v2(&req->remote, &req->key_status, NULL);
#endif
    }
#endif
}

#ifdef TOTA_v2
#define IBRT_TOTA_DAT_MAX_SIZE 200
uint8_t g_tota_data_action[IBRT_TOTA_DAT_MAX_SIZE];
bool app_spp_tota_send_data(uint8_t* ptrData, uint16_t length);
#endif

#ifdef BT_HID_DEVICE
struct bt_hid_sensor_data_t {
    struct ibrt_if_action_header header;
    struct bt_hid_sensor_report_t report;
};
#endif

void app_ibrt_if_start_user_action_v2(uint8_t device_id, uint8_t action, uint32_t param, uint32_t param2)
{
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);
    uint16_t action_length = 0;
    uint8_t *action_data = NULL;
    struct ibrt_if_action_header action_header;
#ifdef BT_HID_DEVICE
    struct bt_hid_sensor_data_t sensor_data;
#endif

    action_header.action = action;
    action_header.remote = curr_device->remote;
    action_header.param = param;
    action_header.param2 = param2;

    action_data = (uint8_t *)&action_header;
    action_length = sizeof(action_header);

#ifdef TOTA_v2
    if (action == IBRT_ACTION_SEND_TOTA_DATA)
    {
        memcpy(g_tota_data_action, (uint8_t *)&action_header, action_length);
        if (action_length + param2 > IBRT_TOTA_DAT_MAX_SIZE)
        {
            TRACE(1, "send tota data length too long %d", param2);
        }
        memcpy(g_tota_data_action+action_length, (void*)(uintptr_t)param, param2);
        action_data = g_tota_data_action;
        action_length += param2;
    }
#endif

#ifdef BT_HID_DEVICE
    if (action == IBRT_ACTION_HID_SENSOR_REPORT)
    {
        sensor_data.header = action_header;
        sensor_data.report = *((struct bt_hid_sensor_report_t *)(uintptr_t)param);
        action_data = (uint8_t *)&sensor_data;
        action_length = sizeof(sensor_data);
    }
#endif

    if (tws_besaud_is_connected() && IBRT_SLAVE == app_tws_get_ibrt_role(&curr_device->remote))
    {
        app_ibrt_conn_send_user_action_v2(action_data, action_length);
    }
    else
    {
        app_ibrt_ui_perform_user_action_v2(action_data, action_length);
    }
}

void app_ibrt_keyboard_sync_volume_info_v2(uint8_t device_id)
{
    struct BT_DEVICE_T *curr_device = app_bt_get_device(device_id);

    if(curr_device && curr_device->acl_is_connected && app_tws_ibrt_tws_link_connected())
    {
        TWS_VOLUME_SYNC_INFO_T_V2 volume_info;

        volume_info.remote = curr_device->remote;

        volume_info.a2dp_local_volume = a2dp_volume_local_get(device_id);

        volume_info.hfp_local_volume = hfp_volume_local_get(device_id);

        TRACE(4, "(d%x) %s: a2dp %d hfp %d", device_id, __func__, volume_info.a2dp_local_volume, volume_info.hfp_local_volume);

        tws_ctrl_send_cmd(APP_TWS_CMD_SYNC_VOLUME_INFO, (uint8_t*)&volume_info, sizeof(volume_info));
    }
}

void app_ibrt_ui_perform_user_action_v2(uint8_t *p_buff, uint16_t length)
{
    struct ibrt_if_action_header *action_header = (struct ibrt_if_action_header *)p_buff;
    uint8_t action = action_header->action;
    uint8_t device_id = app_bt_get_device_id_byaddr(&action_header->remote);
    uint32_t param = action_header->param;
    uint32_t param2 = action_header->param2;
    struct BT_DEVICE_T *curr_device = NULL;

    if (device_id == BT_DEVICE_INVALID_ID)
    {
        TRACE(1, "%s invalid device id", __func__);
        return;
    }

    curr_device = app_bt_get_device(device_id);

    switch (action)
    {
        case IBRT_ACTION_PLAY:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_PLAY);
            break;
        case IBRT_ACTION_PAUSE:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_PAUSE);
            break;
        case IBRT_ACTION_FORWARD:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_FORWARD);
            break;
        case IBRT_ACTION_BACKWARD:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_BACKWARD);
            break;
        case IBRT_ACTION_AVRCP_VOLUP:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_VOLUME_UP);
            break;
        case IBRT_ACTION_AVRCP_VOLDN:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_VOLUME_DOWN);
            break;
        case IBRT_ACTION_AVRCP_ABS_VOL:
            app_bt_a2dp_send_set_abs_volume(device_id, param & 0x7f);
            break;
        case IBRT_ACTION_HFSCO_CREATE:
            if (curr_device->hf_audio_state == BTIF_HF_AUDIO_CON)
            {
                TRACE(1,"%s cannot create audio link\n", __func__);
            }
            else
            {
                btif_hf_create_audio_link(curr_device->hf_channel);
            }
            break;
        case IBRT_ACTION_HFSCO_DISC:
            if (curr_device->hf_audio_state == BTIF_HF_AUDIO_CON)
            {
                btif_hf_disc_audio_link(curr_device->hf_channel);
            }
            else
            {
                TRACE(1,"%s cannot disc audio link\n", __func__);
            }
            break;
        case IBRT_ACTION_REDIAL:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_redial_call(curr_device->hf_channel);
            }
            else
            {
                TRACE(1,"%s cannot redial call\n", __func__);
            }
            break;
        case IBRT_ACTION_ANSWER:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_answer_call(curr_device->hf_channel);
            }
            else
            {
                TRACE(1,"%s cannot answer call\n", __func__);
            }
            break;
        case IBRT_ACTION_HANGUP:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_hang_up_call(curr_device->hf_channel);
            }
            else
            {
                TRACE(1,"%s cannot hangup call\n", __func__);
            }
            break;
        case IBRT_ACTION_LOCAL_VOLUP:
            app_bt_local_volume_up(app_ibrt_keyboard_sync_volume_info_v2);
            break;
        case IBRT_ACTION_LOCAL_VOLDN:
            app_bt_local_volume_down(app_ibrt_keyboard_sync_volume_info_v2);
            break;
        case IBRT_ACTION_SWITCH_A2DP:
            app_ibrt_ui_switch_streaming_a2dp((struct ibrt_if_action_header *)p_buff);
            break;
        case IBRT_ACTION_SWITCH_SCO:
            app_bt_audio_switch_streaming_sco();
            break;
        case IBRT_ACTION_TELL_MASTER_CONN_PROFILE:
            app_ibrt_internal_profile_connect(device_id, (int)param, param2);
            break;
        case IBRT_ACTION_TELL_MASTER_DISC_PROFILE:
            app_ibrt_internal_profile_disconnect(device_id, param);
            break;
        case IBRT_ACTION_TELL_MASTER_DISC_RFCOMM:
            app_ibrt_internal_disonnect_rfcomm((bt_spp_channel_t *)param, (uint8_t)param2);
            break;
#ifdef BT_HID_DEVICE
        case IBRT_ACTION_HID_SEND_CAPTURE:
            app_bt_hid_capture_process(&curr_device->remote);
            break;
        case IBRT_ACTION_HID_SENSOR_REPORT:
            app_bt_hid_send_sensor_report(&curr_device->remote, &((struct bt_hid_sensor_data_t *)p_buff)->report);
            break;
#endif
#ifdef TOTA_v2
        case IBRT_ACTION_SEND_TOTA_DATA:
            app_spp_tota_send_data((uint8_t*)p_buff+sizeof(struct ibrt_if_action_header), length-sizeof(struct ibrt_if_action_header));
            break;
#endif
        case IBRT_ACTION_HOLD_ACTIVE_CALL:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_call_hold(curr_device->hf_channel, BTIF_HF_HOLD_HOLD_ACTIVE_CALLS, 0);
            }
            break;
        case IBRT_ACTION_3WAY_HUNGUP_INCOMING:
        case IBRT_ACTION_RELEASE_HOLD_CALL:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_call_hold(curr_device->hf_channel, BTIF_HF_HOLD_RELEASE_ACTIVE_CALLS, 0);
            }
            break;
        case IBRT_ACTION_HOLD_SWITCH:
            app_ibrt_if_hold_background_switch();
            break;
        case IBRT_ACTION_RELEASE_ACTIVE_ACCEPT_ANOTHER:
            if (curr_device->hf_conn_flag)
            {
                btif_hf_call_hold(curr_device->hf_channel, BTIF_HF_HOLD_RELEASE_ACTIVE_CALLS, 0);
            }
            break;
        case IBRT_ACTION_AVRCP_FAST_FORWARD_START:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_FAST_FORWARD_START);
            break;
         case IBRT_ACTION_AVRCP_FAST_FORWARD_STOP:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_FAST_FORWARD_STOP);
            break;
         case IBRT_ACTION_AVRCP_REWIND_START:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_REWIND_START);
            break;
         case IBRT_ACTION_AVRCP_REWIND_STOP:
            app_bt_a2dp_send_key_request(device_id, AVRCP_KEY_REWIND_STOP);
            break;
        default:
            TRACE(2,"%s unknown user action %d\n", __func__, action);
            break;
    }
}

#endif

/* Add by lewis */
#ifdef CMT_008_UI

#ifdef SUPPORT_SIRI
extern int open_siri_flag;
#endif
extern struct BT_DEVICE_MANAGER_T app_bt_manager;

extern HFCALL_MACHINE_ENUM app_get_hfcall_machine(void);
#if defined(ANC_APP)
extern void app_anc_key(APP_KEY_STATUS *status, void *param);
#endif

void app_ibrt_ui_dynamic_handle_key_function(TOTA_BLE_KET_FUN_MAP key_func)
{
	POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());
	btif_hf_call_setup_t is_call_setup = btapp_hfp_get_call_setup();
	btif_hf_call_active_t is_call_active = btapp_hfp_is_call_active();

	TRACE(0, "%s key_func: %d", __func__, key_func);

	switch(key_func)
	{
		case BLE_KEY_FUN_MAP_NONE:
			TRACE(0, "key func is none");
		break;

		case BLE_KEY_FUN_MAP_PLAYPAUSE:
			TRACE(0, "key func is music play/pause");
			if (btapp_hfp_is_sco_active() || btapp_hfp_get_call_active())
			{
				TRACE(0, "now is calling, ignore");
				return;
			}
			if(a2dp_device && (a2dp_device->a2dp_play_pause_flag == 0)){
				a2dp_handleKey(AVRCP_KEY_PLAY);
			}else{
				a2dp_handleKey(AVRCP_KEY_PAUSE);
			}
		break;

		case BLE_KEY_FUN_MAP_PRE_SONG:
			TRACE(0, "key func is switch previous song");
			if (btapp_hfp_is_sco_active() || btapp_hfp_get_call_active())
			{
				TRACE(0, "now is calling, ignore");
				return;
			}
			a2dp_handleKey(AVRCP_KEY_BACKWARD);
		break;

		case BLE_KEY_FUN_MAP_NEXT_SONG:
			TRACE(0, "key func is switch next song");
			if (btapp_hfp_is_sco_active() || btapp_hfp_get_call_active())
			{
				TRACE(0, "now is calling, ignore");
				return;
			}
			a2dp_handleKey(AVRCP_KEY_FORWARD);
		break;

		case BLE_KEY_FUN_MAP_VA:
			TRACE(0, "key func is trigger/exit VA");
			if(is_call_setup || is_call_active) {
				TRACE(0, "now is calling, ignore");
				return;
			}
			
			TRACE(0, "now call is idle, process VA");
			
			if(!user_custom_is_VA_control_on())
			{
				TRACE(0,"VA control is off, ignore");
				return;
			}
			
#ifdef SUPPORT_SIRI
			if(open_siri_flag == 1){
				TRACE(0,"close siri");
				app_hfp_siri_voice(false);
			} 
			else{
				TRACE(0,"open siri");
				app_hfp_siri_voice(true);
			}
#endif	
		break;

		case BLE_KEY_FUN_MAP_VOL_UP:
			TRACE(0, "key func is volume up");
			app_bt_volumeup();
		break;

		case BLE_KEY_FUN_MAP_VOL_DOWN:
			TRACE(0, "key func is volume down");
			app_bt_volumedown();
		break;

		case BLE_KEY_FUN_MAP_GAME_MODE:
			key_low_latency_mode_switch(true);
		break;
		
		case BLE_KEY_FUN_MAP_ANC:
			TRACE(0, "key func is ANC");
			app_anc_key(NULL, NULL);
		break;
		
		default:
			TRACE(0, "!!!warning, undefined key function");
		break;
	}
}

osTimerId mic_mute_sw_timer = NULL;
static void mic_mute_swtimer_handler(void const *param);
osTimerDef(MIC_MUTE_TIMER, mic_mute_swtimer_handler);// define timers
#define MIC_MUTE_SWTIMER_MS	(5000)

static void mic_mute_swtimer_handler(void const *param)
{
	if(app_bt_manager.hf_tx_mute_flag)
	{
		media_PlayAudio(AUD_ID_BT_SINGLE_BEEP_TONE, 0);
	} else{
		osTimerStop(mic_mute_sw_timer);
		osTimerDelete(mic_mute_sw_timer);
	}
}

void app_mic_mute_swtimer_start(void)
{
	TRACE(0,"%s",__func__);
	
	if(mic_mute_sw_timer == NULL)
		mic_mute_sw_timer = osTimerCreate(osTimer(MIC_MUTE_TIMER), osTimerPeriodic, NULL);

	osTimerStop(mic_mute_sw_timer);
	osTimerStart(mic_mute_sw_timer,MIC_MUTE_SWTIMER_MS);
}

void app_mic_mute_swtimer_stop(void)
{
	TRACE(0,"%s",__func__);

	if(mic_mute_sw_timer == NULL)
		return;
	
	osTimerStop(mic_mute_sw_timer);
}

void app_ibrt_ui_handle_pwr_key(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
	TRACE(0, "%s key event: %d", __func__, status->event);
	
	switch(status->event)
    {
        case APP_KEY_EVENT_DOUBLECLICK:
			if(app_bt_count_connected_device()) {
				TRACE(0, "BT disconnect by headset end");
				app_disconnect_all_bt_connections(false);
			} else{
				TRACE(0, "BT reconnect by headset end");
				app_ibrt_if_event_entry(APP_UI_EV_DOCK);
                app_ibrt_if_event_entry(APP_UI_EV_CASE_CLOSE);

                //app_ibrt_if_event_entry(APP_UI_EV_CASE_OPEN);
                //app_ibrt_if_event_entry(APP_UI_EV_UNDOCK);
                app_bt_profile_connect_manager_opening_reconnect();
			}
		break;

		case APP_KEY_EVENT_LONGLONGPRESS:
			app_shutdown();
		break;
		
        default:
			TRACE(2,"%s Invalid key event", __func__);
        break;
    }
}

void app_ibrt_ui_handle_VA_key(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
	TRACE(0, "%s key event: %d", __func__, status->event);

	//CALL_STATE_E call_state = app_bt_get_call_state();
	btif_hf_call_setup_t is_call_setup = btapp_hfp_get_call_setup();
	btif_hf_call_active_t is_call_active = btapp_hfp_is_call_active();
	
	//if(call_state == CALL_STATE_IDLE) {
	TRACE(0, "is call active: %d, is call setup: %d", is_call_active, is_call_setup);
	if(is_call_setup || is_call_active) {
		TRACE(0, "now is calling, process MIC MUTE/CLEAR MUTE");
		if(app_bt_manager.hf_tx_mute_flag == 0)
        {
            TRACE(0,"MUTE");
            hfp_handle_key(HFP_KEY_MUTE);
			media_PlayAudio(AUD_ID_BT_MIC_MUTE, 0);
			app_mic_mute_swtimer_start();
        }
        else
        {
            TRACE(0,"CLEAR MUTE");
            hfp_handle_key(HFP_KEY_CLEAR_MUTE);
			media_PlayAudio(AUD_ID_BT_MIC_UNMUTE, 0);
			app_mic_mute_swtimer_stop();
        }
	} else{
		TRACE(0, "now call is idle, process VA");
		
		if(!user_custom_is_VA_control_on())
		{
			TRACE(0,"VA control is off, ignore");
			return;
		}
		
#ifdef SUPPORT_SIRI
		if(open_siri_flag == 1){
			TRACE(0,"close siri");
			app_hfp_siri_voice(false);
		} 
		else{
			TRACE(0,"open siri");
			app_hfp_siri_voice(true);
		}
#endif	
	}
}
#endif

#ifdef CMT_008_CST820_TOUCH
void app_ibrt_ui_handle_touch_key_double_click(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
    POSSIBLY_UNUSED struct BT_DEVICE_T* a2dp_device = app_bt_get_device(app_bt_audio_get_curr_a2dp_device());

	switch(hfcall_machine)
    {
		case HFCALL_MACHINE_CURRENT_IDLE:
		case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
			if(a2dp_device && (a2dp_device->a2dp_play_pause_flag == 0)){
                a2dp_handleKey(AVRCP_KEY_PLAY);
            }else{
                a2dp_handleKey(AVRCP_KEY_PAUSE);
            }
		break;

		case HFCALL_MACHINE_CURRENT_INCOMMING:
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_ANSWER_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING:
		case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

		case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING:
        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;
		
   		default:
        break;
	}
}

void app_ibrt_ui_handle_touch_key_longpress(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();

	switch(hfcall_machine)
    {
		case HFCALL_MACHINE_CURRENT_IDLE:
		case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
			//app_switch_to_quick_conversation_mode();
		break;
	
        case HFCALL_MACHINE_CURRENT_INCOMMING:
		case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_HANGUP_CALL);
		break;

		case HFCALL_MACHINE_CURRENT_OUTGOING:
		case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_HANGUP_CALL);
		break;

		case HFCALL_MACHINE_CURRENT_CALLING:
		case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_HANGUP_CALL);
		break;
		
		case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING:
		case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_THREEWAY_HOLD_REL_INCOMING);
		break;

		case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING:
        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
		break;

        default:
        break;
    }
}

void app_ibrt_ui_handle_touch_key(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
	TRACE(0, "%s key event: %d", __func__, status->event);
	
	switch(status->event)
    {
        case APP_KEY_EVENT_SLIDE_LEFT:
            if (btapp_hfp_is_sco_active() || btapp_hfp_get_call_active())
            {
				TRACE(0, "now is calling, ignore");
				return;
			}
			a2dp_handleKey(AVRCP_KEY_BACKWARD);
        break;
			
		case APP_KEY_EVENT_SLIDE_RIGHT:
			if (btapp_hfp_is_sco_active() || btapp_hfp_get_call_active())
            {
				TRACE(0, "now is calling, ignore");
				return;
			}
			a2dp_handleKey(AVRCP_KEY_FORWARD);
		break;

		case APP_KEY_EVENT_SLIDE_UP:
		case APP_KEY_EVENT_SLIDE_UP_AND_HOLD:
			app_bt_volumeup();
		break;

		case APP_KEY_EVENT_SLIDE_DOWN:
		case APP_KEY_EVENT_SLIDE_DOWN_AND_HOLD:
			app_bt_volumedown();
		break;
		
		case APP_KEY_EVENT_DOUBLECLICK:
			app_ibrt_ui_handle_touch_key_double_click();
		break;

		case APP_KEY_EVENT_LONGPRESS:
			app_ibrt_ui_handle_touch_key_longpress();
		break;

		case APP_KEY_EVENT_COVER_PRESS:
			app_switch_to_quick_conversation_mode();
		break;

		case APP_KEY_EVENT_COVER_LEAVE:
			app_exit_quick_conversation_mode();
		break;
		
        default:
			TRACE(2,"%s Invalid key event", __func__);
        break;
    }
}

void app_ibrt_ui_dynamic_handle_touch_key_double_click(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
	TOTA_BLE_KET_FUN_MAP key_func = BLE_KEY_FUN_MAP_INVALID;
	
	switch(hfcall_machine)
    {
		case HFCALL_MACHINE_CURRENT_IDLE:
		case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KET_EVENT_MAP_DOUBLE);
			if(key_func != BLE_KEY_FUN_MAP_INVALID)
			{
				app_ibrt_ui_dynamic_handle_key_function(key_func);
			}
		break;

		case HFCALL_MACHINE_CURRENT_INCOMMING:
        case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_ANSWER_CALL);
        break;

        case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING:
		case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;

		case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING:
        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
            hfp_handle_key(HFP_KEY_THREEWAY_HOLD_AND_ANSWER);
        break;
		
   		default:
        break;
	}
}

void app_ibrt_ui_dynamic_handle_touch_key_longpress(void)
{
    HFCALL_MACHINE_ENUM hfcall_machine = app_get_hfcall_machine();
	TOTA_BLE_KET_FUN_MAP key_func = BLE_KEY_FUN_MAP_INVALID;

	switch(hfcall_machine)
    {
		case HFCALL_MACHINE_CURRENT_IDLE:
		case HFCALL_MACHINE_CURRENT_IDLE_ANOTHER_IDLE:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KEY_EVENT_MAP_LONG);
			if(key_func != BLE_KEY_FUN_MAP_INVALID)
			{
				app_ibrt_ui_dynamic_handle_key_function(key_func);
			}
		break;
	
        case HFCALL_MACHINE_CURRENT_INCOMMING:
		case HFCALL_MACHINE_CURRENT_INCOMMING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_HANGUP_CALL);
		break;

		case HFCALL_MACHINE_CURRENT_OUTGOING:
		case HFCALL_MACHINE_CURRENT_OUTGOING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_HANGUP_CALL);
		break;

		case HFCALL_MACHINE_CURRENT_CALLING:
		case HFCALL_MACHINE_CURRENT_CALLING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_HANGUP_CALL);
		break;
		
		case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING:
		case HFCALL_MACHINE_CURRENT_3WAY_INCOMMING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_THREEWAY_HOLD_REL_INCOMING);
		break;

		case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING:
        case HFCALL_MACHINE_CURRENT_3WAY_HOLD_CALLING_ANOTHER_IDLE:
			hfp_handle_key(HFP_KEY_THREEWAY_HANGUP_AND_ANSWER);
		break;

        default:
        break;
    }
}

void app_ibrt_ui_dynamic_handle_touch_key(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param)
{
	TOTA_BLE_KET_FUN_MAP key_func = BLE_KEY_FUN_MAP_INVALID;

	TRACE(0, "%s key event: %d", __func__, status->event);

	//for tws, should identify L or R earbud
	switch(status->event)
	{
		case APP_KEY_EVENT_SLIDE_LEFT:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KEY_EVENT_MAP_SWIPE_LEFT);
        break;

		case APP_KEY_EVENT_SLIDE_RIGHT:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KEY_EVENT_MAP_SWIPE_RIGHT);
		break;

		case APP_KEY_EVENT_SLIDE_UP:
		case APP_KEY_EVENT_SLIDE_UP_AND_HOLD:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KEY_EVENT_MAP_SWIPE_UP);
			if(APP_KEY_EVENT_SLIDE_UP_AND_HOLD == status->event)
			{
				if(!((BLE_KEY_FUN_MAP_VOL_UP == key_func) || (BLE_KEY_FUN_MAP_VOL_DOWN == key_func)))
				{
					TRACE(0, "up and hold event, but not vol up/down func, ignore");
					return;
				}
			}
		break;

		case APP_KEY_EVENT_SLIDE_DOWN:
		case APP_KEY_EVENT_SLIDE_DOWN_AND_HOLD:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KEY_EVENT_MAP_SWIPE_DOWN);
			if(APP_KEY_EVENT_SLIDE_DOWN_AND_HOLD == status->event)
			{
				if(!((BLE_KEY_FUN_MAP_VOL_UP == key_func) || (BLE_KEY_FUN_MAP_VOL_DOWN == key_func))) 
				{
					TRACE(0, "up and hold event, but not vol up/down func, ignore");
					return;
				}
			}
		break;

		case APP_KEY_EVENT_CLICK:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KET_EVENT_MAP_CLICK);
		break;

		case APP_KEY_EVENT_DOUBLECLICK:
			app_ibrt_ui_dynamic_handle_touch_key_double_click();
			return;
		break;

		case APP_KEY_EVENT_TRIPLECLICK:
			key_func = user_custom_find_key_func(BLE_LR_EARBUD_MAP_R,
				BLE_KET_CODE_MAP_TOUCH, BLE_KEY_EVENT_MAP_TRIPLE);
		break;

		case APP_KEY_EVENT_LONGPRESS:
			app_ibrt_ui_dynamic_handle_touch_key_longpress();
			return;
		break;
		
		case APP_KEY_EVENT_COVER_PRESS:
			app_switch_to_quick_conversation_mode();
		break;

		case APP_KEY_EVENT_COVER_LEAVE:
			app_exit_quick_conversation_mode();
		break;

		case APP_KEY_EVENT_1CLICK_AND_HOLD:
			//a2dp_handleKey(AVRCP_KEY_FAST_FORWARD_START);
		break;

		case APP_KEY_EVENT_1CLICK_AND_HOLD_LEAVE:
			//a2dp_handleKey(AVRCP_KEY_FAST_FORWARD_STOP);
		break;

		case APP_KEY_EVENT_2CLICK_AND_HOLD:
			//a2dp_handleKey(AVRCP_KEY_REWIND_START);
		break;

		case APP_KEY_EVENT_2CLICK_AND_HOLD_LEAVE:
			//a2dp_handleKey(AVRCP_KEY_REWIND_STOP);
		break;
		
		default:
			TRACE(2,"%s Invalid key event", __func__);
        break;
	}

	if(key_func != BLE_KEY_FUN_MAP_INVALID)
	{
		app_ibrt_ui_dynamic_handle_key_function(key_func);
	}
}
#endif

#if (defined(BT_USB_AUDIO_DUAL_MODE) || defined(BTUSB_AUDIO_MODE))
#ifdef CMT_008_CST820_TOUCH
void app_handle_usb_touch_key(APP_KEY_STATUS *status, void *param)
{
	TRACE(0, "%s key event: %d", __func__, status->event);

	switch(status->event)
    {
        case APP_KEY_EVENT_SLIDE_LEFT:
            usb_audio_hid_set_event(USB_AUDIO_HID_SCAN_PREV, 1);
        break;
			
		case APP_KEY_EVENT_SLIDE_RIGHT:
			usb_audio_hid_set_event(USB_AUDIO_HID_SCAN_NEXT, 1);
		break;

		case APP_KEY_EVENT_SLIDE_UP:
		case APP_KEY_EVENT_SLIDE_UP_AND_HOLD:
			usb_audio_hid_set_event(USB_AUDIO_HID_VOL_UP, 1);
		break;

		case APP_KEY_EVENT_SLIDE_DOWN:
		case APP_KEY_EVENT_SLIDE_DOWN_AND_HOLD:
			usb_audio_hid_set_event(USB_AUDIO_HID_VOL_DOWN, 1);
		break;
		
		case APP_KEY_EVENT_DOUBLECLICK:
			usb_audio_hid_set_event(USB_AUDIO_HID_PLAY_PAUSE, 1);
		break;

		case APP_KEY_EVENT_LONGPRESS:
			
		break;

		case APP_KEY_EVENT_COVER_PRESS:
			app_switch_to_quick_conversation_mode();
		break;

		case APP_KEY_EVENT_COVER_LEAVE:
			app_exit_quick_conversation_mode();
		break;
		
        default:
			TRACE(2,"%s Invalid key event", __func__);
        break;
    }
}
#endif


#endif
/* End Add by lewis */

