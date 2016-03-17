/*
 * lcd_display.cpp
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: jeerawatIT
 */
#include <application.h>
#include <stdio.h>

LiquidCrystal_I2C *lcd;
bool lcdFound = false;
uint8_t lcdAddr;
bool runing = false;

uint8_t icon_clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};

uint8_t celsius[8] =
{
	B00111,
	B00101,
	B00111,
	B00000,
	B00000,
	B00000,
	B00000
};

byte icon_termometer[8] = //icon for termometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte icon_water[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};


void lcdConnectionFailed()
{
	if (lcdFound){
		lcd->clear();
		lcd->setCursor(0,0);
		lcd->print("Access point");
		lcd->setCursor(0,1);
		lcd->print("Connection lost");
	}
}

void lcdSystemReset()
{
	if (lcdFound){
		lcd->clear();
		lcd->setCursor(0,0);
		lcd->print("System reset");
		lcd->setCursor(0,1);
		lcd->print("Restart device!");
	}
}

char buf[64];

void lcdPrint(float t, float h )
{
	//DateTime _dateTime = SystemClock.now();
	if (lcdFound){
		if (!runing){
			lcd->clear();
			runing = true;
		}
		//char buf[64];
		lcd->setCursor(0,0);
		lcd->print("   HyDroKMBox   ");
		//sprintf(buf, "%02d/%02d/%d %02d:%02d", _dateTime.Day, _dateTime.Month + 1, _dateTime.Year + 543, _dateTime.Hour, _dateTime.Minute);
		//DateTime _dateTime = SystemClock.now();
		//sprintf(buf, "%02d/%02d/%d %02d:%02d", _dateTime.Day, _dateTime.Month + 1, _dateTime.Year + 543,  _dateTime.Hour, _dateTime.Minute);
		//lcd->print(String(buf));

		lcd->setCursor(0, 1);
		if (!isnan(t) || !isnan(h)){
			lcd->print("\1" + String(t) + (char)233 + "C " + "\2" + String(h) + "%");
		}else{
			lcd->print("----- NaN -----");
		}
		//lcd->setCursor(0,0);
		//lcd->print(" \1 " + String(t) +  (char)223 + "C ");
		//lcd->setCursor(0,1);
		//lcd->print(" \2 " + String(h) + "%");
	}
	if (!isnan(t) || !isnan(h)){
			String message = "{ \"Type\": 7, \"Id\": \""+deviceId+"\", \"Temperature\": \"" + String(t) + "\", \"Humidity\": \"" + String(h) + "\" }";
			Serial.println(message);
			sendSocketClient(message);
			if (BrokerSettings.active == true) {
				if (mqtt->getConnectionState() == TcpClientState::eTCS_Connected){
					mqtt->publish( deviceId + "/" +BrokerSettings.user_name+ "/temperature", message);
				}
			}
		}
}

void readDhtFailed()
{
	if (lcdFound){
		lcd->clear();
		lcd->setCursor(0,0);
		lcd->print("Error");
		lcd->setCursor(0,1);
		lcd->print("Failed read DHT");
	}
}

void lcdApAddress()
{
	if (lcdFound){
		lcd->clear();
		lcd->setCursor(0,0);
		lcd->print("AP Address:");
		lcd->setCursor(0,1);
		lcd->print(WifiAccessPoint.getIP().toString());
	}
}

void lcdStationAddress()
{
	if (lcdFound){
		lcd->clear();
		lcd->setCursor(0,0);
		lcd->print("IP Address:");
		lcd->setCursor(0,1);
		lcd->print(WifiStation.getIP().toString());
	}
}


void scanBus(){

	byte error, address;
	//uint8_t address[] = { 0x27, 0x3F };
	lcdFound = false;
	Wire.begin();
	for (address = 1; address < 127; address++){
		WDT.alive();
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0){
			lcdAddr = address;
			lcdFound = true;
			break;
		}
	}
}


void lcdSystemStarting()
{
	Wire.pins(2, 0);
	scanBus();
	if (lcdFound){
		lcd = new LiquidCrystal_I2C(lcdAddr, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
		lcd->begin(16, 2);
		lcd->backlight();

		lcd->createChar(1, icon_termometer);
		lcd->createChar(2, icon_water);
		lcd->createChar(3, celsius);

		lcd->setCursor(0,0);
		lcd->print("System starting");
		lcd->setCursor(0,1);
		lcd->print("Please wait...");
	}
}

