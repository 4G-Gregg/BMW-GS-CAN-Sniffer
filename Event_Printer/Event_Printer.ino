
#include <SPI.h>

#include <mcp_can.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include "BMW_R1200_GS_K25_CAN_Bus_Defines.h"
#include "BMW_R1200_GS_K25_State.h"

/* Compile time Flags */
#define DEBUG 0
#define PRINT_STATUS_TO_SERIAL_CONSOLE 0
#define PRINT_STATUS_TO_TFT 1

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

/* TFT Pin Defines */
#define TFT_CS     10
#define TFT_RST    7
#define TFT_DC     4

/* Globals */
MCP_CAN CAN(9);
K25_State_t motorcycle_state;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
bool status_changed = true;

/* Arduino Functions */
void setup()
{
  Serial.begin(115200);
  init_CAN_bus();
  init_display();
}

void loop()
{
  process_CAN_Messages();
  if (status_changed)
  {
      print_status();
      status_changed = false;
  }
}

/* Init Helpers */
void init_display()
{
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3); // rotate 90 degrees
  tft.fillScreen(ST7735_BLACK);
}

void init_CAN_bus()
{
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    DEBUG_PRINT_LN("Failed to initialize CAN");
    DEBUG_PRINT_LN("Retrying..");
    delay(100);
  }

  DEBUG_PRINT_LN("Starting to initialize CAN");

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

/* Event Processors */
void process_CAN_Messages()
{
    byte length = 0;
    byte data[8];
    byte new_value = 0;

     if(CAN_MSGAVAIL == CAN.checkReceive())
     {
        CAN.readMsgBuf(&length, data);
        switch(CAN.getCanId())
        {
          case MSG_ID_BMSK_Control_Module:
          {
              /*
            float new_throttle = K25_THROTTLE_PERCENTAGE(data[1]);
            if ( new_throttle != motorcycle_state.throttle_position )
            {
                status_changed = true;
                motorcycle_state.throttle_position = (K25_Throttle_Position_t)new_throttle;
            }
            */

            new_value = LO_NIBBLE(data[4]);
            if ( new_value != motorcycle_state.clutch )
            {
                status_changed = true;
                motorcycle_state.clutch = (K25_Clutch_Lever_State)new_value;
            }
          } break;

          case MSG_ID_BMSK_Control_Module_2:
          {

          } break;

          case MSG_ID_ZFE_Control_Module:
          {
              new_value = LO_NIBBLE(data[6]);
              if ( new_value != motorcycle_state.high_beam )
              {
                  status_changed = true;
                  motorcycle_state.high_beam = (K25_High_Beam_State)new_value;
              }

              new_value = data[7];
              if ( new_value != motorcycle_state.turn_signals )
              {
                  status_changed = true;
                  motorcycle_state.turn_signals = (K25_Turn_Signals_State)new_value;
              }
          } break;

          case MSG_ID_ZFE_Control_Module_2:
          {
              new_value = HI_NIBBLE(data[7]);
              if ( new_value != motorcycle_state.heated_grips )
              {
                  status_changed = true;
                  motorcycle_state.heated_grips = (K25_Heated_Grips_State)new_value;
              }

              new_value = HI_NIBBLE(data[5]);
              if ( new_value != motorcycle_state.info_button )
              {
                  status_changed = true;
                  motorcycle_state.info_button = (K25_Info_Button_State)new_value;
              }
          } break;

          case MSG_ID_ABS_Control_Module:
          {
              new_value = LO_NIBBLE(data[6]);
              if ( new_value != motorcycle_state.brake_levers )
              {
                  status_changed = true;
                  motorcycle_state.brake_levers = (K25_Brake_Lever_State)new_value;
              }

              new_value = HI_NIBBLE(data[1]);
              if ( new_value != motorcycle_state.abs_system )
              {
                  status_changed = true;
                  motorcycle_state.abs_system = (K25_ABS_State)new_value;
              }
          } break;

          case MSG_ID_ABS_Control_Module_2:
          {

          } break;

          case MSG_ID_Instrument_Cluster:
          {

          } break;

          case MSG_ID_Instrument_Cluster_2:
          {
              new_value = LO_NIBBLE(data[1]);
              if ( new_value != motorcycle_state.als )
              {
                  status_changed = true;
                  motorcycle_state.als = (K25_ALS_State)new_value;
              }
          } break;
        }
     }
}

