
#include <SPI.h>
#include "mcp_can.h"
#include "BMW_R1200_GS_K25_CAN_Bus_Defines.h"
#include "BMW_R1200_GS_K25_State.h"

/* Compile time Flags */
#define DEBUG 0

/* Helper Macros */
#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

/* Debug Macros */
#if DEBUG
#define DEBUG_PRINT(x)    Serial.print(x)
#define DEBUG_PRINT_LN(x) Serial.println(x)
#elif
#define DEBUG_PRINT(x)
#define DEBUG_PRINT_LN(x)
#endif // DEBUG

MCP_CAN CAN(9);

/* Arduino Functions */
void setup()
{
    Serial.begin(115200);
    setup_CAN_Bus_Filters();
}

void loop()
{
  process_CAN_Messages();
}

void setup_CAN_Bus_Filters()
{
    while(CAN_OK != CAN.begin(CAN_500KBPS)){
        Serial.println("Failed to initialize CAN");
        Serial.println("Retrying..");
        delay(100);
    }

    Serial.println("Starting to initialize CAN");

    // Set filter masks
    CAN.init_Mask(0, 0, 0xfff);
    CAN.init_Mask(1, 0, 0xfff);

    // Set filters
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

void process_CAN_Messages()
{
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

void handle_MSG_ID_Instrument_Cluster(unsigned char *data, unsigned char length)
{
    DEBUG_PRINT("Instrument Cluster: ");
}

void handle_MSG_ID_Instrument_Cluster_2(unsigned char *data, unsigned char length)
{
    // AMBIENT LIGHT SENSOR
    unsigned char lnibble2 = LO_NIBBLE(data[1]);
    DEBUG_PRINT("Ambient Light Sensor ");
    if( lnibble2 == 0x07 ) DEBUG_PRINT("DARK");
    else if( lnibble2 == 0x03 ) DEBUG_PRINT("LIGHT");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_ABS_Control_Module(unsigned char *data, unsigned char length)
{
    // BRAKE LEVERS
    unsigned char lnibble7 = LO_NIBBLE(data[6]);
    DEBUG_PRINT("Brake Levers: ");
    if( lnibble7 == 0x07 ) DEBUG_PRINT("Front");
    else if( lnibble7 == 0x0B ) DEBUG_PRINT("Rear");
    else if( lnibble7 == 0x03 ) DEBUG_PRINT("None");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");

    // ABS Status
    unsigned char lnibble2 = HI_NIBBLE(data[1]);
    DEBUG_PRINT("ABS: ");
    if( lnibble2 == 0x05 ) DEBUG_PRINT("ON");
    else if( lnibble2 == 0x0B ) DEBUG_PRINT("OFF");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_BMSK_Control_Module(unsigned char *data, unsigned char length)
{
    // THROTTLE POSITION
    float byte2 = data[1];
    float throttlePosition = (byte2 / 255) * 100;
    DEBUG_PRINT("Throttle Position: ");
    DEBUG_PRINT(throttlePosition, DEC);
    DEBUG_PRINT_LN();

    // CLUTCH LEVER
    unsigned char hnibble5 = LO_NIBBLE(data[4]);
    DEBUG_PRINT("Clutch: ");
    if( hnibble5 == 0x0A ) DEBUG_PRINT("IN");
    else if( hnibble5 == 0x06 ) DEBUG_PRINT("OUT");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_ZFE_Control_Module(unsigned char *data, unsigned char length)
{
    // HIGH BEAM
    unsigned char lnibble7 = LO_NIBBLE(data[6]);
    DEBUG_PRINT("High beam ");
    if( lnibble7 == 0x09 ) DEBUG_PRINT("ON");
    else if( lnibble7 == 0x0A ) DEBUG_PRINT("OFF");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");

    // TURN SIGNAL
    unsigned char byte8 = data[7];
    DEBUG_PRINT("Turn Signals ");
    if( byte8 == 0xCF ) DEBUG_PRINT("OFF");
    else if( byte8 == 0xD7 ) DEBUG_PRINT("LEFT ONLY");
    else if( byte8 == 0xE7 ) DEBUG_PRINT("RIGHT ONLY");
    else if( byte8 == 0xEF ) DEBUG_PRINT("BOTH ON");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_ZFE_Control_Module_2(unsigned char *data, unsigned char length)
{
    // HEATED GRIPS
    unsigned char hnibble8 = HI_NIBBLE(data[7]);
    DEBUG_PRINT("Heated Grips: ");
    if( hnibble8 == 0x0F ) DEBUG_PRINT("OFF");
    else if( hnibble8 == 0x0C ) DEBUG_PRINT("LOW");
    else if( hnibble8 == 0x0D ) DEBUG_PRINT("HIGH");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");

    // INFO BUTTON
    unsigned char hnibble6 = HI_NIBBLE(data[5]);
    DEBUG_PRINT("Info Button: ");
    if( hnibble6 == 0x05 ) DEBUG_PRINT("SHORT PRESS");
    else if( hnibble6 == 0x06 ) DEBUG_PRINT("LONG PRESS");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}
