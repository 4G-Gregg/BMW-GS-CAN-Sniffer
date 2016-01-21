
#include <SPI.h>
#include "mcp_can.h"
#include "BMW_R1200_GS_K25_CAN_Bus_Defines.h"

#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

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

    Serial.println("Starting to initialize CAN");
   
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
    Serial.println("CAN Initialized");
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
            handle_MSG_ID_BMSK_Control_Module(data, length);
          } break;
          
          case MSG_ID_BMSK_Control_Module_2:
          {
            
          } break;

          case MSG_ID_ZFE_Control_Module:
          {
            handle_MSG_ID_ZFE_Control_Module(data, length);
          } break;
          
          case MSG_ID_ZFE_Control_Module_2:
          {
            handle_MSG_ID_ZFE_Control_Module_2(data, length);
          } break;
          
          case MSG_ID_ABS_Control_Module:
          {
           handle_MSG_ID_ABS_Control_Module(data, length);
          } break;
          
          case MSG_ID_ABS_Control_Module_2:
          {
           
          } break;
          
          case MSG_ID_Instrument_Cluster:
          {
            handle_MSG_ID_Instrument_Cluster(data, length);
          } break;
          
          case MSG_ID_Instrument_Cluster_2:
          {
            handle_MSG_ID_Instrument_Cluster_2(data, length);
          } break;   
        }
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

void handle_MSG_ID_Instrument_Cluster(unsigned char *data, unsigned char length)
{
    Serial.print("Instrument Cluster: ");
    
}

void handle_MSG_ID_Instrument_Cluster_2(unsigned char *data, unsigned char length)
{
    // AMBIENT LIGHT SENSOR
    unsigned char lnibble2 = LO_NIBBLE(data[1]);
    Serial.print("Ambient Light Sensor ");
    if( lnibble2 == 0x07 ) Serial.print("DARK");
    else if( lnibble2 == 0x03 ) Serial.print("LIGHT");
    else Serial.print("ERROR-Other");
    Serial.println("");   
}

void handle_MSG_ID_ABS_Control_Module(unsigned char *data, unsigned char length)
{
    // BRAKE LEVERS
    unsigned char lnibble7 = LO_NIBBLE(data[6]);
    Serial.print("Brake Levers: ");
    if( lnibble7 == 0x07 ) Serial.print("Front");
    else if( lnibble7 == 0x0B ) Serial.print("Rear");
    else if( lnibble7 == 0x03 ) Serial.print("None");    
    else Serial.print("ERROR-Other");
    Serial.println("");

    // ABS Status
    unsigned char lnibble2 = HI_NIBBLE(data[1]);
    Serial.print("ABS: ");
    if( lnibble2 == 0x05 ) Serial.print("ON");
    else if( lnibble2 == 0x0B ) Serial.print("OFF");
    else Serial.print("ERROR-Other");
    Serial.println("");
}

void handle_MSG_ID_BMSK_Control_Module(unsigned char *data, unsigned char length)
{
    // THROTTLE POSITION
    float byte2 = data[1];
    float throttlePosition = (byte2 / 255) * 100;
    Serial.print("Throttle Position: ");
    Serial.print(throttlePosition, DEC);
    Serial.println(); 

    // CLUTCH LEVER
    unsigned char hnibble5 = LO_NIBBLE(data[4]);
    Serial.print("Clutch: ");
    if( hnibble5 == 0x0A ) Serial.print("IN");
    else if( hnibble5 == 0x06 ) Serial.print("OUT");
    else Serial.print("ERROR-Other");
    Serial.println("");
}

void handle_MSG_ID_ZFE_Control_Module(unsigned char *data, unsigned char length)
{
    // HIGH BEAM
    unsigned char lnibble7 = LO_NIBBLE(data[6]);
    Serial.print("High beam ");
    if( lnibble7 == 0x09 ) Serial.print("ON");
    else if( lnibble7 == 0x0A ) Serial.print("OFF");
    else Serial.print("ERROR-Other");
    Serial.println("");

    // TURN SIGNAL
    unsigned char byte8 = data[7];
    Serial.print("Turn Signals ");    
    if( byte8 == 0xCF ) Serial.print("OFF");
    else if( byte8 == 0xD7 ) Serial.print("LEFT ONLY");
    else if( byte8 == 0xE7 ) Serial.print("RIGHT ONLY");
    else if( byte8 == 0xEF ) Serial.print("BOTH ON");
    else Serial.print("ERROR-Other");
    Serial.println("");
}

void handle_MSG_ID_ZFE_Control_Module_2(unsigned char *data, unsigned char length)
{
    // HEATED GRIPS
    unsigned char hnibble8 = HI_NIBBLE(data[7]);
    Serial.print("Heated Grips: ");
    if( hnibble8 == 0x0F ) Serial.print("OFF");
    else if( hnibble8 == 0x0C ) Serial.print("LOW");
    else if( hnibble8 == 0x0D ) Serial.print("HIGH");    
    else Serial.print("ERROR-Other");
    Serial.println("");

    // INFO BUTTON
    unsigned char hnibble6 = HI_NIBBLE(data[5]);
    Serial.print("Info Button: ");
    if( hnibble6 == 0x05 ) Serial.print("SHORT PRESS");
    else if( hnibble6 == 0x06 ) Serial.print("LONG PRESS");   
    else Serial.print("ERROR-Other");
    Serial.println("");
}


