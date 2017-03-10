/*
 * ADXL345Extended.cpp
 *
 *  Created on: 21 févr. 2017
 *      Author: jgarcia
 */

#include "ADXL345Extended.h"

ADXL345_Extended::ADXL345_Extended() {
	// TODO Auto-generated constructor stub

}

ADXL345_Extended::~ADXL345_Extended() {
	// TODO Auto-generated destructor stub
}

/** Power on chip and choose whether we want to start measurements or not
 */
void ADXL345_Extended::initialize() {
    I2Cdev::writeByte(ADXL345_DEFAULT_ADDRESS, ADXL345_RA_POWER_CTL, 0); // reset all power settings
   // setAutoSleepEnabled(false);
   // setMeasureEnabled(start_measurement);
}

/** Calibrate chip
 * This is done as per document http://www.analog.com/media/en/technical-documentation/application-notes/AN-1077.pdf
 * using the "USING OFFSET REGISTERS" flavor:
 * Sequence description(for 100Hz output data rate):
 *   1- Place sensor in X = 0g, Y = 0g, Z = +1g
 *   2- VS = ON  and VDD I/O = ON
 *   3- Wait for 1.1ms
 *   4- Initialize command sequence
 *   4.1 - switch to +/-16g range
 *   4.2 - start measurement
 *   4.3 - enable DATA_READY interrupt
 *   5 - Wait for 11.1ms
 *   6- Take 100 data points and average
 *   7- Compute calibration value
 *   8- Write to offset registers
 */
 void ADXL345_Extended::calibrate() {
	int16_t ax_calib, ay_calib, az_calib;

	/* reset previously computed offsets */
	setOffset(0,0,0);

	/* configure chip and start measurement */
	setRate(ADXL345_RATE_100);
	delay(2);
	setRange(ADXL345_RANGE_16G);
	setMeasureEnabled(true);
	setIntDataReadyEnabled(true);
	delay(12);

	/* compute 100 measurements average */
	getAcceleration(&ax_calib, &ay_calib, &az_calib);
	for (int16_t i=0; i < 100; i++) {
		int16_t ax, ay, az;
		getAcceleration(&ax, &ay, &az);
		ax_calib = (ax_calib + ax) / 2;
		ay_calib = (ay_calib + ay) / 2;
		az_calib = (az_calib + az) / 2;
	}

	/* extrapolate offset values and apply them */
	ax_calib = -(ax_calib / 4);
	ay_calib = -(ay_calib / 4);
	az_calib = -((az_calib - 256) / 4);
	setOffset((int8_t)ax_calib, (int8_t)ay_calib, (int8_t)az_calib);

	/* stop measurements and reset to default values */
	setMeasureEnabled(false);
	setIntDataReadyEnabled(false);
}

#define ADXL345_INT_PIN 0

 void ADXL345_Extended::setup(struct adxl345_conf conf) {
	/* The SINGLE_TAP bit is set when a single acceleration event
       that is greater than the value in the THRESH_TAP register
       (Address 0x1D) occurs for less time than is specified in
       the DUR register (Address 0x21). */
	setRange(conf.range);
	setFullResolution(1);
	setRate(conf.rate);
	setWakeupFrequency(conf.wake_up_freq);
	setTapThreshold(conf.tap_threshold);
	setTapDuration(conf.tap_duration);
	setTapAxisXEnabled(conf.tap_axis_x_enabled);
	setTapAxisYEnabled(conf.tap_axis_y_enabled);
	setTapAxisZEnabled(conf.tap_axis_z_enabled);
	setIntSingleTapPin(ADXL345_INT_PIN);
	setIntSingleTapEnabled(true);
}

void ADXL345_Extended::start() {
	setMeasureEnabled(true);
}

