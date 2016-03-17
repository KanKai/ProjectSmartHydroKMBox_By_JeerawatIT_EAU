/*
 * mqtt_server.cpp
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: admin
 */
#include <application.h>



MqttClient *mqtt;
Timer procTimer;
Timer reconTimer;
//String inTopic  = "devices/55b0a8e0cc2c1054ff0022ab/get";
//String outTopic = "devices/55b0a8e0cc2c1054ff0022ab/set";
String inTopic  = "";
String outTopic = "";
// Publish our message
bool brokerActive = false;
bool connectionState = false;

void client_connect();

void publishMessage(float t, float h)
{

	if (isnan(t) || isnan(h))
	{
		Serial.println("Failed read DHT");
		//readDhtFailed();
	} else {
		String message = "{ \"Type\": 1, \"Id\": \""+deviceId+"\", \"Temperature\": \"" + String(t) + "\", \"Humidity\": \"" + String(h) + "\" }";
		//Serial.println(message);
		sendSocketClient(message);

		if (connectionState){
			if (mqtt->getConnectionState() == TcpClientState::eTCS_Connected){
				mqtt->publish(deviceId + "/"+ BrokerSettings.user_name +"/temperature", message);
				mqtt->publish(deviceId + "/"+ BrokerSettings.user_name +"/humidity", message);
				Serial.println("Let's publish message now!");
			}else{

				Serial.println("MQTT client disconnected!");
				connectionState = false;
				reconTimer.initializeMs(30000, client_connect).startOnce();
				Serial.println("MQTT client reconnection!");
			}
		}
	}
}

// Callback for messages, arrived from MQTT server
void onMessageReceived(String topic, String message)
{
	Serial.println();
	Serial.println(topic);
	Serial.println(" : "); // Prettify alignment for printing
	Serial.println(message+"\r\n********************************\r\n");

	DynamicJsonBuffer jsonBuffer;

	BrokerSettings.load();
	int state = LOW;
	uint16_t gpio = -1;
	char* jsonString = new char[message.length()+1];

	if (topic == outTopic){
		message.toCharArray(jsonString, message.length()+1);
		JsonObject& root = jsonBuffer.parseObject(jsonString);
		//Serial.println(root.toJsonString());
		String id = root["properties"][0]["id"].toString();
		String value = root["properties"][0]["value"].toString();

		Serial.println("In Topic");
		Serial.println("id : " + id);
		Serial.println("value : " + value);
		Serial.println();
		String token;
		if (id == BrokerSettings.gpio12Id){
			state = value == "on" ? HIGH : LOW;
			token = BrokerSettings.gpio12Id;
			gpio = CHANNEL1;
		}
		if (id == BrokerSettings.gpio13Id){
			state = value == "on" ? HIGH : LOW;
			token = BrokerSettings.gpio13Id;
			gpio = CHANNEL2;
		}
		if (id == BrokerSettings.gpio14Id){
			state = value == "on" ? HIGH : LOW;
			token = BrokerSettings.gpio14Id;
			gpio = CHANNEL3;
		}
		if (id == BrokerSettings.gpio4Id){
			state = value == "on" ? HIGH : LOW;
			token = BrokerSettings.gpio4Id;
			gpio = CHANNEL4;
		}
		if (gpio != -1){
			digitalWrite(gpio, state);
			// websocket broadcast io status
			String st = digitalRead(gpio)?"1":"0";
			sendSocketClient("{ \"Type\": 2, \"gpio\": " + String(gpio) + ", \"status\": " + st  + " }");

			st = digitalRead(gpio)?"on":"off";
			String pubMsg = "{\"properties\":[{ \"token\": \"" + token + "\", \"status\": \"" + st + "\" }]}";
			mqtt->publish("device/"+BrokerSettings.user_name+"/"+BrokerSettings.token+"/status", pubMsg, true);
		}
		delete jsonString;
	}

	if (topic == inTopic){
		Serial.println("Out Topic");

		String status12 = digitalRead(CHANNEL1)?"on":"off";
		String status13 = digitalRead(CHANNEL2)?"on":"off";
		String status14 = digitalRead(CHANNEL3)?"on":"off";
		String status4  = digitalRead(CHANNEL4)?"on":"off";
		String pubMsg = "{\"properties\":[{ \"status\": \"" + status12  + "\", \"token\": \"" + BrokerSettings.gpio12Id + "\"" + " },{ \"status\": \"" + status13  + "\", \"token\": \"" + BrokerSettings.gpio13Id + "\"" + " },{ \"status\": \"" + status14 + "\", \"token\": \"" + BrokerSettings.gpio14Id + "\"" + " },{ \"status\": \"" + status4 + "\", \"token\": \"" + BrokerSettings.gpio4Id + "\"" + " }]}";
		//String pubMsg = "{\"properties\":[{ \"type\": 2, \"status\": \"" + status12  + "\", \"gpio\": 12 },{ \"type\": 2, \"status\": \"" + status13  + "\", \"gpio\": 13 },{ \"type\": 2, \"status\": \"" + status14 + "\", \"gpio\": 14 }]}";
		String pubTopic = "device/"+BrokerSettings.user_name+"/"+BrokerSettings.token+"/status";
		mqtt->publish(pubTopic, pubMsg, true);
		//DEBUG_PRINT(pubTopic + ": " + pubMsg);

	}
}

void client_connect()
{
	BrokerSettings.load();
	// Run MQTT client
	mqtt->connect(deviceId, BrokerSettings.user_name, BrokerSettings.password);
	mqtt->subscribe(inTopic);
	mqtt->subscribe(outTopic);
	connectionState = true;
	//publishMessage();
	Serial.println("MQTT client connected");
}


void startMQTT()
{
	BrokerSettings.load();
	brokerActive = BrokerSettings.active;
	Serial.println("Start MQTT Server");
	Serial.println("Host: "+ BrokerSettings.serverHost);
	Serial.println("Port: "+ String(BrokerSettings.serverPort));
	Serial.println("Interval: "+ String(BrokerSettings.updateInterval));
	Serial.println("Pin: "+ String(BrokerSettings.pinNumber));

	if (brokerActive){
		mqtt = new MqttClient(BrokerSettings.serverHost, BrokerSettings.serverPort, onMessageReceived);
		client_connect();
	}
	// Start publishing loop
	//procTimer.initializeMs(BrokerSettings.updateInterval * 1000, publishMessage).start(); // every 5 seconds
}



