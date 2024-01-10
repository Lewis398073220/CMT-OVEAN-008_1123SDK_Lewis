/**
 *Name        : ctp_hynitron_ext.c
 *Author      : gary
 *Version     : V1.0
 *Create      : 2017-11-23
 *Copyright   : zxzz
 */

#include "plat_types.h"
#include "stdlib.h"
#include "hal_trace.h"

#include "cst_capacitive_tp_hynitron_cst0xx.h"
#include "cst_ctp_hynitron_ext.h"


#ifdef CMT_008_CST820_TOUCH

#define CTP_UPDATE_DBG
#ifdef CTP_UPDATE_DBG
#define CTP_UPDATE_DBG_TRACE(n,str, ...)		TRACE(n,str, ##__VA_ARGS__)
#else
#define CTP_UPDATE_DBG_TRACE(n,str, ...)  	TRACE_DUMMY(n,str, ##__VA_ARGS__)
#endif

#define CTP_UPDATE_ADDR		0x6A

static int16_t cst820_enter_bootmode(void)
{
     uint8_t retryCnt = 10;
	 uint8_t cmd[3];
	 uint8_t receiveByte=0;

     hal_set_cst820_rst_low();
     user_delay_ms(10);
     hal_set_cst820_rst_high();
     user_delay_ms(5);
     while(retryCnt--){ 
        cmd[0]=0xA0;
		cmd[1]=0x01;
	    cmd[2]=0xAB;
		if(user_i2c_write(CTP_UPDATE_ADDR, cmd, 3)){  // enter program mode
		  	user_delay_ms(2);
			continue; 
		}
		
		cmd[0]=0xA0;
		cmd[1]=0x03;
		if(user_i2c_read(CTP_UPDATE_ADDR, cmd, 2, &receiveByte, 1)){ // read flag
			user_delay_ms(2);
			continue; 
		}
		else{
			CTP_UPDATE_DBG_TRACE(2,"*** [%s] rev=0x%02X", __func__, receiveByte);
			//if (receiveByte != 0x55){//for 816
			if (receiveByte != 0xC1){  //for 820
                user_delay_ms(2);
				continue; 
            }else{
                 return 0;
            }
		}
     }
     return -1;
 }

int16_t cst820_update(uint16_t startAddr,uint16_t len,uint8_t* src){
     uint16_t sum_len;
	 uint16_t i=0;
     uint8_t cmd[514];	 
	 uint8_t receiveByte=0;

     if (cst820_enter_bootmode() == -1){
        return -1;
     }
     sum_len = 0; 
	 CTP_UPDATE_DBG_TRACE(1,"*** [%s]", __func__);
#define PER_LEN	512
     do{
         if (sum_len >= len){
             return 0;
         }
         
         // send address
		 cmd[0] = 0xA0;
         cmd[1] = 0x14;
         cmd[2] = startAddr&0xFF;
         cmd[3] = startAddr>>8;
         user_i2c_write(CTP_UPDATE_ADDR, cmd, 4);
         		
		 cmd[0]=0xA0;
		 cmd[1]=0x18;
		 for(i=2;i<514;i++)
		   	cmd[i]=*src++;
		  
		 user_i2c_write(CTP_UPDATE_ADDR, cmd, 514);//һ���Է���512�����ֽ�
 
         cmd[0] = 0xA0;
		 cmd[1] = 0x04;
		 cmd[2] = 0xEE;
         user_i2c_write(CTP_UPDATE_ADDR, cmd, 3);//��ִ���������֮ǰ������д��512���ֽڣ�����
         
         user_delay_ms(300);//���ʱ�䲻��ʡ  unit=1ms    >=150 
         {
            uint8_t retrycnt = 50;
			cmd[0] = 0xA0;
			cmd[1] = 0x05;
			//cmd[2] = 0x00;			 
            while(retrycnt--){                 
                user_i2c_read(CTP_UPDATE_ADDR, cmd, 2, &receiveByte, 1);				
				//CTP_UPDATE_DBG_TRACE("******%s, rev=0x%2x",__func__,receiveByte);
                if (receiveByte == 0x55){
                     break;// success
                }
                user_delay_ms(10);
            }
         }
         startAddr += PER_LEN;
         //src += PER_LEN;
         sum_len += PER_LEN;
     }while(len);
     
     // exit program mode
     cmd[0] = 0xA0;
	 cmd[1] = 0x03;
	 cmd[2] = 0x00;
     user_i2c_write(CTP_UPDATE_ADDR, cmd, 3);

	 return 0;
 }

