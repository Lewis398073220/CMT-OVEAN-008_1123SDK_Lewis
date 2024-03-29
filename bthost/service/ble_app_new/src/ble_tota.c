/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifdef BLE_TOTA_ENABLED
#include "app_ble.h"

#define TOTA_MAX_LEN (509)

/* Modify by lewis */
#ifdef CMT_008_BLE_ENABLE
#define tota_service_uuid_128_le         0x00, 0x00, 0x7C, 0x1E, 0x00, 0x65, 0x74, 0x61, 0x6D, 0x79, 0x72, 0x74, 0x6E, 0x75, 0x6F, 0x43
#define tota_rx_character_uuid_128_le    0x01, 0x00, 0x7C, 0x1E, 0x00, 0x65, 0x74, 0x61, 0x6D, 0x79, 0x72, 0x74, 0x6E, 0x75, 0x6F, 0x43
#define tota_tx_character_uuid_128_le    0x02, 0x00, 0x7C, 0x1E, 0x00, 0x65, 0x74, 0x61, 0x6D, 0x79, 0x72, 0x74, 0x6E, 0x75, 0x6F, 0x43
#else
#define tota_service_uuid_128_le 0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86
#define tota_character_uuid_128_le 0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97,0x97
#endif
/* End Modify by lewis */

/* Modify by lewis */
#ifdef CMT_008_BLE_ENABLE

GATT_DECL_128_LE_PRI_SERVICE(g_ble_tota_service,
    tota_service_uuid_128_le);

GATT_DECL_128_LE_CHAR(g_ble_tota_rx_character,
    tota_rx_character_uuid_128_le,
    GATT_WR_REQ|GATT_WR_CMD,
    ATT_SEC_NONE);

GATT_DECL_CUDD_DESCRIPTOR(g_ble_tota_rx_cudd,
    ATT_SEC_NONE);

GATT_DECL_128_LE_CHAR(g_ble_tota_tx_character,
    tota_tx_character_uuid_128_le,
    GATT_NTF_PROP,
    ATT_SEC_NONE);

GATT_DECL_CCCD_DESCRIPTOR(g_ble_tota_tx_desc_cccd,
    ATT_SEC_NONE);

GATT_DECL_CUDD_DESCRIPTOR(g_ble_tota_tx_cudd,
    ATT_SEC_NONE);

#else
GATT_DECL_128_LE_PRI_SERVICE(g_ble_tota_service,
    tota_service_uuid_128_le);

GATT_DECL_128_LE_CHAR(g_ble_tota_character,
    tota_character_uuid_128_le,
    GATT_WR_REQ|GATT_WR_CMD|GATT_NTF_PROP,
    ATT_SEC_NONE);

GATT_DECL_CCCD_DESCRIPTOR(g_ble_tota_desc_cccd,
    ATT_SEC_NONE);
#endif
/* End Modify by lewis */

static const gatt_attribute_t g_ble_tota_attr_list[] = {
/* Modify by lewis */
#ifdef CMT_008_BLE_ENABLE

	/* Service */
    gatt_attribute(g_ble_tota_service),

	/* RX Characteristics */
    gatt_attribute(g_ble_tota_rx_character),
    /* Descriptor */
	gatt_attribute(g_ble_tota_rx_cudd),

	/* TX Characteristics */
    gatt_attribute(g_ble_tota_tx_character),
	/* Descriptor */
	gatt_attribute(g_ble_tota_tx_desc_cccd),
	gatt_attribute(g_ble_tota_tx_cudd),
	
#else
    /* Service */
    gatt_attribute(g_ble_tota_service),
    /* Characteristics */
    gatt_attribute(g_ble_tota_character),
        /* Descriptor */
        gatt_attribute(g_ble_tota_desc_cccd),
#endif
/* End Modify by lewis */
};

static app_tota_event_callback app_tota_event_cb = NULL;
/* Add by lewis */
#ifdef CMT_008_BLE_ENABLE
static const char custom_tx_desc[] = "Data Path TX Data";
static const char custom_rx_desc[] = "Data Path RX Data";
#endif
/* End Add by lewis */

