
#include "scan.h"
#include "uart.h"
#include "open_interface.h"
#include "music.h"
#include "commands.h"
#include "servo.h"
#include "temp.h"



void main(void)
{
    timer_init();
    temp_sensor_init();
    button_init();
    lcd_init();
    while(1){
        if(button_getButton() == 1){
            lcd_printf("Hello!");
            i2c_write_byte(0xB4);
        }
    }
}
