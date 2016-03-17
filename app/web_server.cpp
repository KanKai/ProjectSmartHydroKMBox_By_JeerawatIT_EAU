/*
 * web_server.cpp
 *
 *  Created on: Jul 29, 2558 BE
 *      Author: admin
 */

#include <application.h>

HttpServer server;
DHT *dht;
bool isLogin = false;

void sendSocketClient(String msg)
{
	WebSocketsList &clients = server.getActiveWebSockets();
	for (int i = 0; i < clients.count(); i++)
		clients[i].sendString(msg);
}

void onIndex(HttpRequest &request, HttpResponse &response)
{
	if (isLogin){
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();

	vars["gpio4"] = digitalRead(4) == HIGH
			? "<image id=\"gpio4\" src=\"light-on.png\">":"<image id=\"gpio4\" src=\"light-off.png\">";
	vars["gpio13"] = digitalRead(13) == HIGH
			? "<image id=\"gpio5\" src=\"light-on.png\">":"<image id=\"gpio13\" src=\"light-off.png\">";
	vars["gpio14"] = digitalRead(14) == HIGH
				? "<image id=\"gpio6\" src=\"light-on.png\">":"<image id=\"gpio14\" src=\"light-off.png\">";
	vars["gpio12"] = digitalRead(12) == HIGH
				? "<image id=\"gpio7\" src=\"light-on.png\">":"<image id=\"gpio12\" src=\"light-off.png\">";

	vars["serial"] = deviceId;

	TempSettings.load();
	vars["temp_min"] = TempSettings.tempMin;
	vars["temp_max"] = TempSettings.tempMax;
	vars["humi_max"] = TempSettings.humiMax;

	response.sendTemplate(tmpl); // will be automatically deleted
	}else{
		response.redirect("/login");
	}
}
void onGpio(HttpRequest &request, HttpResponse &response)
{

	TemplateFileStream *tmpl = new TemplateFileStream("gpio.html");
	auto &vars = tmpl->variables();

	vars["checked12"] = digitalRead(12)?"checked":"";
	vars["checked13"] = digitalRead(13)?"checked":"";
	vars["checked14"] = digitalRead(14)?"checked":"";
	vars["checked4"] = digitalRead(4)?"checked":"";

	response.sendTemplate(tmpl);
}

void onLogin(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("login.html");
	auto &vars = tmpl->variables();

	if (request.getRequestMethod() == RequestMethod::POST){

		String userName = request.getPostParameter("username");
		String password = request.getPostParameter("password");

		Serial.println("User Name: " + userName);
		Serial.println("Password: " + password);

		if ((userName.equals("admin")) && (password.equals("123"))){
			isLogin = true;
			response.redirect("/");
		}else{
			vars["message"] = "<div class=\"alert alert-warning\" role=\"alert\"><span>User Name or Password Not found!</span></div>";
			response.sendTemplate(tmpl);
		}
	}else{
		vars["message"] = "";
		response.sendTemplate(tmpl);
	}

}
void onLogout(HttpRequest &request, HttpResponse &response)
{
	isLogin = false;
	response.redirect("/login");
}

