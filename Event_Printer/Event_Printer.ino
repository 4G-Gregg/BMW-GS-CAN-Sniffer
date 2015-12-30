
#include <SPI.h>
#include "mcp_can.h"
#include "BMW_R1200_GS_K25_CAN_Bus_Defines.h"

MCP_CAN CAN(9);

void setup(){
    Serial.begin(115200);

START_INIT:
    if(CAN_OK != CAN.begin(CAN_500KBPS)){
        Serial.println("Failed to initialize CAN");
        Serial.println("Retrying..");
        delay(100);
        goto START_INIT;
    }
   
    //Set filter masks
    CAN.init_Mask(0, 0, 0xfff);
    CAN.init_Mask(1, 0, 0xfff);

    //Set filters
    CAN.init_Filt(0, 0, MSG_ID_BMSK_Control_Module);
    CAN.init_Filt(1, 0, MSG_ID_BMSK_Control_Module_2);    
    CAN.init_Filt(2, 0, MSG_ID_ZFE_Control_Module);    
    CAN.init_Filt(3, 0, MSG_ID_ZFE_Control_Module_2);   
    CAN.init_Filt(4, 0, MSG_ID_ABS_Control_Module);
    CAN.init_Filt(5, 0, MSG_ID_ABS_Control_Module_2);    
    CAN.init_Filt(6, 0, MSG_ID_Instrument_Cluster);
    CAN.init_Filt(7, 0, MSG_ID_Instrument_Cluster_2);    
    
}


void loop(){
    unsigned char length = 0;
    unsigned char data[8];

     if(CAN_MSGAVAIL == CAN.checkReceive())
     {
        CAN.readMsgBuf(&length, data);
        switch(CAN.getCanId())
        {
          case MSG_ID_BMSK_Control_Module:
          {
            
          } break;
          
          case MSG_ID_BMSK_Control_Module_2:
          {
            
          } break;

          case MSG_ID_ZFE_Control_Module:
          {
            
          } break;
          
          case MSG_ID_ZFE_Control_Module_2:
          {
            
          } break;
          
          case MSG_ID_ABS_Control_Module:
          {
            
          } break;
          
          case MSG_ID_ABS_Control_Module_2:
          {
            
          } break;
          
          case MSG_ID_Instrument_Cluster:
          {
            
          } break;
          
          case MSG_ID_Instrument_Cluster_2:
          {
            
          } break;
          
        }
     }

/*
    if(CAN_MSGAVAIL == CAN.checkReceive()){
        CAN.readMsgBuf(&length, data);
        Serial.print(CAN.getCanId(),HEX);
        for(int i = 0; i<length; i++){
            Serial.print(",");
            if( data[i] < 0x10){ Serial.print("0");}
            Serial.print(data[i], HEX);
        }
        Serial.print(";");
    }
*/
}