/* Print Functions */
void print_status()
{
  String text = "";
  /*
  text += "Throttle: ";
  text += motorcycle_state.throttle_position;
  text += "%        \n";
  */

  text += "Heated Grips: ";
  if ( motorcycle_state.heated_grips == K25_Heated_Grips_State_off ) text += "OFF";
  else if ( motorcycle_state.heated_grips == K25_Heated_Grips_State_low ) text += "LOW";
  else if ( motorcycle_state.heated_grips == K25_Heated_Grips_State_high ) text += "HIGH";
  else text += "Unknown";
  text += "    \n";

  text += "Turn Signals: ";
  if ( motorcycle_state.turn_signals == K25_Turn_Signals_State_off ) text += "OFF";
  else if ( motorcycle_state.turn_signals == K25_Turn_Signals_State_left ) text += "LEFT";
  else if ( motorcycle_state.turn_signals == K25_Turn_Signals_State_right ) text += "RIGHT";
  else if ( motorcycle_state.turn_signals == K25_Turn_Signals_State_both ) text += "HAZARDS";
  else text += "Unknown";
  text += "    \n";

  text += "High Beam: ";
  if ( motorcycle_state.high_beam == K25_High_Beam_State_off ) text += "OFF";
  else if ( motorcycle_state.high_beam == K25_High_Beam_State_on ) text += "ON";
  else text += "Unknown";
  text += "       \n";

  /* INFO BUTTON CURRENTLY NOT WORKING
  text += "Info Button: ";
  if ( motorcycle_state.info_button == K25_Info_Button_State_short_press ) text += "SHORT";
  else if ( motorcycle_state.info_button == K25_Info_Button_State_long_press ) text += "LONG";
  else text += "Unknown";
  text += "\n";
  */

  text += "Clutch: ";
  if ( motorcycle_state.clutch == K25_Clutch_Lever_State_out ) text += "OUT";
  else if ( motorcycle_state.clutch == K25_Clutch_Lever_State_in ) text += "IN";
  else text += "Unknown";
  text += "       \n";

  text += "Brakes: ";
  if ( motorcycle_state.brake_levers == K25_Brake_Lever_State_none ) text += "NONE";
  else if ( motorcycle_state.brake_levers == K25_Brake_Lever_State_front ) text += "FRONT";
  else if ( motorcycle_state.brake_levers == K25_Brake_Lever_State_rear ) text += "REAR";
  else text += "Unknown";
  text += "       \n";

  text += "ABS: ";
  if ( motorcycle_state.abs_system == K25_ABS_State_off ) text += "OFF";
  else if ( motorcycle_state.abs_system == K25_ABS_State_on ) text += "ON";
  else text += "Unknown";
  text += "       \n";
/*
  text += "ALS: ";
  if ( motorcycle_state.als == K25_ALS_State_dark ) text += "DARK";
  else if ( motorcycle_state.als == K25_ALS_State_light ) text += "LIGHT";
  else text += "Unknown";
  text += "       \n";
*/

#if PRINT_STATUS_TO_SERIAL_CONSOLE
  Serial.println(text);
#endif

#if PRINT_STATUS_TO_TFT
  draw_text(&text, ST7735_WHITE, ST7735_BLACK);
#endif
}

void draw_text(String *text, uint16_t color, uint16_t background_color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color, background_color);
  tft.setTextWrap(true);
  tft.print(text->c_str());
}
