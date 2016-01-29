
#include <SPI.h>

#include <mcp_can.h>

#include "BMW_R1200_GS_K25_CAN_Bus_Defines.h"
#include "BMW_R1200_GS_K25_State.h"

/* Compile time Flags */
#define DEBUG 0
#define PRINT_STATUS_TO_SERIAL_CONSOLE 1

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

/* PIN Defines */
#define AUX_LIGHT_1_POWER_CONTROL_PIN 7
#define AUX_LIGHT_1_PWM_PIN 5
#define AUX_LIGHT_2_POWER_CONTROL_PIN 4
#define AUX_LIGHT_2_PWM_PIN 6

#define AUX_LIGHT_PWM_BRIGHTNESS_OFF  0
#define AUX_LIGHT_PWM_BRIGHTNESS_LOW  170 // TODO: Determine the correct value here to achieve 8v
#define AUX_LIGHT_PWM_BRIGHTNESS_HIGH 255

/* Globals */
MCP_CAN CAN(9);
K25_State_t motorcycle_state;
Heated_Jacket_State_t heated_jacket_state = Heated_Jacket_State_Off;
Aux_Light_State_t aux_light_state = Aux_Light_Off;
bool status_changed = true;
bool abs_button_up_event = false;

/* Arduino Functions */
void setup()
{
  Serial.begin(115200);
  init_aux_light_controller();
  init_CAN_bus();
}

void loop()
{
  process_CAN_Messages();

  if ( status_changed )
  {
      set_aux_light_state();
      if ( motorcycle_state.heated_grips == K25_Heated_Grips_State_off ) set_heated_jacket_off();
      if ( abs_button_up_event ) handle_abs_button_event();
      if ( PRINT_STATUS_TO_SERIAL_CONSOLE ) print_status();
      status_changed = false;
  }
}