void onNetworkConfig(HttpRequest &request, HttpResponse &response)
{
	Serial.println("onNetworkConfig..");
	NetworkSettings.load();
	TemplateFileStream *tmpl = new TemplateFileStream("network.html");
	auto &vars = tmpl->variables();
	String option = "";
	for (int i = 0; i < networks.count(); i++)
	{
		if (networks[i].hidden) continue;
		if (networks[i].ssid == NetworkSettings.ssid){
			option += "<option selected>" + networks[i].ssid + "</option>";
		}else{
			option += "<option>" + networks[i].ssid + "</option>";
		}
	}
	vars["option"] = option;

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		NetworkSettings.ssid = request.getPostParameter("ssid");
		NetworkSettings.password = request.getPostParameter("password");
		NetworkSettings.dhcp = request.getPostParameter("dhcp") == "1";
		NetworkSettings.ip = request.getPostParameter("ip");
		NetworkSettings.netmask = request.getPostParameter("netmask");
		NetworkSettings.gateway = request.getPostParameter("gateway");
		debugf("Updating IP settings: %d", NetworkSettings.ip.isNull());
		NetworkSettings.save();

		vars["dhcpon"] = request.getPostParameter("dhcp") == "1" ? "checked='checked'" : "";
		vars["dhcpoff"] = request.getPostParameter("dhcp") == "0" ? "checked='checked'" : "";
		vars["password"] = request.getPostParameter("password");
		vars["ip"] = request.getPostParameter("ip");
		vars["netmask"] = request.getPostParameter("netmask");
		vars["gateway"] = request.getPostParameter("gateway");

	}else{
		bool dhcp = WifiStation.isEnabledDHCP();
		vars["dhcpon"] = dhcp ? "checked='checked'" : "";
		vars["dhcpoff"] = !dhcp ? "checked='checked'" : "";
		vars["password"] = NetworkSettings.password;
		if (!WifiStation.getIP().isNull())
		{
			vars["ip"] = WifiStation.getIP().toString();
			vars["netmask"] = WifiStation.getNetworkMask().toString();
			vars["gateway"] = WifiStation.getNetworkGateway().toString();
		}
		else
		{
			vars["ip"] = "192.168.1.200";
			vars["netmask"] = "255.255.255.0";
			vars["gateway"] = "192.168.1.1";
		}
	}
	response.sendTemplate(tmpl); // will be automatically deleted
}

void onBrokerConfig(HttpRequest &request, HttpResponse &response)
{
	Serial.println("onBrokerConfig..");
	BrokerSettings.load();
	TemplateFileStream *tmpl = new TemplateFileStream("config.html");
	auto &vars = tmpl->variables();

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		BrokerSettings.active = request.getPostParameter("mqtt") == "1";
		BrokerSettings.user_name = request.getPostParameter("user_name");
		BrokerSettings.password = request.getPostParameter("password");
		BrokerSettings.serverHost = request.getPostParameter("broker");
		BrokerSettings.serverPort = request.getPostParameter("port").toInt();
		BrokerSettings.updateInterval = request.getPostParameter("interval").toInt();
		BrokerSettings.pinNumber = request.getPostParameter("pin").toInt();
		BrokerSettings.token = request.getPostParameter("token");
		BrokerSettings.gpio12Id = request.getPostParameter("gpio12");
		BrokerSettings.gpio13Id = request.getPostParameter("gpio13");
		BrokerSettings.gpio14Id = request.getPostParameter("gpio14");
		BrokerSettings.gpio4Id = request.getPostParameter("gpio4");
		BrokerSettings.save();
	}
	//Serial.println("Host: "+ BrokerSettings.serverHost);
	//Serial.println("Port: "+ String(BrokerSettings.serverPort));
	//Serial.println("Interval: "+ String(BrokerSettings.updateInterval));
	//Serial.println("Pin: "+ String(BrokerSettings.pinNumber));

	vars["mqtton"] = BrokerSettings.active ? "checked='checked'" : "";
	vars["mqttoff"] = !BrokerSettings.active ? "checked='checked'" : "";
	vars["user_name"] = BrokerSettings.user_name;
	vars["password"] = BrokerSettings.password;
	vars["broker"] = BrokerSettings.serverHost;
	vars["port"] = BrokerSettings.serverPort;
	vars["interval"] = BrokerSettings.updateInterval;
	vars["pin"] = BrokerSettings.pinNumber;
	vars["token"] = BrokerSettings.token;
	vars["gpio12"] = BrokerSettings.gpio12Id;
	vars["gpio13"] = BrokerSettings.gpio13Id;
	vars["gpio14"] = BrokerSettings.gpio14Id;
	vars["gpio4"] = BrokerSettings.gpio4Id;
	if (BrokerSettings.active){
		vars["subscribe"] = deviceId + "/" + BrokerSettings.user_name + "/temperature";
		vars["subscribe"] = deviceId + "/" + BrokerSettings.user_name + "/humidity";
	}else{
		vars["subscribe"] = "N/A";
	}

	response.sendTemplate(tmpl);
}
void onTempConfig(HttpRequest &request, HttpResponse &response)
{

	TemplateFileStream *tmpl = new TemplateFileStream("temconfig.html");
	auto &vars = tmpl->variables();

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		TempSettings.tempMin = request.getPostParameter("tempMin").toInt();
		TempSettings.tempMax = request.getPostParameter("tempMax").toInt();
		//TempSettings.humiMin = request.getPostParameter("humiMin").toInt();
		TempSettings.humiMax = request.getPostParameter("humiMax").toInt();
		TempSettings.interval = request.getPostParameter("interval").toInt();
		TempSettings.mode = request.getPostParameter("mode") == "1";
		TempSettings.save();
		loadMonitorConfig();
	}

	TempSettings.load();
	Serial.println("tempMin: "+ TempSettings.tempMin);
	Serial.println("tempMax: "+ String(TempSettings.tempMax));
	//Serial.println("humiMin: "+ String(TempSettings.humiMin));
	Serial.println("humiMax: "+ String(TempSettings.humiMax));
	Serial.println("interval: "+ String(TempSettings.interval));

	vars["tempMin"] = TempSettings.tempMin;
	vars["tempMax"] = TempSettings.tempMax;
	//vars["humiMin"] = TempSettings.humiMin;
	vars["humiMax"] = TempSettings.humiMax;
	vars["interval"] = TempSettings.interval;
	vars["autoon"] = TempSettings.mode ? "checked='checked'" : "";
	vars["autooff"] = !TempSettings.mode ? "checked='checked'" : "";
	loadMonitorConfig();
	response.sendTemplate(tmpl);
}

