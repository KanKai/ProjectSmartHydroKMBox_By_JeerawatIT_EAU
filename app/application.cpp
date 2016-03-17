
#include <application.h>
#include <mqtt_server.cpp>

//#ifndef WIFI_SSID

//	#define WIFI_SSID "IoT_Hydro" // Put you SSID and Password here
//	#define WIFI_PWD "IoT.123456789"
//#endif



String deviceId;
BssList networks;
bool readyToRun;
Timer statusTimer;

//#endif
unsigned long pressed;
bool configStarted = false;

void startServers()
{
	Serial.println("System Ready...");
	startFTP();
	startWebServer();

	// Temp & Humi Monitoring
	startMonitor();
}

void connectOk()
{

	startServers();
	procTimer.initializeMs(5000, startMQTT).startOnce();
	if (lcdFound){
		lcdStationAddress();
	}else{
		statusTimer.stop();
		statusTimer.initializeMs(1000, blink).start();
	}
	readyToRun = true;
	statusTimer.initializeMs(5000, startServers).startOnce();


}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	Serial.println("I'm NOT CONNECTED. Need help :(");
	procTimer.stop();
	tempTimer.stop();
	statusTimer.stop();
	if (lcdFound){
		lcdConnectionFailed();
	}else{
		statusTimer.initializeMs(100, blink).start();
	}
}

void SystemReady()
{

	if (lcdFound){
		if (WifiAccessPoint.isEnabled()){
			lcdApAddress();
			statusTimer.initializeMs(5000, startServers).startOnce();
		}
	}else{
		statusTimer.initializeMs(100, blink).start();
	}


}

void disableWifi()
{
	WifiStation.enable(false);
}

void networkScanCompleted(bool succeeded, BssList list)
{
	if (succeeded)
	{
		for (int i = 0; i < list.count(); i++)
			if (!list[i].hidden && list[i].ssid.length() > 0)
				networks.add(list[i]);
	}
	Serial.println("Network Scan Completed");
	networks.sort([](const BssInfo& a, const BssInfo& b){ return b.rssi - a.rssi; } );
	if (!NetworkSettings.exist()){
		procTimer.initializeMs(2000, disableWifi).startOnce();
	}
}

void enablerAccessPoint()
{
	DeviceSettings.load();
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config(DeviceSettings.ssid + "-" + String(getChipId()), DeviceSettings.password, AUTH_OPEN);
}

void IRAM_ATTR interruptHandler()
{
	if (digitalRead(INT_PIN) == LOW){
		pressed = millis();
	}
	if (digitalRead(INT_PIN) == HIGH){
		if ((millis() - pressed) > 5000){
			pressed = millis();
			NetworkSettings.Delete();
			tempTimer.stop();

			if (lcdFound){
				lcdSystemReset();
			}else{
				digitalWrite(CONF_PIN, LOW);
			}
		}
	}
}


void init()
{
	spiffs_mount(); 
	lcdSystemStarting();

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Debug output to serial
	attachInterrupt(INT_PIN, interruptHandler, CHANGE);
	deviceId = String(getChipId());
	readyToRun = false;
	init_gpio();
	SystemClock.setTimeZone(7);

	BrokerSettings.load();
	DeviceSettings.load();
	inTopic = "devices/" + BrokerSettings.user_name + "/" + BrokerSettings.token+"/get";
	outTopic = "devices/"+ BrokerSettings.user_name + "/" + BrokerSettings.token+"/set";

	dht = new DHT(5, DHT22, 30);
	dht->begin();
	WifiStation.enable(true);
#ifndef WIFI_SSID

	if (NetworkSettings.exist())
	{
		NetworkSettings.load();
		debugf("Load file config...");
		WifiStation.config(NetworkSettings.ssid, NetworkSettings.password);
		if (!NetworkSettings.dhcp && !NetworkSettings.ip.isNull())
			WifiStation.setIP(NetworkSettings.ip, NetworkSettings.netmask, NetworkSettings.gateway);
		WifiStation.waitConnection(connectOk, 20, connectFail); // We recommend 20+ seconds for connection timeout at start

		WifiAccessPoint.enable(false);
	}else{
		enablerAccessPoint();
	}
#else
	WifiAccessPoint.enable(false);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.waitConnection(connectOk, 20, connectFail);
	//enablerAccessPoint();
#endif
	WifiStation.startScan(networkScanCompleted);
	// Start AP for configuration
	//WifiAccessPoint.enable(true);
	//WifiAccessPoint.config(DeviceSettings.ssid+"-"+deviceId, DeviceSettings.password, AUTH_OPEN);
	// Run WEB server on system ready
	System.onReady(SystemReady);
}
