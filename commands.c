#include "commands.h"

char msg[40] = {0};

cyBOT_Scan_t scan;

void run_auto_mode(scan_point_t *scan_points, object_t *object_list, scan_point_t *raw_data, oi_t *sensor_data, s_object_t *stored_objects, uint8_t *stored_count, s_pos *self){
    int object_count = do_scan_filter(scan_points, object_list, raw_data, sensor_data, stored_objects, stored_count, self);
    go_to_smallest(object_list, stored_count, sensor_data, self, stored_objects);
    cyBOT_Scan(90, &scan);
    print_objects(object_list, object_count);
    uart_sendStr("Cybot\rX_pos\tY_pos\tHeading\r");
    sprintf(msg, "%.2f\t%.2f\t%.0f\r", self->pos_x, self->pos_y, self->heading);
    uart_sendStr(msg);
    print_stored(stored_objects, *stored_count, self);
}

void run_object_scan(scan_point_t *scan_points, object_t *object_list, scan_point_t *raw_data, oi_t *sensor_data, s_object_t *stored_objects, uint8_t *stored_count, s_pos *self){
    int object_count = do_scan_filter(scan_points, object_list, raw_data, sensor_data, stored_objects, stored_count, self);
    char hot_found = 0;
    for(int i = 0; i <= 90; i++){
        if(scan_points->temp > 75.0){
            hot_found = 1;
        }
    }
    cyBOT_Scan(90, &scan);
    uart_sendChar(0xF0 + hot_found);
}

void run_calibrate_servo(){
    cyBOT_SERVO_cal();
}

void run_play_music(oi_t *sensor_data){
    short length = sizeof(midi_data[0]);
    play_song(midi_data[0], midi_data[1], length, sensor_data);
    uart_sendChar(0x07);
}

void run_test_1(s_pos *self){
    scan_init(0x08);
    cyBOT_Scan(90, &scan);
    uint16_t ir = adc_read();
    //sprintf(msg, "raw: %d, ir dist: %f, sonar dist: %f\r", ir, get_dist_from_IR(ir), scan.sound_dist * 10);
    //uart_sendStr(msg);
    uart_sendChar('\n');
}

void run_test_2(oi_t *sensor_data, s_pos *self){
    oi_setWheels(100, -100);
    for(int i = 0; i < 100; i++){
        oi_update(sensor_data);
        self->heading += sensor_data->angle;
        sprintf(msg, "%lf\r", sensor_data->angle);
        uart_sendStr(msg);
    }
    sprintf(msg, "%f\r", self->heading);
    uart_sendStr(msg);
    uart_sendChar('\n');
    oi_setWheels(0, 0);
}

void run_calibrate_ir(){
    oi_setWheels(50, 50);
    tx_float_t dist;
    tx_short_t ir;
    scan_init(0x7);
    uart_sendChar(0x11);
    for(int i = 0; i < 100; i++){
        cyBOT_Scan(90, &scan);
        lcd_printf("%f", scan.sound_dist);
        if(scan.sound_dist < 15){
            oi_setWheels(-50, -50);
        }else if(scan.sound_dist > 110){
            oi_setWheels(50, 50);
        }
        dist.f = scan.sound_dist * 10;//times 10 to convert to mm
        ir.s = (short)scan.IR_raw_val;
        send_short(ir);
        send_float(dist);

    }
    oi_setWheels(0, 0);
    tx_float_t rx;
    for(int i = 0; i < 4; i++){
        rx.c[i] = uart_receive();
    }
    p1 = rx.f;
    for(int i = 0; i < 4; i++){
        rx.c[i] = uart_receive();
    }
    p2 = rx.f;
    for(int i = 0; i < 4; i++){
        rx.c[i] = uart_receive();
    }
    p3 = rx.f;

}

void run_move_forward(oi_t *sensor_data, s_pos *self){
    move_forward(sensor_data, 50, 200, self);
    uart_sendChar(0x0B);
}

void run_turn_left(oi_t *sensor_data, s_pos *self){
    turn_counter_clockwise(sensor_data, 5, 100, self);
    uart_sendChar(0x0C);
}

void run_turn_right(oi_t *sensor_data, s_pos *self){
    turn_clockwise(sensor_data, 5, 100, self);
    uart_sendChar(0x0D);
}

void run_get_status(s_pos *self, s_object_t *stored_objects, uint8_t stored_count){
    uart_sendChar(0x0E);
    uart_sendChar(stored_count);
    tx_float_t f;
    f.f = self->pos_x;
    send_float(f);
    f.f = self->pos_y;
    send_float(f);
    f.f = self->heading;
    send_float(f);
    tx_short_t s;
    for(int i = 0; i < stored_count; i++){
        s.s = stored_objects[i].pos_x;
        send_short(s);
        s.s = stored_objects[i].pos_y;
        send_short(s);
        s.s = stored_objects[i].width;
        send_short(s);
    }
}

void run_basic_scan(scan_point_t *raw_data){
    do_ir_scan(raw_data);
    uart_sendChar('\n');
}

void run_dist_ahead(){
    scan_init(0x03);
    uint8_t i = 0;
    do{
        cyBOT_Scan(90, &scan);
        i++;
    }while(i < 10 && scan.sound_dist > 101.0);
    sprintf(msg, "%f\r", scan.sound_dist);
    uart_sendStr(msg);
    uart_sendChar('\n');
}

void run_end(char *end, oi_t *sensor_data){
    oi_free(sensor_data);
    *end = 1;
}

void send_float(tx_float_t data){
    uart_sendChar(data.c[0]);
    uart_sendChar(data.c[1]);
    uart_sendChar(data.c[2]);
    uart_sendChar(data.c[3]);
}

void send_short(tx_short_t data){
    uart_sendChar(data.c[0]);
    uart_sendChar(data.c[1]);
}

