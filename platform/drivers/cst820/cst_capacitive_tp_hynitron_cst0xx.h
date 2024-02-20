/**
 *Name        : capacitive_tp_hynitron_cst0xx.h
 *Author      : gary
 *Version     : V1.0
 *Create      : 2017-11-9
 *Copyright   : zxzz
 */


#ifndef CAPACITIVE_TP_HYNITRON_CST0XX_H__
#define CAPACITIVE_TP_HYNITRON_CST0XX_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CMT_008_CST820_TOUCH

//_TODO:
#define CTP_SLAVER_ADDR            			0x15
#define CTP_HYNITRON_EXT_CST820_UPDATE 	1
    
#define CTP_TRUE   1
#define CTP_FALSE  0

/* KCT988 touch
enum ctp_gesture_id{
	CTP_GESTURE_NONE=0x00,
	CTP_GESTURE_UP=0x04,//0x01,
	CTP_GESTURE_DOWN=0x03,//0x02,
	CTP_GESTURE_LEFT=0x01,//0x03,
	CTP_GESTURE_RIGHT=0x02,//0x04,
	CTP_GESTURE_CLICK=0x05,
	CTP_GESTURE_UP_AND_HOLD=0x06,
	CTP_GESTURE_DOWN_AND_HOLD=0x07,	
	CTP_GESTURE_DOUBLE=0x0B,
	CTP_GESTURE_LONG=0x0C,
	CTP_GESTURE_UP_AFTER_LONG=0x0D,
	CTP_GESTURE_TRIPLE=0x0E,//0x0D,
	CTP_GESTURE_COVER_PRESS=0x0F,//0x1E,
	CTP_GESTURE_COVER_LEAVE=0x10,//0x1F,	
};
*/
enum ctp_gesture_id{
	CTP_GESTURE_NONE=0x00,
	CTP_GESTURE_UP=0x01,
	CTP_GESTURE_DOWN=0x02,
	CTP_GESTURE_LEFT=0x03,
	CTP_GESTURE_RIGHT=0x04,
	CTP_GESTURE_CLICK=0x05,
	CTP_GESTURE_UP_AND_HOLD=0x06,
	CTP_GESTURE_DOWN_AND_HOLD=0x07,	
	CTP_GESTURE_DOUBLE=0x0B,
	CTP_GESTURE_LONG=0x0C,
	CTP_GESTURE_UP_AFTER_LONG=0x2D,
	CTP_GESTURE_TRIPLE=0x0D,
	CTP_GESTURE_COVER_PRESS=0x1E,
	CTP_GESTURE_COVER_LEAVE=0x1F,
	CTP_1CLICK_AND_HOLD = 0x50,
	CTP_1CLICK_AND_HOLD_LEAVE=0x5F,
	CTP_2CLICK_AND_HOLD = 0x60,
	CTP_2CLICK_AND_HOLD_LEAVE=0x6F,
};

void user_delay_ms(uint16_t Nms);
void hal_set_cst820_rst_high(void);
void hal_set_cst820_rst_low(void);
void cst820_rst_init(void);
void user_i2c_init(uint32_t speed);
uint32_t user_i2c_write(uint8_t uchDeviceId, const uint8_t uchWriteBytesArr[], uint16_t usWriteLen);
uint32_t user_i2c_read(uint8_t uchDeviceId, const uint8_t uchCmddBytesArr[], uint16_t usCmddLen, uint8_t uchReadBytesArr[], uint16_t usMaxReadLen);
void cst820_open_module(void);

#endif /*CMT_008_CST820_TOUCH*/

#ifdef __cplusplus
}
#endif

#endif /*CAPACITIVE_TP_HYNITRON_CST0XX_H__*/