uint32_t cst820_read_checksum(void){
     union{
         uint16_t sum;
         uint8_t buf[2];
     }checksum;
     uint8_t cmd[3];
 
     if (cst820_enter_bootmode() == -1){
        return -1;
     }
     
     cmd[0] = 0xA0;
	 cmd[1] = 0x03;
	 cmd[2] = 0x00;
     if (user_i2c_write(CTP_UPDATE_ADDR, cmd, 3)){
         return -1;
     }
     user_delay_ms(500);// Ҫ��400ms����

     checksum.sum = 0;
	 cmd[0] = 0xA0;
	 cmd[1] = 0x08;
	 if(user_i2c_read(CTP_UPDATE_ADDR, cmd, 2, checksum.buf, 2)){
         return -1;
     }	 
	 CTP_UPDATE_DBG_TRACE(2,"*** [%s] read_checksum=%d",__func__,checksum.sum);
     return checksum.sum;
}

 bool ctp_hynitron_update(void)
 {
#if CTP_HYNITRON_EXT_CST820_UPDATE
    user_i2c_init(200000); //��iic��ַ���ó�0x6A��iic�ٶ�Ҫ��400k���� 
	cst820_rst_init();
    if (cst820_enter_bootmode() == 0){			//����bootģʽ
#include "capacitive_hynitron_cst820_update.h"
        if(sizeof(app_bin) > 10){
            uint16_t startAddr = app_bin[1];	//app_bin��capacitive_hynitron_cst820_update.h�е�����
            uint16_t length = app_bin[3];
            uint16_t checksum = app_bin[5];
            startAddr <<= 8; 
			startAddr |= app_bin[0];			//�̼�����ʼ��ַ
            length <<= 8; 
			length |= app_bin[2];				//�̼��Ĵ�С
            checksum <<= 8;
			checksum |= app_bin[4]; 			//�̼���У���
			//�����ǻ�ȡ�����̼���һЩ��Ϣ	
			CTP_UPDATE_DBG_TRACE(2,"*** [%s] cst820 updatefile_checksum=%d",__func__,checksum);
            if(cst820_read_checksum() != checksum){			//��ȡIC�����У��ͣ���̼��Ĳ�һ��ʱ�ͽ�����������
                cst820_update(startAddr, length, app_bin+6);	//��ʼִ����������
                cst820_read_checksum();						//��������¼��ȥ��У���
            }
        }
        return CTP_TRUE;
    }
#endif
      return CTP_FALSE;
 }


#if 0
#if CTP_HYNITRON_EXT==1
#define REG_LEN_1B  1 //�Ĵ�������
#define REG_LEN_2B  2 //�Ĵ�������

#if CTP_HYNITRON_EXT_CST820_UPDATE==1
/*****************************************************************/
// For CSK0xx update
 /*
  *
  */
static int cst820_enter_bootmode(void){
     char retryCnt = 10;

     CTP_SET_I2C_DATA_OUTPUT;//io��ģ��iic������ʵ����Ҫ��������
     CTP_SET_I2C_DATA_HIGH;
     CTP_SET_I2C_CLK_OUTPUT;
     CTP_SET_I2C_CLK_HIGH;
     CTP_SET_RESET_PIN_LOW;//���ƴ���IC��λ����
     hctp_delay_ms(10);
     CTP_SET_RESET_PIN_HIGH;
     mdelay(5);
     while(retryCnt--){
         u8 cmd[3];
         cmd[0] = 0xAB;
         if (-1 == hctp_write_bytes(0xA001,cmd,1,REG_LEN_2B)){  // enter program mode
             mdelay(2); // 2ms
             continue;                   
         }
         if (-1 == hctp_read_bytes(0xA003,cmd,1,REG_LEN_2B)) { // read flag
             mdelay(2); // 2ms
             continue;                           
         }else{
             if (cmd[0] != 0x55){
                 msleep(2); // 2ms
                 continue;
             }else{
                 return 0;
             }
         }
     }
     return -1;
 }
 /*
  *
  */
