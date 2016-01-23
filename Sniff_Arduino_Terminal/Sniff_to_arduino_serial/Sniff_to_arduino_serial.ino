
#include <SPI.h>
#include "mcp_can.h"

MCP_CAN CAN(9);

void setup(){
    Serial.begin(115200);

START_INIT:
    if(CAN_OK != CAN.begin(CAN_500KBPS)){
        delay(100);
        goto START_INIT;
    }

    //Set filter masks
    CAN.init_Mask(0, 0, 0xfff);
    CAN.init_Mask(1, 0, 0xfff);

    //Set filters
    // Look for button presses
    CAN.init_Filt(0, 0, 0x2D0); // ZFE
    CAN.init_Filt(1, 0, 0x294); // ABS
    CAN.init_Filt(2, 0, 0x130); // ZFE 2
    /*
    CAN.init_Filt(0, 0, 0x10C);
    CAN.init_Filt(1, 0, 0x130);
    CAN.init_Filt(2, 0, 0x294);
    CAN.init_Filt(3, 0, 0x2BC);
    CAN.init_Filt(4, 0, 0x2D0);
    CAN.init_Filt(5, 0, 0x3F8);
    CAN.init_Filt(6, 0, 0x2A8);
    CAN.init_Filt(7, 0, 0x3FF);
    */
    // Print in CSV format
    Serial.println("time, CAN-ID, b0, b1, b2, b3, b4, b5, b6, b7");
    Serial.println();
}


void loop(){
    unsigned char length = 0;
    unsigned char data[8];

    // Print in CSV format
    if(CAN_MSGAVAIL == CAN.checkReceive()){
        CAN.readMsgBuf(&length, data);
        Serial.print(millis());
        Serial.print(",");
        Serial.print(CAN.getCanId(),HEX);
        for(int i = 0; i<length; i++){
            Serial.print(",");
            if( data[i] < 0x10){ Serial.print("0");}
            Serial.print(data[i], HEX);
        }
        Serial.println("");
    }
}
