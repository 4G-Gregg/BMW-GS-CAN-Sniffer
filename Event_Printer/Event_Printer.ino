
#include <SPI.h>
#include <mcp_can.h>
#include "BMW_R1200_GS_K25_CAN_Bus_Defines.h"
#include "BMW_R1200_GS_K25_State.h"

/* Compile time Flags */
#define DEBUG 0
#define PRINT_TO_SERIAL_CONSOLE 1

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
#if PRINT_TO_SERIAL_CONSOLE
    print_motorcycle_state();
#endif
}

void print_motorcycle_state()
{
    Serial.print("Throttle Position: ");
    Serial.print(motorcycle_state.throttle_position, DEC);
    Serial.println();

    Serial.print("Heated Grips: ");
    if( motorcycle_state.heated_grips == K25_Heated_Grips_State_off ) Serial.print("OFF");
    else if( motorcycle_state.heated_grips == K25_Heated_Grips_State_low ) Serial.print("LOW");
    else if( motorcycle_state.heated_grips == K25_Heated_Grips_State_high ) Serial.print("HIGH");
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("Turn Signals: ");
    if( motorcycle_state.turn_signals == K25_Turn_Signals_State_off ) Serial.print("OFF");
    else if( motorcycle_state.turn_signals == K25_Turn_Signals_State_left ) Serial.print("LEFT");
    else if( motorcycle_state.turn_signals == K25_Turn_Signals_State_right ) Serial.print("RIGHT");
    else if( motorcycle_state.turn_signals == K25_Turn_Signals_State_both ) Serial.print("HAZARDS");    
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("High Beam: ");
    if( motorcycle_state.high_beam == K25_High_Beam_State_off ) Serial.print("OFF");
    else if( motorcycle_state.high_beam == K25_High_Beam_State_on ) Serial.print("ON");   
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("Info Button: ");
    if( motorcycle_state.info_button == K25_Info_Button_State_short_press ) Serial.print("SHORT");
    else if( motorcycle_state.info_button == K25_Info_Button_State_long_press ) Serial.print("LONG");   
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("Clutch: ");
    if( motorcycle_state.clutch == K25_Clutch_Lever_State_out ) Serial.print("OUT");
    else if( motorcycle_state.clutch == K25_Clutch_Lever_State_in ) Serial.print("IN");   
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("Brakes: ");
    if( motorcycle_state.brake_levers == K25_Brake_Lever_State_none ) Serial.print("NONE");
    else if( motorcycle_state.brake_levers == K25_Brake_Lever_State_front ) Serial.print("FRONT");  
    else if( motorcycle_state.brake_levers == K25_Brake_Lever_State_rear ) Serial.print("REAR");  
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("ABS: ");
    if( motorcycle_state.abs_system == K25_ABS_State_off ) Serial.print("OFF");
    else if( motorcycle_state.abs_system == K25_ABS_State_on ) Serial.print("ON");  
    else Serial.print("Unknown"); 
    Serial.println();

    Serial.print("ALS: ");
    if( motorcycle_state.als == K25_ALS_State_dark ) Serial.print("DARK");
    else if( motorcycle_state.als == K25_ALS_State_light ) Serial.print("LIGHT");  
    else Serial.print("Unknown"); 
    Serial.println();
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
}

void handle_MSG_ID_ABS_Control_Module(unsigned char *data, unsigned char length)
{
    // BRAKE LEVERS
    byte value = LO_NIBBLE(data[6]);
    motorcycle_state.brake_levers = (K25_Brake_Lever_State)value;

    // ABS Status
    value = HI_NIBBLE(data[1]);
    motorcycle_state.abs_system = (K25_ABS_State)value;
}

void handle_MSG_ID_BMSK_Control_Module(unsigned char *data, unsigned char length)
{
    // THROTTLE POSITION
    float fvalue = data[1];
    motorcycle_state.throttle_position = (K25_Throttle_Position_t)K25_THROTTLE_PERCENTAGE(fvalue);

    // CLUTCH LEVER
    byte value = LO_NIBBLE(data[4]);
    motorcycle_state.clutch = (K25_Clutch_Lever_State)value;
}

void handle_MSG_ID_ZFE_Control_Module(unsigned char *data, unsigned char length)
{
    // HIGH BEAM
    byte value = LO_NIBBLE(data[6]);
    motorcycle_state.high_beam = (K25_High_Beam_State)value;

    // TURN SIGNAL
    value = data[7];
    motorcycle_state.turn_signals = (K25_Turn_Signals_State)value;    
}

void handle_MSG_ID_ZFE_Control_Module_2(unsigned char *data, unsigned char length)
{
    // HEATED GRIPS
    byte value = HI_NIBBLE(data[7]);
    motorcycle_state.heated_grips = (K25_Heated_Grips_State)value;    

    // INFO BUTTON
    value = HI_NIBBLE(data[5]);
    motorcycle_state.info_button = (K25_Info_Button_State)value;
}
