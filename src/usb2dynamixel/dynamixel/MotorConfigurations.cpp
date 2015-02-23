/*
 * MotorConfigurations.cpp
 *
 *  Created on: 18.02.2015
 *      Author: lutz
 */

#include "MotorConfigurations.h"
#include <iostream>
#include <fstream>
#include <assert.h>

#include "json/json.h"

MotorConfigurationsManager::MotorConfigurationsManager(std::string filename)
	: m_filename(filename)
{
	std::stringstream strStream;
	strStream << std::ifstream(filename).rdbuf();
	std::string str = strStream.str();


	Json::Value rootNode;
	Json::Reader jsonReader;
	jsonReader.parse(str, rootNode);


	Json::Value motorsArray = rootNode["motors"];
	if (not motorsArray.isArray()) {
		motorsArray = Json::Value(Json::arrayValue);
	}

	for (uint32_t i(0); i < motorsArray.size(); ++i) {
		Json::Value &motorNode = motorsArray[i];
		MotorConfiguration config;
		dynamixel::motorID id;
		if (not motorNode["id"].empty() && motorNode["id"].isUInt()) {
			id = motorNode["id"].asUInt();
			assert(id < 254 && "no valid motor id set in file");
		} else {
			assert(false && "no motor id set in file");
		}

		if (not motorNode["baudrate"].empty() && motorNode["baudrate"].isUInt()) {
			config.baudrate = motorNode["baudrate"].asUInt();
		}

		if (not motorNode["offset"].empty() && motorNode["offset"].isInt()) {
			config.offset = motorNode["offset"].asInt();
		}

		if (not motorNode["name"].empty() && motorNode["name"].isString()) {
			config.name = motorNode["name"].asString();
		}

		if (not motorNode["bus"].empty() && motorNode["bus"].isString()) {
			config.bus = motorNode["bus"].asString();
		}

		motorConfigs[id] = config;
	}
}

MotorConfiguration& MotorConfigurationsManager::operator[](dynamixel::motorID id)
{
	assert(id < 254 && "request for invalid motorID");
	return motorConfigs[id];
}

MotorConfiguration const& MotorConfigurationsManager::operator[](dynamixel::motorID id) const
{
	assert(id < 254 && "request for invalid motorID");
	return motorConfigs.at(id);
}


MotorConfigurationsManager::~MotorConfigurationsManager() {
}

void MotorConfigurationsManager::save()
{
	Json::Value rootNode;
	Json::StyledWriter jsonWriter;
	Json::Value motorArray(Json::arrayValue);

	for (auto const& config : motorConfigs)
	{
		Json::Value motor;
		motor["id"] = config.first;
		motor["baudrate"] = config.second.baudrate;
		motor["offset"] = config.second.offset;
		motor["name"] = config.second.name;
		motor["bus"] = config.second.bus;
		motorArray.append(motor);
	}

	rootNode["motors"] = motorArray;

	std::string jsonString = jsonWriter.write(rootNode);

	std::ofstream oFile(m_filename);
	oFile << jsonString;
	oFile.close();
}
