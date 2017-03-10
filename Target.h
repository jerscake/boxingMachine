/*
 * Target.h
 *
 *  Created on: 9 mars 2017
 *      Author: jgarcia
 */

#ifndef TARGET_H_
#define TARGET_H_
#include "ADXL345Extended.h"

#define TARGET_NAME_LEN 20

typedef struct Test {
	int i;
	int y;
};

typedef struct target {
	char name[TARGET_NAME_LEN];
	uint32_t stats;
	uint8_t shift_register_output_pin;
	uint8_t external_interrupt_pin;
	void (*isr)();
	ADXL345_Extended * accel;
	adxl345_conf accel_conf;
};



#endif /* TARGET_H_ */
