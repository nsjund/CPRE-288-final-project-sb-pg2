
#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "open_interface.h"
#include "lcd.h"
#include "util.h"

typedef struct{
    float pos_x;
    float pos_y;
    float heading;
}s_pos;

typedef struct{
    short x1;
    short y1;
    short x2;
    short y2;
}path_t;

void turn_clockwise(oi_t *sensor, int degrees, int turn_speed, s_pos *self);

void turn_counter_clockwise(oi_t *sensor, int degrees, int turn_speed, s_pos *self);

uint8_t move_forward(oi_t *sensor, int mm, int max_speed, s_pos *self);

//void move_forward_colission_avoid(oi_t *sensor, int mm, int move_speed, s_pos *self);

int move_backwards(oi_t *sensor, int mm, int move_speed, s_pos *self);

void dummy_move(oi_t *sensor, int mm, int move_speed);

uint8_t go_to_point(oi_t *sensor, short x, short y, s_pos *self);

void face_angle(oi_t *sensor, float target, s_pos *self);

int get_speed(int target_disp, int current_disp, int max_speed, int acceleration);

void update_self(oi_t *sensor, s_pos *self);

//void follow_path(oi_t* sensor, path_point_t* path, s_pos *self, uint8_t steps, short end_angle);

#endif
