//============================================================================
// Name        : mx_calib.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "motorDiscovery.h"
#include "usb2dynamixel/dynamixel/dynamixel.h"
#include "usb2dynamixel/dynamixel/USB2Dynamixel.h"

#include "json/json.h"
#include "commonOptions/commonOptions.h"

namespace
{
	commonOptions::Option<std::string> cfgDevice("device", "/dev/ttyUSB0", "serial device to connect to the motors to");
	commonOptions::Option<std::string> cfgConfigFile("file", "motorConfig.json", "file to work on");
}

int main() {

	USB2Dynamixel usb2dyn("/dev/ttyUSB0", 20);
	MotorDiscovery discovery(usb2dyn);


	std::ifstream inFile;
	inFile.opencfg(*ConfigFile);

	std::stringstream strStream;
	strStream << inFile.rdbuf();
	std::string str = strStream.str();
	inFile.close();

	Json::Value rootNode;
	Json::Reader jsonReader;
	Json::StyledWriter jsonWriter;
	jsonReader.parse(str, rootNode);

	std::vector<uint>baudrates{dynamixel::baudIndexToBaudrate(1), dynamixel::baudIndexToBaudrate(34)};
	std::vector<std::pair<dynamixel::motorID, uint>> motors = discovery.scanForMotors(0, 253, baudrates);

	Json::Value motorsArray = rootNode["motors"];
	if (not motorsArray.isArray()) {
		motorsArray = Json::Value(Json::arrayValue);
	}

	for (auto const& motor : motors) {
		// test if we know that motor allready and if so update it's node
		Json::Value *targetMotorNode(nullptr);

		for (uint i(0); i < motorsArray.size(); ++i) {
			Json::Value &motorNode = motorsArray[i];
			if (motorNode["id"] == motor.first) {
				targetMotorNode = &motorNode;
				break;
			}
		}

		if (nullptr != targetMotorNode) {
			// update the node
			(*targetMotorNode)["id"] = motor.first;
			(*targetMotorNode)["baudrate"] = motor.second;
		} else {
			Json::Value newMotorNode;
			newMotorNode["id"] = motor.first;
			newMotorNode["baudrate"] = motor.second;
			motorsArray.append(newMotorNode);
		}

	}

	rootNode["motors"] = motorsArray;
	str = jsonWriter.write(rootNode);

	std::ofstream oFile;
	oFile.open(*cfgConfigFile);
	oFile << str;
	oFile.close();
	return 0;
}
