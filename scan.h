
#ifndef SCAN_H_
#define SCAN_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inc/tm4c123gh6pm.h>
#include "Timer.h"
#include "open_interface.h"
#include "lcd.h"
#include "servo.h"
#include "movement.h"
#include "util.h"
#include "temp.h"

float p1, p2, p3;

typedef struct
{
    uint8_t object_num;
    uint8_t start_angle;
    uint8_t end_angle;
    uint8_t center_angle;
    uint8_t width;
    float linear_width;
    float dist;
}object_t;

typedef struct
{
    short pos_x;
    short pos_y;
    short width;
}s_object_t;

typedef struct
{
    float temp;
    float dist;
}scan_point_t;

void print_to_csv(float* scan_points);

void do_scan(float *scan_points);

void send_scan(scan_point_t *scan_points, float *raw_data);

void filter_outlier(float* outliers_filtered, float* distances, float threshold);

int find_objects(object_t* object_list, scan_point_t* distances, float threshold, oi_t* sensor);

void print_objects(object_t* object_list, int object_count);

void go_to_smallest(object_t* objects, uint8_t* count, oi_t* sensor, s_pos *self, s_object_t* stored_objects);

int do_scan_filter(scan_point_t* scan_points, object_t* object_list, float* raw_data, oi_t* sensor, s_object_t* stored_objects, uint8_t* stored_count, s_pos *self);

void do_ir_scan(scan_point_t* scan_points);

void scan_init(char init_val);

uint8_t store_objects(object_t* objects, s_pos *self, s_object_t* stored_objects, uint8_t object_count, uint8_t stored_count);

void print_stored(s_object_t* stored_objects, uint8_t stored_count, s_pos *self);

uint8_t compare_objects(s_object_t a, s_object_t b);

void refine_edges(object_t *object, oi_t *sensor, float threshold, int object_count);

float get_dist_from_IR(uint16_t raw_IR);

//compensates for distance between servo and center
//Distances are changed to mm
void compensate_for_displacement(object_t* object_list, uint8_t object_count);

float get_avg_ir(oi_t* sensor, uint8_t angle);

uint8_t is_path_valid(s_object_t* stored, path_t p, uint8_t stored_count);


#endif