void onAjaxGpio(HttpRequest &request, HttpResponse &response)
{
	Serial.println("GPIO: " + request.getQueryParameter("gpio"));
	Serial.println("STATE: " + request.getQueryParameter("state"));

	int gpio = request.getQueryParameter("gpio").toInt();
	digitalWrite(gpio, request.getQueryParameter("state") == "true"?HIGH:LOW);

	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["state"] = (bool)digitalRead(gpio);
	json["gpio"] = gpio;
	// send response request
	response.sendJsonObject(stream);

	delete stream;

	// websocket broadcast io status
	String st = digitalRead(gpio)?"1":"0";
	String msg = "{ \"Type\": 2, \"gpio\": " + request.getQueryParameter("gpio") + ", \"status\": " + st  + " }";
	sendSocketClient(msg);
}
void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}

void wsConnected(WebSocket& socket)
{
	//totalActiveSockets++;

	// Notify everybody about new connection
	//WebSocketsList &clients = server.getActiveWebSockets();
	//for (int i = 0; i < clients.count(); i++)
	//	clients[i].sendString("New friend arrived! Total: " + String(totalActiveSockets));
}

void wsMessageReceived(WebSocket& socket, const String& message)
{
	Serial.printf("WebSocket message received:\r\n%s\r\n", message.c_str());
	String response = "Echo: " + message;
	//socket.sendString(response);
}

void wsBinaryReceived(WebSocket& socket, uint8_t* data, size_t size)
{
	Serial.printf("Websocket binary data recieved, size: %d\r\n", size);
}

void wsDisconnected(WebSocket& socket)
{
	//totalActiveSockets--;

	// Notify everybody about lost connection
	//WebSocketsList &clients = server.getActiveWebSockets();
	//for (int i = 0; i < clients.count(); i++)
	//	clients[i].sendString("We lost our friend :( Total: " + String(totalActiveSockets));
}


void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/network", onNetworkConfig);
	server.addPath("/config", onBrokerConfig);
	server.addPath("/ajax/gpio", onAjaxGpio);
	server.addPath("/temconfig", onTempConfig);
	server.addPath("/gpio", onGpio);
	server.addPath("/login", onLogin);
	server.addPath("/logout", onLogout);
	server.setDefaultHandler(onFile);

	// Web Sockets configuration
	server.enableWebSockets(true);
	server.setWebSocketConnectionHandler(wsConnected);
	server.setWebSocketMessageHandler(wsMessageReceived);
	server.setWebSocketBinaryHandler(wsBinaryReceived);
	server.setWebSocketDisconnectionHandler(wsDisconnected);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
	Serial.println("==============================\r\n");

}


