/*
 * temp.h
 *
 *  Created on: Nov 7, 2025
 *      Author: nsjund
 */

#ifndef TEMP_H_
#define TEMP_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>

#define IR_TEMP_ADDRESS 0x5A

void i2c_write_byte(uint8_t data);

void temp_sensor_init();

uint16_t temp_read();

#endif /* TEMP_H_ */
