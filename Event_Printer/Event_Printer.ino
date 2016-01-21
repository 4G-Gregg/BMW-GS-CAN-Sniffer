
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
#ifdef DEBUG
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINT_LN(...) Serial.println(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINT_LN(...)
#endif

/* Globals */
MCP_CAN CAN(9);
K25_State_t motorcycle_state;

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
    byte value = LO_NIBBLE(data[1]);
    motorcycle_state.als = (K25_ALS_State)value;
    DEBUG_PRINT("Ambient Light Sensor ");
    if( value == 0x07 ) DEBUG_PRINT("DARK");
    else if( value == 0x03 ) DEBUG_PRINT("LIGHT");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_ABS_Control_Module(unsigned char *data, unsigned char length)
{
    // BRAKE LEVERS
    byte value = LO_NIBBLE(data[6]);
    motorcycle_state.brake_levers = (K25_Brake_Lever_State)value;
    DEBUG_PRINT("Brake Levers: ");
    if( value == 0x07 ) DEBUG_PRINT("Front");
    else if( value == 0x0B ) DEBUG_PRINT("Rear");
    else if( value == 0x03 ) DEBUG_PRINT("None");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");

    // ABS Status
    value = HI_NIBBLE(data[1]);
    motorcycle_state.abs_system = (K25_ABS_State)value;
    DEBUG_PRINT("ABS: ");
    if( value == 0x05 ) DEBUG_PRINT("ON");
    else if( value == 0x0B ) DEBUG_PRINT("OFF");
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
    byte value = LO_NIBBLE(data[4]);
    motorcycle_state.clutch = (K25_Clutch_Lever_State)value;
    DEBUG_PRINT("Clutch: ");
    if( value == 0x0A ) DEBUG_PRINT("IN");
    else if( value == 0x06 ) DEBUG_PRINT("OUT");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_ZFE_Control_Module(unsigned char *data, unsigned char length)
{
    // HIGH BEAM
    byte value = LO_NIBBLE(data[6]);
    motorcycle_state.high_beam = (K25_High_Beam_State)value;
    DEBUG_PRINT("High beam ");
    if( value == 0x09 ) DEBUG_PRINT("ON");
    else if( value == 0x0A ) DEBUG_PRINT("OFF");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");

    // TURN SIGNAL
    value = data[7];
    motorcycle_state.turn_signals = (K25_Turn_Signals_State)value;    
    DEBUG_PRINT("Turn Signals ");
    if( value == 0xCF ) DEBUG_PRINT("OFF");
    else if( value == 0xD7 ) DEBUG_PRINT("LEFT ONLY");
    else if( value == 0xE7 ) DEBUG_PRINT("RIGHT ONLY");
    else if( value == 0xEF ) DEBUG_PRINT("BOTH ON");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}

void handle_MSG_ID_ZFE_Control_Module_2(unsigned char *data, unsigned char length)
{
    // HEATED GRIPS
    byte value = HI_NIBBLE(data[7]);
    motorcycle_state.heated_grips = (K25_Heated_Grips_State)value;    
    DEBUG_PRINT("Heated Grips: ");
    if( value == 0x0F ) DEBUG_PRINT("OFF");
    else if( value == 0x0C ) DEBUG_PRINT("LOW");
    else if( value == 0x0D ) DEBUG_PRINT("HIGH");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");

    // INFO BUTTON
    value = HI_NIBBLE(data[5]);
    motorcycle_state.info_button = (K25_Info_Button_State)value;
    DEBUG_PRINT("Info Button: ");
    if( value == 0x05 ) DEBUG_PRINT("SHORT PRESS");
    else if( value == 0x06 ) DEBUG_PRINT("LONG PRESS");
    else DEBUG_PRINT("ERROR-Other");
    DEBUG_PRINT_LN("");
}
