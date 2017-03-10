/*
 * ADXL345Extended.h
 *
 *  Created on: 21 févr. 2017
 *      Author: jgarcia
 */

#ifndef ADXL345EXTENDED_H_
#define ADXL345EXTENDED_H_

#include "ADXL345.h"


typedef struct adxl345_conf {
	uint8_t range;
	uint8_t rate;
	uint8_t wake_up_freq;
	uint8_t tap_threshold;
	uint8_t tap_duration;
	bool tap_axis_x_enabled;
	bool tap_axis_y_enabled;
	bool tap_axis_z_enabled;
};

class ADXL345_Extended : public ADXL345 {
public:
	ADXL345_Extended();
	ADXL345_Extended(struct adxl345_conf);
	virtual ~ADXL345_Extended();

	void initialize();
	void calibrate();
	void setup(struct adxl345_conf);
	void start();


private:
	struct adxl345_conf conf;
};

#endif /* ADXL345EXTENDED_H_ */
