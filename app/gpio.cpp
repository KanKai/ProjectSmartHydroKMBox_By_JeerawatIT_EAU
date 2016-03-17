/*
 * gpio.cpp
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: admin
 */

#include <application.h>

bool state = true;

void blink()
{
	digitalWrite(CONF_PIN, state);
	state = !state;
}

void init_gpio()
{
	pinMode(4, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
	pinMode(14, OUTPUT);
	digitalWrite(4, LOW);
	digitalWrite(12, LOW);
	digitalWrite(13, LOW);
	digitalWrite(14, LOW);
}