static int ble_tota_server_callback(gatt_svc_t *svc, gatt_server_event_t event, gatt_server_callback_param_t param)
{
    app_tota_event_param_t cb_param = {0};

    cb_param.connhdl = svc->connhdl;

    switch (event)
    {
        case GATT_SERV_EVENT_CHAR_WRITE:
        {
            gatt_server_char_write_t *p = param.char_write;
            if (p->value_offset != 0 || p->value_len == 0 || p->value == NULL)
            {
                return false;
            }
            if(app_tota_event_cb)
            {
                cb_param.event_type   = APP_TOTA_RECEVICE_DATA;
                cb_param.conidx = p->con_idx;
                cb_param.param.receive_data.data_len = p->value_len;
                cb_param.param.receive_data.data     = (uint8_t *)p->value;
                app_tota_event_cb(&cb_param);
            }
            return true;
        }
        case GATT_SERV_EVENT_DESC_WRITE:
        {
            gatt_server_desc_write_t *p = param.desc_write;
            uint16_t config = CO_COMBINE_UINT16_LE(p->value);
            bool notify_enabled = false;
            if (config & GATT_CCCD_SET_NOTIFICATION)
            {
                notify_enabled = true;
            }
            if(app_tota_event_cb)
            {
                cb_param.event_type   = APP_TOTA_CCC_CHANGED;
                cb_param.conidx = p->con_idx;
                cb_param.param.ntf_en = notify_enabled;
                app_tota_event_cb(&cb_param);
            }
            return true;
        }
        case GATT_SERV_EVENT_NTF_TX_DONE:
        case GATT_SERV_EVENT_INDICATE_CFM:
        {
            gatt_server_indicate_cfm_t *p = param.confirm;
            if(app_tota_event_cb)
            {
                cb_param.event_type   = APP_TOTA_SEND_DONE;
                cb_param.conidx = p->con_idx;
                cb_param.param.status = p->error_code;
                app_tota_event_cb(&cb_param);
            }
            break;
        }
        case GATT_SERV_EVENT_MTU_CHANGED:
        {
            gatt_server_mtu_changed_t *p = param.mtu_changed;
            if (app_tota_event_cb)
            {
                cb_param.event_type = APP_TOTA_MTU_UPDATE;
                cb_param.conidx = p->con_idx;
                cb_param.param.mtu = p->mtu;
                app_tota_event_cb(&cb_param);
            }
            break;
        }
        case GATT_SERV_EVENT_CONN_CLOSED:
        {
            gatt_server_conn_closed_t *p = param.closed;
            if(app_tota_event_cb)
            {
                cb_param.event_type = APP_TOTA_DIS_CONN_EVENT;
                cb_param.conidx = p->con_idx;
                cb_param.param.status = p->error_code;
                app_tota_event_cb(&cb_param);
            }
            break;
        }
		
/* Add by lewis */
#ifdef CMT_008_BLE_ENABLE
		case GATT_SERV_EVENT_DESC_READ:
        {
            gatt_server_desc_read_t *p = param.desc_read;
            if (p->descriptor == g_ble_tota_tx_cudd)
            {
                gatts_write_read_rsp_data(p->ctx, (uint8_t *)custom_tx_desc, sizeof(custom_tx_desc));
                return true;
            }
            else if (p->descriptor == g_ble_tota_rx_cudd)
            {
                gatts_write_read_rsp_data(p->ctx, (uint8_t *)custom_rx_desc, sizeof(custom_rx_desc));
                return true;
            }
            break;
        }
#endif
/* End Add by lewis */

        default:
        {
            break;
        }
    }

    return 0;
}

void ble_tota_init(void)
{
    gatts_register_service(g_ble_tota_attr_list, ARRAY_SIZE(g_ble_tota_attr_list), ble_tota_server_callback, NULL);
}

void app_tota_event_reg(app_tota_event_callback cb)
{
    if(!app_tota_event_cb)
    {
        app_tota_event_cb = cb;
    }
}

void app_tota_event_unreg(void)
{
    app_tota_event_cb = NULL;
}

bool app_tota_send_notification(uint8_t conidx, uint8_t* data, uint32_t len)
{
    bt_status_t status = BT_STS_SUCCESS;
	
/* Modify by lewis */
#ifdef CMT_008_BLE_ENABLE
	status = gatts_send_notification(gap_conn_bf(conidx), g_ble_tota_tx_character, data, (uint16_t)len);
#else
    status = gatts_send_notification(gap_conn_bf(conidx), g_ble_tota_character, data, (uint16_t)len);
#endif
/* End Modify by lewis */

    return (status != BT_STS_FAILED);
}

bool app_tota_send_indication(uint8_t conidx, uint8_t* data, uint32_t len)
{
    bt_status_t status = BT_STS_SUCCESS;
	
/* Modify by lewis */
#ifdef CMT_008_BLE_ENABLE
	status = gatts_send_indication(gap_conn_bf(conidx), g_ble_tota_tx_character, data, (uint16_t)len);
#else
    status = gatts_send_indication(gap_conn_bf(conidx), g_ble_tota_character, data, (uint16_t)len);
#endif
/* End Modify by lewis */

	return (status != BT_STS_FAILED);
}

#endif /* BLE_TOTA_ENABLED */
