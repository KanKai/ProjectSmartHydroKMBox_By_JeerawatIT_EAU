/*
 * application.h
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: admin
 */

#ifndef INCLUDE_APPLICATION_H_
#define INCLUDE_APPLICATION_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/DHT/DHT.h>
#include <Libraries/LiquidCrystal/LiquidCrystal_I2C.h>
#include <NetworkConfig.h>
#include <BrokerConfig.h>
#include <DeviceConfig.h>
#include <TempConfig.h>

#define CHANNEL1 12
#define CHANNEL2 13
#define CHANNEL3 14
#define CHANNEL4 4

 extern String inTopic;
 extern String outTopic;

// SDA GPIO2, SCL GPIO0
//#define I2C_LCD_ADDR 0x27
#define I2C_LCD_ADDR 0x3F
extern LiquidCrystal_I2C *lcd;

extern DHT *dht;
extern Timer procTimer;
extern Timer tempTimer;
extern Timer statusTimer;

// MQTT client
// For quickly check you can use: http://www.hivemq.com/demos/websocket-client/ (Connection= test.mosquitto.org:8080)
extern MqttClient *mqtt;
extern HttpServer server;
extern FTPServer ftp;
extern BssList networks;
extern bool readyToRun;
extern String deviceId;
extern bool lcdFound;



uint32_t getChipId(void);

void restart();

void sendSocketClient(String msg);

void lcdPrint(float t, float h );

void lcdStationAddress();

void lcdApAddress();

void lcdSystemStarting();

void lcdConnectionFailed();

void lcdSystemReset();

void readDhtFailed();

void publishMessage(float t, float h);

void onMessageReceived(String topic, String message);

void startMQTT();

void startFTP();

void startWebServer();

void startServers();

void init_gpio();

void blink();

void loadMonitorConfig();

void startMonitor();


#endif /* INCLUDE_APPLICATION_H_ */