/* Init Helpers */
void init_CAN_bus()
{
  while ( CAN_OK != CAN.begin(CAN_500KBPS) ) {
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
  CAN.init_Filt(1, 0, MSG_ID_ZFE_Control_Module);
  CAN.init_Filt(2, 0, MSG_ID_ZFE_Control_Module_2);
  CAN.init_Filt(3, 0, MSG_ID_Instrument_Cluster_2);

  Serial.println("CAN Initialized");
}

void init_aux_light_controller()
{
  // Set control pins to outputs
  pinMode(AUX_LIGHT_1_POWER_CONTROL_PIN, OUTPUT);
  pinMode(AUX_LIGHT_1_PWM_PIN, OUTPUT);
  pinMode(AUX_LIGHT_2_POWER_CONTROL_PIN, OUTPUT);
  pinMode(AUX_LIGHT_2_PWM_PIN, OUTPUT);  

  // Initially set them to off
  set_aux_light_state_off();
}

/* Event Processors */
void process_CAN_Messages()
{
    byte length = 0;
    byte data[8];
    byte new_value = 0;

     if( CAN_MSGAVAIL == CAN.checkReceive() )
     {
        CAN.readMsgBuf(&length, data);
        switch( CAN.getCanId() )
        {
          case MSG_ID_BMSK_Control_Module:
          {
              new_value = LO_NIBBLE(data[5]);
              if ( new_value != motorcycle_state.abs_button )
              {
                status_changed = true;
                if ( motorcycle_state.abs_button == K25_ABS_Button_State_on &&
                     new_value == K25_ABS_Button_State_off )
                {
                    abs_button_up_event = true;
                }
                motorcycle_state.abs_button = (K25_ABS_Button_State)new_value;
              }
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

/* State Change Functions */
void handle_abs_button_event()
{
    if (motorcycle_state.heated_grips != K25_Heated_Grips_State_off)
    {
        switch (heated_jacket_state)
        {
            case Heated_Jacket_State_Off:
            {
                heated_jacket_state = Heated_Jacket_State_1;
            } break;

            case Heated_Jacket_State_1:
            {
                heated_jacket_state = Heated_Jacket_State_2;
            } break;

            case Heated_Jacket_State_2:
            {
                heated_jacket_state = Heated_Jacket_State_3;
            } break;

            case Heated_Jacket_State_3:
            {
                heated_jacket_state = Heated_Jacket_State_4;
            } break;

            case Heated_Jacket_State_4:
            {
                heated_jacket_state = Heated_Jacket_State_5;
            } break;

            case Heated_Jacket_State_5:
            {
                heated_jacket_state = Heated_Jacket_State_Off;
            } break;
        }
        set_heated_jacket_state();
    }
    abs_button_up_event = false;
}

void set_aux_light_state()
{
    // Day Time
    if ( motorcycle_state.als == K25_ALS_State_light )
    {
        set_aux_light_state_high();
    }
    // Night Time
    else
    {
        // Night Time but high beams are on...
        if (motorcycle_state.high_beam == K25_High_Beam_State_on)
        {
            set_aux_light_state_high();
        }
        else
        {
            set_aux_light_state_low();
        }
    }
}

void set_aux_light_state_high()
{
    if ( aux_light_state != Aux_Light_High )
    {
        aux_light_state = Aux_Light_High;
        digitalWrite(AUX_LIGHT_1_POWER_CONTROL_PIN, HIGH);
        digitalWrite(AUX_LIGHT_2_POWER_CONTROL_PIN, LOW);
        analogWrite(AUX_LIGHT_1_PWM_PIN, AUX_LIGHT_PWM_BRIGHTNESS_HIGH);
        analogWrite(AUX_LIGHT_2_PWM_PIN, AUX_LIGHT_PWM_BRIGHTNESS_HIGH);
    }
}

void set_aux_light_state_low()
{
    if ( aux_light_state != Aux_Light_Low )
    {
        aux_light_state = Aux_Light_Low;
        digitalWrite(AUX_LIGHT_1_POWER_CONTROL_PIN, HIGH);
        digitalWrite(AUX_LIGHT_2_POWER_CONTROL_PIN, LOW);
        analogWrite(AUX_LIGHT_1_PWM_PIN, AUX_LIGHT_PWM_BRIGHTNESS_LOW);
        analogWrite(AUX_LIGHT_2_PWM_PIN, AUX_LIGHT_PWM_BRIGHTNESS_LOW);
    }
}

void set_aux_light_state_off()
{
    if ( aux_light_state != Aux_Light_Off )
    {
        aux_light_state = Aux_Light_Off;
        digitalWrite(AUX_LIGHT_1_POWER_CONTROL_PIN, LOW);
        digitalWrite(AUX_LIGHT_2_POWER_CONTROL_PIN, HIGH);
        analogWrite(AUX_LIGHT_1_PWM_PIN, AUX_LIGHT_PWM_BRIGHTNESS_OFF);
        analogWrite(AUX_LIGHT_2_PWM_PIN, AUX_LIGHT_PWM_BRIGHTNESS_OFF);
    }
}

void set_heated_jacket_state()
{
    /*
    // heated jacket can only be on if heated grips are on
    if ( motorcycle_state.heated_grips != K25_Heated_Grips_State_off )
    {
        if ( heated_jacket_state == Heated_Jacket_State_Off )
        {
            set_heated_jacket_level(1);
            // NOTE: The heated jacket controller was doing PWM at 1 Hz. This should try to be replicated here.
            // NOTE: The following example seems to do just that: http://playground.arduino.cc/Code/Timer1
        }
    }
    else
    {
        set_heated_jacket_off();
    }
    */
}

void set_heated_jacket_off()
{
    // TODO: Set heated jacket state to off
}

/* Print Functions */
void print_status()
{
  String text = "Heated Grips: ";
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

  text += "ABS Button: ";
  if ( motorcycle_state.abs_button == K25_ABS_Button_State_off ) text += "OFF";
  else if ( motorcycle_state.abs_button == K25_ABS_Button_State_on ) text += "ON";
  else text += "Unknown";
  text += "       \n" ;

  text += "ALS: ";
  if ( motorcycle_state.als == K25_ALS_State_dark ) text += "DARK";
  else if ( motorcycle_state.als == K25_ALS_State_light ) text += "LIGHT";
  else text += "Unknown";
  text += "       \n";

  text += "Aux Lights: ";
  if ( aux_light_state == Aux_Light_Off ) text += "OFF";
  else if ( aux_light_state == Aux_Light_Low ) text += "LOW";
  else if ( aux_light_state == Aux_Light_Low ) text += "HIGH";
  text += "   \n";

  text += "Jacket: ";
  if ( heated_jacket_state == Heated_Jacket_State_Off ) text += "OFF";
  else if ( heated_jacket_state == Heated_Jacket_State_1 ) text += "1";
  else if ( heated_jacket_state == Heated_Jacket_State_2 ) text += "2";
  else if ( heated_jacket_state == Heated_Jacket_State_3 ) text += "3";
  else if ( heated_jacket_state == Heated_Jacket_State_4 ) text += "4";
  else if ( heated_jacket_state == Heated_Jacket_State_5 ) text += "5";
  text += "   \n";

  Serial.println(text);
}