static int cst820_update(u16 startAddr,u16 len,u8* src){
     u16 sum_len;
     u8 cmd[10];

     if (cst820_enter_bootmode() == -1){
        return -1;
     }
     sum_len = 0;
 
#define PER_LEN	512
     do{
         if (sum_len >= len){
             return;
         }
         
         // send address
         cmd[0] = startAddr&0xFF;
         cmd[1] = startAddr>>8;
         hctp_write_bytes(0xA014,cmd,2,REG_LEN_2B);
         
		 
		#if 0  
			#if 1
			{
				u8 i=0;
				for(i=0; i<4; i++){
					hctp_write_bytes(0xA018+(i*128),src+(i*128),PER_LEN/4,REG_LEN_2B);//512���ֽڷ�4�Σ�ÿ�η���128���ֽڡ�
				}
			}
			#else
			{
			  u8 i=0;
			  for(i=0; i<16; i++){
			   hctp_write_bytes(0xA018+(i*32),src+(i*32),PER_LEN/16,REG_LEN_2B);//512���ֽڷ�16�Σ�ÿ�η���32���ֽڡ�
			  }
			}
			
		   #endif 
		#else
		   hctp_write_bytes(0xA018,src,PER_LEN,REG_LEN_2B);//һ���Է���512�����ֽ�
		#endif

 
         cmd[0] = 0xEE;
         hctp_write_bytes(0xA004,cmd,1,REG_LEN_2B);//��ִ���������֮ǰ������д��512���ֽڣ�����
         msleep(100);//���ʱ�䲻��ʡ  unit=1ms    >=150
 
         {
             u8 retrycnt = 50;
             while(retrycnt--){
                 cmd[0] = 0;
                 hctp_read_bytes(0xA005,cmd,1,REG_LEN_2B);
                 if (cmd[0] == 0x55){
                     break;// success
                 }
                 msleep(10);
             }
         }
         startAddr += PER_LEN;
         src += PER_LEN;
         sum_len += PER_LEN;
     }while(len);
     
     // exit program mode
     cmd[0] = 0x00;
     hctp_write_bytes(0xA003,cmd,1,REG_LEN_2B);
 }
 /*
  *
  */
static u32 cst820_read_checksum(u16 startAddr,u16 len){
     union{
         u32 sum;
         u8 buf[4];
     }checksum;
     char cmd[3];
     char readback[4] = {0};
 
     if (cst820_enter_bootmode() == -1){
        return -1;
     }
     
     cmd[0] = 0;
     if (-1 == hctp_write_bytes(0xA003,cmd,1,REG_LEN_2B)){
         return -1;
     }
     msleep(500);//Ҫ��400ms����

     checksum.sum = 0;
     if (-1 == hctp_read_bytes(0xA008,checksum.buf,2,REG_LEN_2B)){
         return -1;
     }
     return checksum.sum;
}
#endif 

/*****************************************************************/
// common

 /*
  *
  */
 kal_bool ctp_hynitron_update(void)
 {
     kal_uint8 lvalue;
     kal_uint8 write_data[2];
     kal_bool temp_result = CTP_TRUE;

#if CTP_HYNITRON_EXT_CST820_UPDATE==1
    hctp_i2c_init(0x6A<<1,50);   //��iic��ַ���ó�0x6A��iic�ٶ�Ҫ��400k����  
    if (cst820_enter_bootmode() == 0){//����bootģʽ
#include "capacitive_hynitron_cst820_update.h"
        if(sizeof(app_bin) > 10){
            kal_uint16 startAddr = app_bin[1];//app_bin��capacitive_hynitron_cst820_update.h�е�����
            kal_uint16 length = app_bin[3];
            kal_uint16 checksum = app_bin[5];
            startAddr <<= 8; startAddr |= app_bin[0];	//�̼�����ʼ��ַ
            length <<= 8; length |= app_bin[2];			//�̼��Ĵ�С
            checksum <<= 8; checksum |= app_bin[4]; 	//�̼���У���
			//�����ǻ�ȡ�����̼���һЩ��Ϣ			
            if(cst820_read_checksum(startAddr, length) != checksum){//��ȡIC�����У��ͣ���̼��Ĳ�һ��ʱ�ͽ�����������
                cst820_update(startAddr, length, app_bin+6);//��ʼִ����������
                cst820_read_checksum(startAddr, length);//��������¼��ȥ��У���
				//
            }
        }
        return CTP_TRUE;
    }
#endif

      return CTP_FALSE;
 }

#endif  //CTP_HYNITRON_EXT==1
#endif

#endif /*CMT_008_CST820_TOUCH*/

//����������ɺ�Ҫ�Ļ�0x15��ַ��������һ�����͸�λ���͸�λ��������������

