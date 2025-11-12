/*
 * commands.h
 *
 *  Created on: Nov 2, 2025
 *      Author: nsjund
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <scan.h>
#include "uart.h"
#include "open_interface.h"
#include "music.h"
#include "scan.h"
#include "movement.h"
#include "lcd.h"
#include "servo.h"

typedef union{
    char c[4];
    float f;
    int i;
}tx_float_t;

typedef union{
    char c[2];
    short s;
}tx_short_t;

void run_auto_mode(float *scan_points, object_t *object_list, float *raw_data, oi_t *sensor_data, s_object_t *stored_objects, uint8_t *stored_count, s_pos *self);

void run_object_scan(float *scan_points, object_t *object_list, float *raw_data, oi_t *sensor_data, s_object_t *stored_objects, uint8_t *stored_count, s_pos *self);

void run_calibrate_servo();

void run_play_music(oi_t *sensor_data);

void run_test_1(s_pos *self);

void run_test_2(oi_t *sensor_data, s_pos *self);

void run_calibrate_ir();

void run_move_forward(oi_t *sensor_data, s_pos *self);

void run_turn_left(oi_t *sensor_data, s_pos *self);

void run_turn_right(oi_t *sensor_data, s_pos *self);

void run_get_status(s_pos *self, s_object_t *stored_objects, uint8_t stored_count);

void run_basic_scan(float *raw_data);

void run_dist_ahead();

void run_end(char *end, oi_t *sensor_data);

//works for ints too
void send_float(tx_float_t data);

void send_short(tx_short_t data);

#endif /* COMMANDS_H_ */
