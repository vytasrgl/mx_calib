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
#include "usb2dynamixel/dynamixel/MotorConfigurations.h"

#include "calibrateOffsets.h"

#include "json/json.h"
#include "commonOptions/commonOptions.h"

namespace
{
	commonOptions::Option<std::string> cfgDevice("device", "/dev/ttyUSB0", "serial device to connect to the motors to");
	commonOptions::Option<std::string> cfgConfigFile("file", "motorConfig.json", "file to work on");

	commonOptions::Switch swtHelp("help", "show help", []() {
		commonOptions::print();
		exit(EXIT_SUCCESS);
	});
}

int main(int argc, char** argv) {
	if (not commonOptions::parse(argc, argv)) {
		std::cerr << "Invalid paramer" << std::endl;
		commonOptions::print();
		exit(EXIT_FAILURE);

	}

	USB2Dynamixel usb2dyn(*cfgDevice, 20);
	MotorDiscovery discovery(usb2dyn);

	std::vector<uint32_t> baudrates{dynamixel::baudIndexToBaudrate(1), dynamixel::baudIndexToBaudrate(34)};
	std::vector<std::pair<dynamixel::motorID, uint32_t>> motors = discovery.scanForMotors(0, 253, baudrates);

	MotorConfigurationsManager configManager(*cfgConfigFile);

	for (auto const& motor : motors)
	{
		configManager[motor.first].baudrate = motor.second;
	}


	calibrateOffsets(configManager, usb2dyn);

	return EXIT_SUCCESS;
}
