#include "Arduino.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "ADXL345Extended.h"
#include "Target.h"

void isrJab(void);
void isrCross(void);

#define INDEX_JAB          0
#define INDEX_CROSS        1
#define INDEX_LEFT_HOOK    2
#define INDEX_RIGHT_HOOK   3
#define INDEX_LEFT_UPCT    4
#define INDEX_RIGHT_UPCT   5
#define NB_TARGETS 2

target targets[] = {
		{ "jab", 0, 0, 2, isrJab, nullptr,  // name, stats, shift reg, int_pin, isr, accel obj
				{
					ADXL345_RANGE_16G, // range
					ADXL345_RATE_200,  // rate
					0x1,               // wake_up_freq
					0xFF,              // tap_thr
					0x60,              // tap_duration
					false,             // tap_x_axis
					true,              // tap_y_axis
					false,             // tap_z_axis
				}
		},
		{ "cross", 0, 1, 3, isrCross, nullptr, // name, stats, shift reg, int_pin, isr, accel obj
				{
					ADXL345_RANGE_16G, // range
					ADXL345_RATE_200,  // rate
					0x1,               // wake_up_freq
					0xFF,              // tap_thr
					0x60,              // tap_duration
					false,             // tap_x_axis
					true,              // tap_y_axis
					false,             // tap_z_axis
				}
		}
};



int dataPin  = 4;   // green
int latchPin = 5;  // blue
int clockPin = 6;  // yellow

uint8_t hitFlags = 0;

void isrJab() {
	hitFlags |= 1 << INDEX_JAB;
}

void isrCross() {
	hitFlags |= 1 << INDEX_CROSS;
}

void talkToTarget(uint8_t targetShiftRegPin) {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, ~(1 << targetShiftRegPin));
	digitalWrite(latchPin, HIGH);
}

void setup() {
    //set pins to output so you can control the shift register
	Serial.print("[SETUP] Setup Arduino pins driving shift register");
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

	// join I2C bus (I2Cdev library doesn't do this automatically)
	Wire.begin();

	// initialize serial communication
	Serial.begin(38400);
	for (int i=0; i < NB_TARGETS; i++) {
		Serial.print("[SETUP] Setup target ");
		Serial.println(targets[i].name);

		Serial.println("[SETUP] Pull up all shift register outputs but the target's one ");
		talkToTarget(targets[i].shift_register_output_pin);

		Serial.println("[SETUP] setup Arduino pin for target interrupt");
		pinMode(targets[i].external_interrupt_pin, INPUT);
		Serial.println("[SETUP] attach ISR");
		attachInterrupt(
				digitalPinToInterrupt(targets[i].external_interrupt_pin),
				targets[i].isr, RISING);
		Serial.println("[SETUP] create accelerometer objet");
		targets[i].accel = new ADXL345_Extended();
		Serial.println("[SETUP] Initialize I2C device");
		targets[i].accel->initialize();
		Serial.println("[SETUP] Probe device");
		if (targets[i].accel->testConnection() == 0) {
			Serial.println("[SETUP] Failed to test device...");
			return;
		}
		Serial.println("[SETUP] Calibrate accelerometer");
		targets[i].accel->calibrate();
		Serial.println("[SETUP] Setup accelerometer");
		targets[i].accel->setup(targets[i].accel_conf);
		Serial.println("[SETUP] Start accelerometer");
		targets[i].accel->start();
	}
}


void checkHits() {
	for (int i = 0; i < NB_TARGETS; i++) {
		if (((hitFlags >> i) & 1)) {
			targets[i].stats++;

			Serial.print(targets[i].stats);
			Serial.print(" - ");
			Serial.print(targets[i].name);
			Serial.println(" detected");
			hitFlags &= ~(1 << i);

			delay(150); // ori=200
			talkToTarget(targets[i].shift_register_output_pin);
			targets[i].accel->getIntSingleTapSource();
		}
	}
}

// The loop function is called in an endless loop
void loop() {
	checkHits();
}
