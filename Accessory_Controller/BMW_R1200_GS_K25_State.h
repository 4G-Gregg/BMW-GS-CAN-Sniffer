
#define K25_THROTTLE_PERCENTAGE(x) ( x / 255 ) * 100

typedef float K25_Throttle_Position_t;

enum K25_Heated_Grips_State
{
    K25_Heated_Grips_State_off  = 0x0C,
    K25_Heated_Grips_State_low  = 0x0D,
    K25_Heated_Grips_State_high = 0x0E
};

enum K25_Turn_Signals_State
{
    K25_Turn_Signals_State_off   = 0xCF,
    K25_Turn_Signals_State_left  = 0xD7,
    K25_Turn_Signals_State_right = 0xE7,
    K25_Turn_Signals_State_both  = 0xEF
};

enum K25_High_Beam_State
{
    K25_High_Beam_State_off = 0x0A,
    K25_High_Beam_State_on  = 0x09
};

enum K25_Info_Button_State
{
    K25_Info_Button_State_short_press = 0x05,
    K25_Info_Button_State_long_press  = 0x06
};

enum K25_ABS_Button_State
{
    K25_ABS_Button_State_off = 0x05,
    K25_ABS_Button_State_on  = 0x06
};

enum K25_Clutch_Lever_State
{
    K25_Clutch_Lever_State_out = 0x06,
    K25_Clutch_Lever_State_in  = 0x0A
};

enum K25_Brake_Lever_State
{
    K25_Brake_Lever_State_none  = 0x03,
    K25_Brake_Lever_State_front = 0x07,
    K25_Brake_Lever_State_rear  = 0x0B
};

enum K25_ABS_State
{
    K25_ABS_State_off = 0x0B,
    K25_ABS_State_on  = 0x05
};

enum K25_ALS_State
{
    K25_ALS_State_dark  = 0x0B,
    K25_ALS_State_light = 0x07
};

typedef struct
{
    K25_Throttle_Position_t throttle_position;
    K25_Heated_Grips_State heated_grips;
    K25_Turn_Signals_State turn_signals;
    K25_High_Beam_State high_beam;
    K25_Info_Button_State info_button;
    K25_Clutch_Lever_State clutch;
    K25_Brake_Lever_State brake_levers;
    K25_ABS_State abs_system;
    K25_ALS_State als;
    K25_ABS_Button_State abs_button;
} K25_State_t;

typedef enum Heated_Jacket_State
{
    Heated_Jacket_State_Off,
    Heated_Jacket_State_1,
    Heated_Jacket_State_2,
    Heated_Jacket_State_3,
    Heated_Jacket_State_4,
    Heated_Jacket_State_5
} Heated_Jacket_State_t;

typedef enum
{
    Aux_Light_Off,
    Aux_Light_Low,
    Aux_Light_High
} Aux_Light_State_t;
