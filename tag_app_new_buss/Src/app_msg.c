
#include "bsp.h"

wr_firmware_t firmware;

bz_resp_t  bz_resp={0};

static void App_recvQueryRsp(uint8* pData);
static void App_recvAckRsp(uint8* pData);

uint8 App_SendBuzziEventRsp(uint8 cmd,uint8 status) ;
uint8 App_SendTransparentMsg(uint8 evet_code);
/*
 *Name:App_SendNetworkSearchMsg
 *Function:sending net-working find request 
 */
void App_SendNetworkSearchMsg(void)
{
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;
	papp_search_req_t pnwksearch = (papp_search_req_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
        
	TxMpdu.fcf = FCF_NOACK_SRC_LONG_DATA;  //no need ack.
	TxMpdu.DestPanId = tag_flash_info.pan_id;
	TxMpdu.DestshortAddr = BROADCAST;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
	TxMpdu.ApduLen = sizeof(rf_header_t)+sizeof(app_search_req_t);
        
	phead->head = NWK_REQUEST_HEAD;
	phead->cmd = NWK_SEARCH;
	phead->length = sizeof(app_search_req_t);
        
	pnwksearch->devType = RFID_NODE;
  pnwksearch->status = tag_state.status;
	pnwksearch->tail = TAIL;
  

	RfSendPacket();

	
}


/*
 *Name:App_RecvNetworkSearchMsgRsp
 *Function:receiving network-find response from gateway.
 */
void App_RecvNetworkSearchMsgRsp(void)
{
  prf_header_t prsp = (prf_header_t)RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload;
	papp_search_rsp_t pnwksearch = (papp_search_rsp_t)(RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload+sizeof(rf_header_t));

  if((prsp->head==NWK_RESPONSE_HEAD)&&(prsp->cmd==NWK_SEARCH)
			&&(pnwksearch->devType==RFID_GATEWAY) &&(pnwksearch->tail==TAIL))//filter.
	{
		
		tag_flash_info.pan_id = pnwksearch->nwkPanid;
    tag_flash_info.gateway_addr = pnwksearch->nwkAddr;
                                  
    //agree to join this pan and then backup pan info to external flash.
    update_find_NetWorkInfo(tag_flash_info.pan_id, tag_flash_info.gateway_addr); 
		
    tag_state.state = NWK_JOIN_INIT;
	}
	
}


/*
 *Name:App_SendJoinRequestMsg
 *Function:sending going to join a certain PAN request 
 */
void App_SendJoinRequestMsg(void)
{
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;//for send msg of JOIN REQ.
	papp_join_req_t pjoinrequest = (papp_join_req_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
	
        TxMpdu.fcf = FCF_NOACK_SRC_LONG_DATA; //no need ack.
	TxMpdu.DestPanId = tag_flash_info.pan_id; //for we have stored dest panid 
	TxMpdu.DestshortAddr = tag_flash_info.gateway_addr;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
	TxMpdu.ApduLen = sizeof(rf_header_t)+sizeof(app_join_req_t);
        
	phead->head = NWK_REQUEST_HEAD;
	phead->cmd = NWK_JOIN;//now we tell gateway that we're do JOIN REQ.
	phead->length = sizeof(app_join_req_t);
        
	pjoinrequest->devType = RFID_NODE;
        pjoinrequest->firmware_version = APP_SOFT_VERSION;
        pjoinrequest->sleeptime = tag_state.sleeptime;
	pjoinrequest->tail = TAIL;
        
  RfSendPacket();
}

/*
 *Name:App_RecvJoinRequestMsgRsp
 *Function:receiving agree-to-let-it-join response from gateway.
 */
void App_RecvJoinRequestMsgRsp(void)
{
	prf_header_t prsp = (prf_header_t)RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload;
	papp_join_rsp_t pjoinrequest = (papp_join_rsp_t)(RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload+sizeof(rf_header_t));
	
  if((prsp->head==NWK_RESPONSE_HEAD) && (prsp->cmd==NWK_JOIN) 
																		 && (pjoinrequest->devType == RFID_GATEWAY) && (pjoinrequest->tail == TAIL))
	{
		sys_info_print("Join Net..", 10);

		
		tag_flash_info.gateway_addr = pjoinrequest->tagAddr;
		
    tag_flash_info.listen_channel = pjoinrequest->listen_channel;
    tag_flash_info.work_channel =   pjoinrequest->work_channel;
		tag_flash_info.short_addr = pjoinrequest->tagAddr;
		
		srand(tag_flash_info.short_addr);
		
		update_join_NetWorkInfo(tag_flash_info.listen_channel,tag_flash_info.work_channel, tag_flash_info.short_addr);
		
		
    tag_state.state = STANDBY_INIT;//LISTEN_INIT;
		AppRF_turn(STANDBY_INIT);
		
		
	}
}

int App_RecvStandByMsgRsp(void)
{
	
	prf_header_t prsp = (prf_header_t)RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload;
	uint8* pdata = (uint8*)(RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload+sizeof(rf_header_t));

	
  if((prsp->head==NWK_RESPONSE_HEAD)&&(prsp->cmd==CMD_LONG_BUSSI_DATA ||prsp->cmd==CMD_UPDATE_FIRMWARE))
  {
    tag_state.state = WRITE_INIT;
    return 1;
  }

  if((prsp->head==NWK_RESPONSE_HEAD)&&(prsp->cmd==NWK_QUERY))
  {  
    App_recvQueryRsp(pdata);
		
    return 0;
	} 
        
  if((prsp->head==NWK_RESPONSE_HEAD)&&(prsp->cmd==NWK_RESP))
  {
     App_recvAckRsp(pdata);
    return 0;
  }

  return 0;
}


uint8 App_SendEventRsp(uint8 cmd,uint8 status) 
{
	TRANSIMIT_STATUS trans_st = TRANS_FAILED;
	uint8 permit_trans_times = TIMEOUT_RESP_TRANS;
        
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;
	prf_rsp_t prsp = (prf_rsp_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
	
	TxMpdu.fcf = FCF_NOACK_SRC_SHORT_DATA;
	TxMpdu.DestPanId = tag_flash_info.pan_id;
	TxMpdu.DestshortAddr = tag_flash_info.gateway_addr;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
	TxMpdu.ApduLen = sizeof(rf_header_t)+sizeof(rf_rsp_t);
	
	phead->head = CMD_RESPONSE_HEAD;
	phead->cmd = NWK_RESP;
	phead->length = sizeof(rf_rsp_t);
	 
	prsp->token = tag_state.token;
	prsp->ackCmd = cmd;
	prsp->status = status;
	prsp->tail = TAIL;
        
        
	while(permit_trans_times)
	{
		trans_st = RfSendPacket();
           
		if(trans_st == TRANS_SUCCESS) break;

		permit_trans_times--;
	}
       
	return trans_st;
}

void App_SendRetransReqMsg(void)
{
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;
	papp_retans_req_t pretrans = (papp_retans_req_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
	
	TxMpdu.fcf = FCF_NOACK_SRC_SHORT_DATA;
	TxMpdu.DestPanId = tag_flash_info.pan_id;
	TxMpdu.DestshortAddr = tag_flash_info.gateway_addr;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
	TxMpdu.ApduLen = sizeof(rf_header_t)+sizeof(app_retans_req_t);
	
	phead->head = NWK_REQUEST_HEAD;
	phead->cmd = NWK_RETRANS;
	phead->length = sizeof(app_retans_req_t);
	
	pretrans->token = tag_state.token;
	pretrans->store_slice = req_slice;
	pretrans->tail = TAIL;
        
  RfSendPacket();
}

/*
 *Name:App_SendQueryMsg
 *Function:sending query msg of tag_state. 
 */
void App_SendQueryMsg(void)
{
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;
	papp_standby_query_t pquery = (papp_standby_query_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
	
  TxMpdu.fcf = FCK_QUERY_MSG_ALONG;//no need ack.
	TxMpdu.DestPanId = tag_flash_info.pan_id;
	TxMpdu.DestshortAddr = tag_flash_info.gateway_addr;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
	TxMpdu.ApduLen = sizeof(rf_header_t)+sizeof(app_standby_query_t);
	
  phead->head = NWK_REQUEST_HEAD;
	phead->cmd = NWK_QUERY;
	phead->length = sizeof(app_standby_query_t);
	
  pquery->token = tag_state.token;
	pquery->power = tag_state.energy;
	pquery->tail = TAIL;
	
  //tag_state.energy  &= 0x7f;
    
  RfSendPacket();
}



void App_SendEventRspMsg(uint8 idx)
{
    //WD_KICK();
    
	switch(idx)
	{
		case FLAG_SHORT_DATA:
			App_SendBuzziEventRsp(CMD_SHORT_BUSSI_DATA,0);
			break;
      
    case FLAG_LONG_DATA:
      App_SendBuzziEventRsp(CMD_LONG_BUSSI_DATA,0);
      break; 
      
    case FLAG_FMWARE_OK:
			App_SendEventRsp(CMD_UPDATE_FIRMWARE,0);
			break;
     
    case FLAG_FMWARE_FAIL:
      App_SendEventRsp(CMD_UPDATE_FIRMWARE,1);
      break;  
 
    case FLAG_TRANS_REPORT:
		{
			uint8 idx;
			
			for(idx=0;idx<MAX_REPORT_USR_EVENT_NUM;idx++)
      {
				if(tag_state.eve_counter[idx]) break;
			} 

			if((idx == EVENT_KEY_PUT_PRESS) && (tag_state.erase_flag)){
           
				//fade_indic_block_away();  //erase digit block 
            
				
          
				tag_state.erase_flag = FALSE;
			}
      
			if(idx<MAX_REPORT_USR_EVENT_NUM)
			{
				tag_state.cur_code = idx;
				App_SendTransparentMsg(idx);
			}
			else
			{
				tag_state.ackflag &= ~(1<<FLAG_TRANS_REPORT);
			}
		}
    break;
      
    default:break;
    }
}


void App_Write_RecvProcess(void)
{
		/*fetch frame & identify type of it*/
		prf_header_t preq = (prf_header_t)RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload;
		uint8 *pPayload = RxMpdu.MpduBuff[RxMpdu.CurrentMpdu].pApduPayload+sizeof(rf_header_t);
		
		if(preq->cmd == CMD_LONG_BUSSI_DATA)      App_RecvWriteScreenReq(pPayload, preq->length);
    else if(preq->cmd == CMD_UPDATE_FIRMWARE) App_RecvWriteFirmwareToflash(pPayload, preq->length);
}

void App_RecvWriteScreenReq(uint8 *pPayload, uint8 length)
{
	if(((pPayload[0]&0x7f)-1)==screen.sliceseq) 
	{ 
    uint8 data[50];
		sprintf(data, "slic-%.2x", pPayload[0]);
		sys_info_print(data, strlen(data));		
		screen.sliceseq = pPayload[0];
		
	//WD_KICK();
 
		if(screen.sliceseq & 0x80)
		{
			uint8 wrtmplen = length-2;
			
			if(((screen.writelen+wrtmplen) == screen.length)&&(pPayload[length-1]==TAIL))
			{
				tag_state.recvflag = TRUE;
																
				memcpy(&screen.buf[screen.writelen], &pPayload[1], wrtmplen);//start copy from 2nd byte.
                                

				Write_EPaper(screen.buf, backgroud);
                                
				tag_state.ackflag |= (1<<FLAG_LONG_DATA);//0x0080;
				//update_ack_info(tag_state.ackflag);
				
				req_slice = 0; //reset. and finish,????
				tag_state.state = STANDBY_INIT;
			}
			else
			{
				sys_info_print("screen-error-1", 14);
				tag_state.state = STANDBY_INIT;////re-get ,it seems to be last request garbage!@,
			}
		}
		else
		{
			uint8 wrtmplen = length-2;//??????????
			if(pPayload[length-1]==TAIL)
			{
				tag_state.recvflag = TRUE;
				memcpy(&screen.buf[screen.writelen], &pPayload[1], wrtmplen);
				screen.writelen += wrtmplen;
  
				req_slice =  screen.sliceseq |0x4000; //????,????1.
			}
			else
			{      
       	sys_info_print("screen-error-2", 14);
				tag_state.state = STANDBY_INIT; ////re-get ,for slice check has error!
			}
		}
	}
	else
	{
		uint8 data[50];
		sprintf(data, "err3-%.2x-%.2x", (pPayload[0]&0x7f)-1, screen.sliceseq);
		sys_info_print(data, strlen(data));
		tag_state.recvflag = FALSE;
		tag_state.state = STANDBY_INIT; //reget data.
	}
	
	
}

void App_RecvWriteFirmwareToflash(uint8 *pdata, uint8 length)
{
    static uint32 check_sum = 0;
    uint16 slice = *pdata | (*(pdata+1)<<8);
    uint8  fill_offset;
    uint16  wrlasttmp ;
    uint8  *dataToWrite = pdata + 2;

    if(((slice&0x3fff)-1)== firmware.sliceseq) //recv by seq.
    {  
			  uint8 wrtmplen = length-3;//?????,max value is 108
        //cur_slice = firmware.sliceseq; //pos of prior received slice.
        firmware.sliceseq = slice ;
      
       
        
       // WD_KICK();
        
        if((firmware.sliceseq & 0x8000)!=0)// is the last slice ,.,.
        {
					if(((firmware.writelen + screen.writelen + wrtmplen) == firmware.length)&&(*(pdata+length-1)==TAIL))
					{
						tag_state.recvflag = TRUE;
            //WD_KICK();
                        
					  //check_sum += sum_check_cal32(dataToWrite,wrtmplen);	
                        
            if((screen.writelen + wrtmplen) >= FLUSH_BUF_SIZE){ 
            if(screen.writelen %4){
							fill_offset = 4 - screen.writelen %4; 
							memcpy(&screen.buf[screen.writelen], dataToWrite, fill_offset);
              screen.writelen += fill_offset;
              wrtmplen -= fill_offset;
              dataToWrite = dataToWrite + fill_offset;
            }
            //store_firmware_slice_to_flash(firmware.writelen,screen.buf,screen.writelen);
                      
		        firmware.writelen += screen.writelen;
            screen.writelen = 0;
		      }

		      memcpy(&screen.buf[screen.writelen], dataToWrite, wrtmplen);
		      screen.writelen += wrtmplen;
                        
          //bytes of last slice may not Four byte aligned ,NOT forget to handle it...
          wrlasttmp = (screen.writelen%4)?(screen.writelen+(4-screen.writelen%4)):screen.writelen;
                        
          //store_firmware_slice_to_flash(firmware.writelen,screen.buf,wrlasttmp);
                                           
          //led_spark_indicate_firmware_finish();
                        
          if(firmware.recv_check_sum == check_sum){//????????
             tag_state.ackflag |= (uint16)(1<<FLAG_FMWARE_OK);//0x0100;
          }else{   
             tag_state.ackflag |= (uint16)(1<<FLAG_FMWARE_FAIL);//0x0200;
          }               
          //update_ack_info(tag_state.ackflag);
                        
          req_slice = 0; //reset. and finish,?????standby??
          tag_state.state = STANDBY_INIT;
	    	}
				else{
          tag_state.state = STANDBY_INIT;////re-get ,it seems to be last request garbage!@,
				}
	}
	else //not the last slice
	{
		if(*(pdata+length-1)==TAIL)
		{
			tag_state.recvflag = TRUE;
			check_sum += sum_check_cal32(dataToWrite,wrtmplen);	
                        
			if((screen.writelen + wrtmplen) >= FLUSH_BUF_SIZE){ 
					if(screen.writelen %4){
							fill_offset = 4 - screen.writelen %4; 
							memcpy(&screen.buf[screen.writelen], dataToWrite, fill_offset);
							screen.writelen += fill_offset;
							wrtmplen -= fill_offset;
							dataToWrite = dataToWrite + fill_offset;
					}
					
          //store_firmware_slice_to_flash(firmware.writelen,screen.buf,screen.writelen);
                            
		      firmware.writelen += screen.writelen;
          screen.writelen = 0;
		  }

			memcpy(&screen.buf[screen.writelen], dataToWrite, wrtmplen);
			screen.writelen += wrtmplen;
                        
			//cur_slice = firmware.sliceseq; //update screen slice received.
			req_slice = firmware.sliceseq | 0x8000; 
		}
		else{        
			tag_state.state = STANDBY_INIT; ////re-get ,for slice check has error!
		}
	}}else{
     tag_state.recvflag = FALSE;
	  tag_state.state = STANDBY_INIT; //reget data.
  }
}

static void App_recvAckRsp(uint8* pData)
{
      papp_rsp_ack_t pack = (papp_rsp_ack_t)pData;
      if(pack->tail == TAIL)
      {
         tag_state.recvflag = TRUE;
         

         //WD_KICK();
         
         switch(pack->ackCmd)
         {
            
            case CMD_REPORT_TAG_DELIVER_MSG:
            {
                uint8 idx;
                
                bz_resp.wait_flag = 0;
        
                if(tag_state.cur_code == EVENT_KEY_PUT_PRESS)
                {
                   tag_state.led.type = 0x00;
                   tag_state.checkfactor = 0xc3; //shorten sleep time.
                }
                   
                if(tag_state.eve_counter[tag_state.cur_code]>0)
                    tag_state.eve_counter[tag_state.cur_code] --;
                
                for(idx=0;idx<MAX_REPORT_USR_EVENT_NUM;idx++)
                    if(tag_state.eve_counter[idx])
                      break;
                
                if(idx >= MAX_REPORT_USR_EVENT_NUM)
                {
                    tag_state.ackflag &= ~(1<<FLAG_TRANS_REPORT);
                    update_ack_info(tag_state.ackflag);
                }
            }
            break;

            case CMD_SHORT_BUSSI_DATA://1:
            {
               tag_state.ackflag &= ~(uint16)(1<<FLAG_SHORT_DATA);
               update_ack_info(tag_state.ackflag);
               
               if(tag_state.buz_type == CMD_SHORT_BUSSI_DATA)
               {
                  switch(tag_state.buz_cmd_type)
                  { 
                   case CMD_SLEEPTIME:
                     {
                        tag_state.sleeptime = tag_state.cfg_sleeptime;
											  A7139_SetSleepTime(tag_state.sleeptime/100);
#ifdef TEST_TAG
											 
											  tag_test_print(tag_state.sleeptime);
#endif
                     }break;
                   case CMD_REBIND_NET:
                     { 
                        //tag_flash_info.gw_addr = (uint16)tag_state.shared_area[0];
                        //tag_flash_info.panId = (uint16)tag_state.shared_area[1];
                        //tag_flash_info.channel = (uint8)tag_state.shared_area[2];

                        //back_up_pan_info_to_flash(tag_flash_info.panId,tag_flash_info.gw_addr,tag_flash_info.channel); //agree to join this pan.
                
		                    //halRfSetPanId(tag_flash_info.panId);
                        //halRfSetChannel(tag_flash_info.channel);
                
                        tag_state.buz_type = tag_state.buz_cmd_type = 0;
                        tag_state.state = NWK_JOIN_INIT;   
                        return;
                     }break;
                   case CMD_NIGHT_MODE:
                     {
                        if(tag_state.status == TAG_HOLD_FOR_EVE_MODE)
                        {
                           tag_state.state = NIGHT_MODE;
                           tag_state.status = TAG_EVENING_MODE_RUN;
                        }
                        tag_state.buz_type = tag_state.buz_cmd_type = 0;
                        return;
                     }break;
                   case CMD_OFFLINE_NET:
                     {
                        tag_state.checkfactor = 0;
                        tag_state.buz_type = tag_state.buz_cmd_type = 0;
                        tag_state.state = IDLE_RUN;
                        return; 
                     }break;
                  }
               }
               tag_state.buz_type = tag_state.buz_cmd_type = 0;
            }
            break;
    
            case CMD_LONG_BUSSI_DATA:
            {
                 tag_state.buz_type = tag_state.buz_cmd_type = 0;
                 tag_state.ackflag &= ~(uint16)(1<<FLAG_LONG_DATA);
                 update_ack_info(tag_state.ackflag);
            }
            break;

            case CMD_UPDATE_FIRMWARE:
            {
                if(tag_state.ackflag &((uint16)(1<<FLAG_FMWARE_OK)))
                {
                     tag_state.ackflag &= ~(uint16)(1<<FLAG_FMWARE_OK);
                     //go_to_system_reload(firmware.writelen+screen.writelen,firmware.soft_version);
                }
                else
                {
                     tag_state.ackflag &= ~(uint16)(1<<FLAG_FMWARE_FAIL);
                     restore_screen_context_data();
                }
                update_ack_info(tag_state.ackflag);
            }
            break;
            
            default:break;
         }
				 	
         if(pack->more_buss_data == 1) tag_state.state = STANDBY_INIT;
				 else 
				 {
					 tag_state.state = LISTEN_INIT;
					 AppRF_turn(LISTEN_INIT);
					 rf_para_clean();
					 return;
				 }
      }
}

static void App_recvQueryRsp(uint8* pData)
{
  
     /******************************************\
      * pData stream is protocol packed as :\
      * ==  dataType  | EventInfo   | Tail ====\
      * ==  -1 byte - | -n byte(s) -| -1 byte==\
      ******************************************/
  
     uint8 *pdata = pData;  
     tag_state.recvflag = TRUE;
          
     switch(*pdata)
     {
				case DATATYPE_NULL_JOB:{//无任务，进入监听模式
					 tag_state.state = LISTEN_INIT;
					 AppRF_turn(LISTEN_INIT);
					 rf_para_clean();
					 return;
				}
				break;
				
				case CMD_SHORT_BUSSI_DATA:{
					uint8 led = *(pdata+1);
        
					tag_state.buz_type = *pdata;    
					pdata += 2; 
					
					//WD_KICK();
					tag_state.buz_cmd_type = *pdata;
			 
					switch(*pdata)
					{
        
						case CMD_TURN_OFF_LED:led = (tag_state.led.type &(~led));break; 
						case CMD_LOAD_GUI:{
							uint8 *para_p = pdata+1;
							load_usr_gui(*para_p);
						}
						break; 
						case CMD_BLOCK_DISP: refresh_screen_by_code(pdata+1);break;
						case CMD_LED_SETUP: 
						{
							uint8 *para_p = pdata+1;
            
							tag_state.led.on_ms = ((uint16)(*para_p)) |((uint16)(*(para_p+1))<<8);
							tag_state.led.off_ms = ((uint16)(*(para_p+2))) |((uint16)(*(para_p+3))<<8);
							tag_state.led.spark_t = ((uint16)(*(para_p+4))) |((uint16)(*(para_p+5))<<8);
							tag_state.led.type = 0;
							tag_state.led.spark_flag = (tag_state.led.on_ms==0xFFFF)?0:1;
                 
							update_led_spark_paras(&tag_state.led);
						}
						break;
						case CMD_SLEEPTIME: 
						{
							uint8 *para_p = pdata+1;
							tag_state.cfg_sleeptime = ((uint32)(*para_p)) |((uint32)(*(para_p+1))<<8)|\
                                    ((uint32)(*(para_p+2))<<16) |((uint32)(*(para_p+3))<<24);
           
							back_up_sleeptime_to_flash(tag_state.cfg_sleeptime);
						}
						break;
						case CMD_BUTT_REPORT:
						{
							uint8 *para_p = pdata+1;
							tag_state.reportflag = 0;
            
							if(*para_p)  tag_state.reportflag |= 0x0F;
							
							if(*(para_p+1)) tag_state.reportflag |= 0xF0;
            
							back_up_reportflag_to_flash(tag_state.reportflag);
            
						}
						break;
						case CMD_NIGHT_MODE:
						{
							uint8 *para_p = pdata+1;
							if(*para_p == 0x00)
							{
								tag_state.status = TAG_STAT_NORMAL;
							}
							else
							{
								para_p++;
								tag_state.cfg_sleeptime = ((uint32)(*para_p)) |((uint32)(*(para_p+1))<<8)|\
                 ((uint32)(*(para_p+2))<<16) |((uint32)(*(para_p+3))<<24);
              
								para_p += 4;
								tag_state.check_cycle_s = ((uint16)(*para_p)) |((uint16)(*(para_p+1))<<8);
              
								tag_state.status = TAG_HOLD_FOR_EVE_MODE;
							}
               
						}
						break;
						case CMD_REBIND_NET:
						{
							uint8 *para_p = pdata+1;
            
							tag_state.shared_area[0] = (*para_p) |((uint16)(*(para_p+1))<<8);
							tag_state.shared_area[1] = (*(para_p+2)) |((uint16)(*(para_p+3))<<8);
							tag_state.shared_area[2] = *(para_p+4);           
						}
						break;
						default:break;
					}
        
          /*light led after refresh screen.*/
          tag_state.led.type = led;
          system_flag.led_state = tag_state.led.type;
					get_ledn_on();

          tag_state.ackflag |= (1<<FLAG_SHORT_DATA);
          update_ack_info(tag_state.ackflag);
          
          tag_state.state = STANDBY_INIT;
      }
			break; 
            
      case DATATYPE_SCREEN_DATE:{ 
          prf_wrscreen_info_t pwrscreeninfo;
          pdata += 1;// offset by dataType.
          pwrscreeninfo = (prf_wrscreen_info_t)(pdata);
		      
          screen.length = pwrscreeninfo->length;
          screen.writelen = screen.sliceseq =0; 

          req_slice = 0x4000;
					tag_state.state = WRITE_INIT;   	
      }
			break;
           
      case DATATYPE_FIREWARE_DATA:{
				  prf_firmware_info_t pfirmwareinfo;
          pdata += 1;
          pfirmwareinfo  = (prf_firmware_info_t)(pdata);
		      
          firmware.length = pfirmwareinfo->length;
          firmware.recv_check_sum = pfirmwareinfo->check_sum;
          firmware.soft_version =  pfirmwareinfo->soft_version;
                    
          firmware.writelen = firmware.sliceseq = screen.writelen = 0;
                
          req_slice = 0x8000;
          tag_state.state = WRITE_INIT;
      }
			break;
            
      case TAG_KICKE_OUT_FROM_NET:{ //go to do a atempt of re-find a zigbee PAN.
            
//           gui_refresh(IDX_FIND_NET_GUI);
            
           tag_state.state = NWK_FIND_INIT;
           AppRF_turn(NWK_FIND_INIT);				
      }
      break;
          
       case EXCEPTION_NOTICE_FLAG:
       {
					papp_rsp_excp_t exprep;
            pdata += 1;// offset by dataType.
            exprep = (papp_rsp_excp_t)(pdata);
            
            if(exprep->exp_code == BUZY_LATANCY){
                 tag_state.checkfactor = 0x83;//draw back 3s for a new ask,
                 tag_state.sleepflag = TRUE;
            }
            else if(exprep->exp_code == BAD_REQUEST){
                req_slice  =  0;
								
                tag_state.state = STANDBY_INIT;
          }
       }
			 break;
          
       default:break;
      } //switch end
}



uint8 App_SendBuzziEventRsp(uint8 cmd,uint8 status) 
{
	ErrorStatus trans_st = ERROR;
	uint8 permit_trans_times = TIMEOUT_RESP_TRANS;
        
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;
	prf_buz_rsp_t prsp = (prf_buz_rsp_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
	
	TxMpdu.fcf = FCF_NOACK_SRC_SHORT_DATA;
	TxMpdu.DestPanId = tag_flash_info.pan_id;
	TxMpdu.DestshortAddr = tag_flash_info.gateway_addr;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
	TxMpdu.ApduLen = sizeof(rf_header_t)+sizeof(rf_buz_rsp_t);
	
	phead->head = CMD_RESPONSE_HEAD;
	phead->cmd = NWK_RESP;
	phead->length = sizeof(rf_buz_rsp_t);
	 
	prsp->token = tag_state.token;
	prsp->ackCmd = cmd;
	prsp->event_type = tag_state.buz_type;
	prsp->event_cmd_type = tag_state.buz_cmd_type;
	//prsp->led_status = led_state & 0x0f;
	prsp->status = status;
	
	test_recv_time(1);
	prsp->cost_time =  get_test_recv_time();//(time_cost>>8) + (time_cost<<8);
		
	prsp->tail = TAIL;
        
        
	while(permit_trans_times)
	{
		trans_st = RfSendPacket();
           
		if(trans_st == SUCCESS) break;
	  
		permit_trans_times--;
	}
       
	return trans_st;   
}


uint8 App_SendTransparentMsg(uint8 evet_code) //
{
	ErrorStatus trans_st = ERROR;
	uint8 permit_trans_times = TIMEOUT_RESP_TRANS;
        
	prf_header_t phead = (prf_header_t)TxMpdu.ApduPayload;
	prf_pr_rsp_t prsp = (prf_pr_rsp_t)(TxMpdu.ApduPayload + sizeof(rf_header_t));
	uint8 *pdata = prsp->common_data;

	TxMpdu.fcf = FCF_NOACK_SRC_SHORT_DATA;
	TxMpdu.DestPanId = tag_flash_info.pan_id;
	TxMpdu.DestshortAddr = tag_flash_info.gateway_addr;
	TxMpdu.txSeqNumber = (uint8)TxState.frameCounter;
           
	phead->head = CMD_RESPONSE_HEAD;
	phead->cmd = NWK_RESP;
	 
	if(bz_resp.wait_flag){
		
  }else{
		bz_resp.wait_flag = 1;
		bz_resp.seq_no++;
	}
        
	prsp->token = tag_state.token;
	prsp->bz_seqno = bz_resp.seq_no;
	prsp->ackCmd = CMD_REPORT_TAG_DELIVER_MSG;
        
	prsp->sendcode = tag_state.cur_code ;
	
	
	if(evet_code == EVENT_KEY_PUT_PRESS){
		prsp->len= sizeof(tag_state.led.type);
		memcpy(prsp->common_data,(uint8*)&tag_state.led.type,sizeof(tag_state.led.type));
	}else if(evet_code == EVENT_INVENTORYMODE_CONFIRM){
		prsp->len = 4;
		memcpy(prsp->common_data,(uint8*)&state_para.pandian_num_block_num,4);
	}else{
		prsp->len = 0; //不携带附加信息。
  }
	
 
	TxMpdu.ApduLen = sizeof(rf_header_t)+(sizeof(rf_pr_rsp_t) - MAX_TRANS_PICKING_DATA_LEN+prsp->len) + 1;
	phead->length = (sizeof(rf_pr_rsp_t) - MAX_TRANS_PICKING_DATA_LEN+prsp->len);
        
	pdata += prsp->len;
	*pdata = TAIL;
        
	while(permit_trans_times)
	{
		trans_st = RfSendPacket();
           
		if(trans_st == SUCCESS) break;
		
		permit_trans_times--;
	}
       
	return trans_st;
}


