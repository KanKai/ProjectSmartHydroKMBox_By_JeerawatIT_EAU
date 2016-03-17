/*
 * temp_control.cpp
 *
 *  Created on: Aug 1, 2558 BE
 *      Author: admin
 */

#include <application.h>



Timer tempTimer;
Timer checkTimer;

int tempMin;
int tempMax;
int humiMin;
int humiMax;
int interval;
bool mode;

void sendSocket(int gpio)
{
	// websocket broadcast io status
	String st = digitalRead(gpio)?"1":"0";
	String msg = "{ \"Type\": 2, \"gpio\": " + String(gpio) + ", \"status\": " + st  + " }";
	sendSocketClient(msg);
}

bool humiNormal(float h)
{
	return (h <= humiMax);
}

bool tempNormal(float t)
{
	return ((t >= tempMin) && (t <= tempMax));
}

void fanOn()
{
	if (digitalRead(4) == LOW){
		digitalWrite(4, HIGH);
		sendSocket(4);
	}
}

void fanOff()
{
	if (digitalRead(4) == HIGH){
		digitalWrite(4, LOW);
		sendSocket(4);
	}
}

void heaterOn()
{
	if (digitalRead(13) == LOW){
		digitalWrite(13, HIGH);
		sendSocket(13);
	}
	if (digitalRead(14) == LOW){
		digitalWrite(14, HIGH);
		sendSocket(14);
	}
	if (digitalRead(12) == LOW){
		digitalWrite(12, HIGH);
		sendSocket(12);
	}
}

void heaterOff()
{
	if (digitalRead(13) == HIGH){
		digitalWrite(13, LOW);
		sendSocket(13);
	}
	if (digitalRead(14) == HIGH){
		digitalWrite(14, LOW);
		sendSocket(14);
	}
	if (digitalRead(12) == HIGH){
		digitalWrite(12, LOW);
		sendSocket(12);
	}
}

void stopTemp()
{
	heaterOff();
	fanOff();

	tempTimer.initializeMs(interval * 1000, startMonitor).start();

}

void processTemp(float t, float h)
{
	// GPIO4  = Fan
	// GPIO13 = Heater
	if (tempNormal(t) & humiNormal(h) ){
		return;
	}

	if (t > tempMax){
		heaterOn();
		fanOff();
		tempTimer.initializeMs(40 * 1000, stopTemp).startOnce();


	}else{
		tempTimer.initializeMs(1000, stopTemp).stop();
	}
	if (t < tempMin){
		heaterOff();
		fanOn();

	}
	if (h > humiMax){
		fanOn();
	}else{
		fanOff();
	}

}

void runInterval()
{
	if ((tempMin == 0) | (tempMax == 0) | (humiMax == 0) ){
		Serial.println("Temperature config not found!");
		return;
	}
	float h = dht->readHumidity();
	float t = dht->readTemperature();
	if (isnan(t) || isnan(h))
	{
		Serial.println("Failed read DHT");
		readDhtFailed();

	}else{
		lcdPrint(t, h);
		//if (readyToRun){
		publishMessage(t, h);
		//}
		//if (mode){
		processTemp(t, h);
		//}
	}
}

void loadMonitorConfig()
{
	TempSettings.load();
	tempMin = TempSettings.tempMin;
	tempMax = TempSettings.tempMax;
	//humiMin = TempSettings.humiMin;
	humiMax = TempSettings.humiMax;
	interval = TempSettings.interval;
	mode = TempSettings.mode;
	tempTimer.stop();
	tempTimer.initializeMs(interval * 1000, runInterval).start();


}

void startMonitor()
{
	loadMonitorConfig();

	Serial.println("Starting temperature monitoring: " + String(TempSettings.interval));
	tempTimer.initializeMs(interval * 1000, runInterval).start();
}
