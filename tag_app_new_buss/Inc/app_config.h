/*
 * app_config.h
 *
 * 
 * ******globle defines.*****
 * 
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "soft_version.h"
#include "string.h"

#define USE_CRC_MSG

typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;

typedef unsigned char   bool;

#define STM32_UID_ADDR  0x1ffff7ac

/*****Define channel ******/

/*****Define of Power threshold for cycle picking ******/
#define MAX_VOLT												330
#define FIX_CYCLE_VOLT                  260

/*****Define of Power acquisition cycle ******/
#define LAZY_PW_SAMPLE_CYCLE            ((uint32)86400000)   //a day(ms)
#define BUSY_PW_SAMPLE_CYCLE            ((uint32)1800000)    //half an hour(ms).

/*****Define of RF working paras. ******/
#define RF_DEFAULT_CHANNEL              ((uint8)0)    //Default 2.4 GHz RF channel
#define MAX_MPDU_RXBUFF_NUM       	((uint8)5)
#define MAX_MPDU_LEN              	((uint8)64)

/*****Define of Cache buffer for receiving screen & firmware slices. ******/
#define SCREEN_RECVBUF_SIZE		((uint16)2048)
#define FLUSH_BUF_SIZE                  ((uint16)2040)

#define MAX_TRANS_PICKING_DATA_LEN	((uint8)12)

/*****Define of Null Wait timeouts & retry times of re-do job. ******/
typedef enum
{
    /*********TIMEOUT_RETRANSMIT : max retry-times for retransmit a pakage for a ack in Link layer *************/
    TIMEOUT_RETRANSMIT	= 4,     //�ײ�ACKȷ��ʧЧ�ط�����
    /*********TIMEOUT_JOIN_WAIT : max wait-time-out for waiting a join-response package. *************/
    TIMEOUT_JOIN_WAIT   = 5,     //�������صȴ�����
    /*********TIMEOUT_RETRY_FIND : max try-find broadcast msg sending out. *************/
    TIMEOUT_RETRY_FIND  = 5,     //Ѱ����ʱ����
	  TIMEOUT_RETRY_BUSS  = 10,     //ҵ���ش�������
    /*********TIMEOUT_RF_INIT : RF init-failed as max retry initing times. *************/
    TIMEOUT_RF_INIT     = 10,    //RF��ʼ����ʱ����
    /*********TIMEOUT_RESP_TRANS : max retry-times for retransmit a pakage for a ack in Network layer *************/
    TIMEOUT_RESP_TRANS  = 3,     //ȷ��֡�ظ�ʧ���ط�����
    /*********TIMEOUT_MAX_NO_RESP : No any response from coordinator,timeout for judging lost keep touch state *************/
    TIMEOUT_MAX_NO_RESP = 15,    //������ʧ����ʱ����
}TIME_OUT_T; 

/*****Define of working timeout of certain wait job ******/
typedef enum
{
    /*********DEFAULT_WAKETIME : default persist time for a waken cycle.*************/
    DEFAULT_WAKETIME  = (uint16)65, //��λ��΢�룬wake time,10ms is small
    /*********DEFAULT_WAITTIME : default persist time for a wating recv slice cycle.*************/
    DEFAULT_WAITTIME  =	(uint16)40, //��λ��΢�룬wait time��this is must be long enough.
    /*********DEFAULT_SLEEPTIME :default persist time for a sleep cycle*************/
    DEFAULT_SLEEPTIME =	(uint16)10,     //��λ���룬sleep time
    /*********DEFAULT_NEXT_FINDTIMEWAIT :default persist time for next find-retry hold on time*************/
    DEFAULT_NEXT_FINDTIMEWAIT  = (uint16)180,//��λ����
    
    DEFAULT_NIGHT_SLP_ONCE = (uint16)600,//��λ����
    
    DEFALUT_LED_ON_MS   = (uint16)50,//ms
    DEFALUT_LED_OFF_MS  = (uint16)500,//ms     
    DEFAULT_LED_SPARK_TIMES = (uint16)0xFFFF,//��˸���޴�
}TAG_TIME_T;


typedef enum 
{
   SYS_GUI_NOT_IN_NET ,	
   SYS_GUI_FINDING_IN_NET,
	 SYS_GUI_IN_NET,
}TAG_STATEType;

/*****Define of Relative index of storage GUI Table******/

#define   BLOCK_GUI_PAGE_SIZE              8     //Ԥ����8ҳ��GUI��Ϣ
#define   USR_GUI_PAGE_SIZE              10     //Ԥ����8ҳ��GUI��Ϣ
typedef enum 
{
   STORE_SYS_INFO_PAGE_ADDR   =  0x0800FC00,	//���ϵͳ��Ϣҳ				   1K
   STORE_SYS_STATE_PAGE_ADDR  =  0x0800F800,	//���ϵͳ״̬��Ϣҳ       1K
   STORE_LAST_GUI_PAGE_ADDR   =  0x0800F400,	//���GUI�ݴ�ҳ            1k
	 STORE_BLOCK_GUI_PAGE_ADDR  = (0x0800F400  - BLOCK_GUI_PAGE_SIZE*0x400),
	 STORE_USR_GUI_PAGE_ADDR    = (0x0800F000  - BLOCK_GUI_PAGE_SIZE*0x400 - USR_GUI_PAGE_SIZE*0x400),
	
	 STORE_APP_UPDATE_ADDR      = (0x08001400 + 29*0x400), //���������ַ
   STORE_APP_RUN_ADDR         =  0x08001400, //������е�ַ   37K MAX
	 STORE_STATIC_ZIMO_GUI_ADDR	= (0x08000C00 + 16*26 + 16*10 + 2*16),
	 STORE_STATIC_FUHAO_GUI_ADDR= (0x08000C00 + 16*26 + 16*10),
	 STORE_STATIC_NUM_GUI_ADDR	= (0x08000C00 + 16*26),
	 STORE_STATIC_ABC_GUI_ADDR	=  0x08000C00,
	 STORE_STATIC_GUI_ADDR      =  0x08000800,
	 STORE_BOOT_ADDR     			  =  0x08000000,
}FLASH_InfoType;

#define IS_SYS_INFO_TYPE(type) (((type) == STORE_SYS_INFO_PAGE_ADDR)||\
                                ((type) == STORE_SYS_STATE_PAGE_ADDR))





#endif /* APP_CONFIG_H_ */

