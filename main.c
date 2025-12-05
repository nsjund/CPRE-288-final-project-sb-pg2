
#include "scan.h"
#include "uart.h"
#include "open_interface.h"
#include "music.h"
#include "commands.h"
#include "servo.h"
#include "temp.h"


void main(void)
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    uart_init(67);
    temp_sensor_init();

    char recived = 0;

    char msg[30];
    int index = 0;

    scan_point_t scan_points[91] = {0};
    scan_point_t raw_data[91] = {0};

    char end = 0;

    object_t object_list[5] = {0};

    cyBOT_Scan_t scan;
    cyBOT_Scan(0, &scan);

    s_pos self;
    self.pos_x = 0;
    self.pos_y = 0;
    self.heading = 90;

    uint8_t stored_count = 0;
    s_object_t stored_objects[10] = {0};

    while(!end){
        index = 0;
        recived = uart_receive();
        while(recived != '\n'){
            msg[index++] = recived;
            recived = uart_receive();
        }
        recived = msg[0];
        switch (recived){
            case 0x04:
                run_auto_mode(scan_points, object_list, raw_data, sensor_data, stored_objects, &stored_count, &self);
                break;
            case 0x05:
                run_object_scan(scan_points, object_list, raw_data, sensor_data, stored_objects, &stored_count, &self);
                break;
            case 0x06:
                run_calibrate_servo();
                break;
            case 0x07:
                run_play_music(sensor_data);
                break;
            case 0x08:
                //run_test_1();
                break;
            case 0x09:
                //run_test_2();
                break;
            case 0x11:
                run_calibrate_ir();
                break;
            case 0x0B:
                run_move_forward(sensor_data, &self);
                break;
            case 0x0C:
                run_turn_left(sensor_data, &self);
                break;
            case 0x0D:
                run_turn_right(sensor_data, &self);
                break;
            case 0x0E:
                run_get_status(&self, stored_objects, stored_count);
                break;
            case 0x0F:
                run_basic_scan(raw_data);
                break;
            case 0x10:
                run_dist_ahead();
                break;
            case 0x03:
                run_end(&end, sensor_data);
                break;
        }

    }



}