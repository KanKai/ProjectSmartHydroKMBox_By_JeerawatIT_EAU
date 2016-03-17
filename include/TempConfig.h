/*
 * TempConfig.h
 *
 *  Created on: Jul 31, 2558 BE
 *      Author: admin
 */
#include <SmingCore/SmingCore.h>

#ifndef INCLUDE_TEMPCONFIG_H_
#define INCLUDE_TEMPCONFIG_H_


#define TEMP_SETTINGS_FILE ".temp.conf" // leading point for security reasons :)

struct TempSettingsStorage
{
	int tempMin;
	int tempMax;
	int humiMin;
	int humiMax;
	int interval;
	bool mode;

	void load()
	{
		DynamicJsonBuffer jsonBuffer;
		if (exist()){
			int size = fileGetSize(TEMP_SETTINGS_FILE);
			char* jsonString = new char[size + 1];
			fileGetContent(TEMP_SETTINGS_FILE, jsonString, size + 1);
			JsonObject& root = jsonBuffer.parseObject(jsonString);

			JsonObject& temp = root["temp"];
			tempMin = temp["tempMin"].toString().toInt();
			tempMax = temp["tempMax"].toString().toInt();
			humiMin = temp["humiMin"].toString().toInt();
			humiMax = temp["humiMax"].toString().toInt();
			interval = temp["interval"].toString().toInt();
			mode = temp["mode"];

			delete[] jsonString;
		}else{
			tempMin = 25;
			tempMax = 35;
			humiMin = 50;
			humiMax = 80;
			interval = 5;
			mode = true;
		}
	}

	void save()
	{
		DynamicJsonBuffer jsonBuffer;
		JsonObject& root = jsonBuffer.createObject();

		JsonObject& temp = jsonBuffer.createObject();
		root["temp"] = temp;
		temp["mode"] = mode;
		temp.addCopy("tempMin", String(tempMin));
		temp.addCopy("tempMax", String(tempMax));
		temp.addCopy("humiMin", String(humiMin));
		temp.addCopy("humiMax", String(humiMax));
		temp.addCopy("interval", String(interval));
		//TODO: add direct file stream writing
		fileSetContent(TEMP_SETTINGS_FILE, root.toJsonString());

	}

	bool exist() { return fileExist(TEMP_SETTINGS_FILE); }
};

static TempSettingsStorage TempSettings;


#endif /* INCLUDE_TEMPCONFIG_H_ */
