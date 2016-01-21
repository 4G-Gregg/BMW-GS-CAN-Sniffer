
#define THROTTLE_PERCENTAGE(x) ( x / 255 ) * 100

enum Heated_Grips_State
{
    off  = 0x0F,
    low  = 0x0C,
    high = 0x0D
}

enum Turn_Signals_State
{
    off   = 0xCF,
    left  = 0xD7,
    right = 0xE7,
    both  = 0xEF
}

enum High_Beam_State
{
    off = 0x0A,
    on  = 0x09
}

enum Info_Button_State
{
    short_press = 0x05,
    long_press = 0x06
}

enum Clutch_Lever_State
{
    out = 0x06,
    in  = 0x0A
}

enum Brake_Lever_State
{
    none  = 0x03,
    front = 0x07,
    rear  = 0x0B
}

enum ABS_State
{
    off = 0x0B,
    on  = 0x05
}

enum Ambient_Light_Sensor_State
{
    dark  = 0x07,
    light = 0x03
}
