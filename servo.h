/*
 * servo.h
 *
 *  Created on: Nov 4, 2025
 *      Author: nsjund
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include "lcd.h"
#include "Timer.h"
#include "button.h"

typedef struct{
    float sound_dist;  // Distance from Ping Sensor (cyBOT_Scan returns -1.0 if PING is not enabled)
    int IR_raw_val;    // Raw ADC value from IR sensor (cyBOT_Scan return -1 if IR is not enabled)
} cyBOT_Scan_t;

void cyBOT_init_Scan(int feature);

void cyBOT_Scan(int angle, cyBOT_Scan_t* getScan);

#define right_calibration_value 16005

#define left_calibration_value 44536

#define cal_offset_value -48

#define pulse_period 320000

volatile int pulse_width;

void servo_init(void);

void servo_move(int degrees);

void cyBOT_SERVO_cal();

void print_calibration(int sign, int degrees);

void adc_init(void);

uint16_t adc_read(void);

int get_overflows();

void TIMER3B_Handler(void);

void send_pulse(void);

float ping_read(void);

void ping_init();

#endif /* SERVO_H_ */
