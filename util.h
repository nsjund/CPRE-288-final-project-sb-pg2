/*
 * util.h
 *
 *  Created on: Oct 17, 2025
 *      Author: nsjund
 */

#ifndef UTIL_H_
#define UTIL_H_

#define MIN(a, b) ((a) <= (b) ? (a) : (b))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

float degToRad(short deg);

short radToDeg(float rad);

float fdegToRad(float deg);

#endif /* UTIL_H_ */
