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
	commonOptions::Option<std::string> cnfDeviceName("device", "/dev/ttyUSB0", "Device name");
	commonOptions::Option<std::string> cfgConfigFile("file", "motorConfig.json", "file to work on");

	commonOptions::Option<int> cfgSetupMotorID("id", -1, "setup motor to 1000000 baud and id (parameter)");
	commonOptions::Switch swtScanAll("scanAll", "scan all motors");

	commonOptions::Switch swtHelp("help", "show help", []() {
		commonOptions::print();
		exit(EXIT_SUCCESS);
	});
}

static void runSimpleTasks(USB2Dynamixel &usb2dyn)
{
	if (0 <= *cfgSetupMotorID) {
		std::cout << "running setup for motor " << *cfgSetupMotorID << std::endl;
		{
			usb2dyn.setBaudrate(dynamixel::baudIndexToBaudrate(34));
			utils::delay(0.1 * seconds);
			{
				std::mutex mutex;
				usb2dyn.write(dynamixel::broadcastID, dynamixel::Register::BAUD_RATE, {1}, &mutex);
				mutex.lock();
				utils::delay(0.1 * seconds);
			}
		}
		{
			usb2dyn.setBaudrate(dynamixel::baudIndexToBaudrate(1));
			utils::delay(0.1 * seconds);
			{
				std::mutex mutex;
				usb2dyn.write(dynamixel::broadcastID, dynamixel::Register::STATUS_RETURN_LEVEL, {1}, &mutex);
				mutex.lock();
				utils::delay(0.1 * seconds);
			}
			{
				std::mutex mutex;
				usb2dyn.write(dynamixel::broadcastID, dynamixel::Register::ID, {uint8_t(*cfgSetupMotorID)}, &mutex);
				mutex.lock();
				utils::delay(0.1 * seconds);
			}
			{
				std::mutex mutex;
				usb2dyn.ping(dynamixel::motorID(*cfgSetupMotorID), 0.1 * seconds, [](dynamixel::motorID, bool success, uint8_t, const uint8_t*, uint8_t)
						{
							if (not success) {
								std::cout << "motor is not set" << std::endl;
							} else {
								std::cout << "motor is set" << std::endl;
							}
						}
				, &mutex);
				mutex.lock();
			}
		}

		MotorDiscovery discovery(usb2dyn);

		std::vector<uint32_t> baudrates{dynamixel::baudIndexToBaudrate(1)};
		std::vector<std::pair<dynamixel::motorID, uint32_t>> motors = discovery.scanForMotors(0, 253, baudrates);

		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char** argv) {
	if (not commonOptions::parse(argc, argv)) {
		std::cerr << "Invalid paramer" << std::endl;
		commonOptions::print();
		exit(EXIT_FAILURE);

	}
	std::vector<std::string> deviceNames;
	deviceNames.push_back(*cnfDeviceName);
	USB2Dynamixel usb2dyn(deviceNames, 50);

	MotorDiscovery discovery(usb2dyn);

	runSimpleTasks(usb2dyn);
 

	std::vector<uint32_t> baudrates{dynamixel::baudIndexToBaudrate(1)};
	if (*swtScanAll) {
		baudrates.clear();
		for (uint8_t baud(1); baud < 40; ++baud) {
			baudrates.push_back(dynamixel::baudIndexToBaudrate(baud));
		}
	}
	std::vector<std::pair<dynamixel::motorID, uint32_t>> motors = discovery.scanForMotors(0, 50, baudrates);
	return EXIT_SUCCESS;
}
