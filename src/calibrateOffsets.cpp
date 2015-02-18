#include "calibrateOffsets.h"

#include "utils/utils.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>


static void clearLine()
{
	for (int i(0); i<77; ++i) {
		std::cout << " ";
	}
	std::cout<<"\r";
}

static uint16_t readPosition(dynamixel::motorID motor, USB2Dynamixel &usb2Dynamixel)
{
	std::mutex mutex;
	uint16_t position;
	usb2Dynamixel.read(motor, dynamixel::Register::PRESENT_POSITION, 2, 0.01 * seconds,
			[&](dynamixel::motorID, bool success, uint8_t, const uint8_t* receiveBuffer, uint8_t)
			{
				if (success)
				{
					position = *((uint16_t*)receiveBuffer);
				}
			}, &mutex);
	mutex.lock();
	return position;
}

static void disableStiffness(dynamixel::motorID motor, USB2Dynamixel &usb2Dynamixel)
{
	std::mutex mutex;
	usb2Dynamixel.write(motor, dynamixel::Register::TORQUE_ENABLE, {0}, &mutex);
	mutex.lock();
}


void calibrateOffsets(MotorConfigurationsManager &configManager, USB2Dynamixel &usb2dynamixel) {
	if (configManager.getConfigs().size() < 1) {
		std::cerr<<"No Motors to calibrate found"<<std::endl;
		return;
	}


	std::map<dynamixel::motorID, MotorConfiguration> const& configs = configManager.getConfigs();

	for (auto const& motor : configs)
	{
		usb2dynamixel.setBaudrate(motor.second.baudrate);
		disableStiffness(motor.first, usb2dynamixel);
	}

	std::cout<<"========================================="<<std::endl;
	std::cout<<"This Tool helps calibrating motor offsets"<<std::endl;
	std::cout<<"========================================="<<std::endl;

	enum class State { DisplayChooseID, ChooseID, DisplayCalibrating, Calibrating };
	State state { State::DisplayChooseID };
	int selectedID {0};

	while(true) {
		switch (state) {
		case State::DisplayChooseID:
			std::cout<<"-----------------------------------------"<<std::endl;
			std::cout<<std::endl<<"ChooseID ( ";
			for (auto const& motor : configs)
			{
				std::cout << int(motor.first) << " ";
			}
			std::cout<<"): "<<std::endl;
			state = State::ChooseID;
			break;
		case State::ChooseID: {
			utils::nonblock(false);
			int id {0};
			std::cin >> id;
			if (configs.find(id) == configs.end()) {
				std::cout<<"Not a valid motor"<<std::endl;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				state = State::DisplayChooseID;
			} else {
				state = State::DisplayCalibrating;
				selectedID = id;
				usb2dynamixel.setBaudrate(configs.at(id).baudrate);
			}
			utils::nonblock(true);
			break;
		}
		case State::DisplayCalibrating: {
			std::cout << std::endl;
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "Press Space or Enter to accept or ESC to reject" << std::endl;
			std::cout << "Calbrating motor: " << selectedID << std::endl;
			state = State::Calibrating;
			break;
		}
		case State::Calibrating: {
			clearLine();

			int offset   = configs.at(selectedID).offset;
			uint16_t rawPosition = readPosition(selectedID, usb2dynamixel);
			uint16_t position = rawPosition - offset;
			int newOffset = rawPosition - (1 << 11);

			std::cout << "Raw Position: " << std::setw(5) << rawPosition << "; ";
			std::cout << "Norm Position: " << std::setw(5) << position << "; ";
			std::cout << "Last Offset: " << std::setw(5) << offset << "; ";
			std::cout << "New Offset: " << std::setw(5) << newOffset << "; ";

			std::cout << "\r" << std::flush;
			if (utils::kbhit()) {
				auto c = std::cin.get();
				if (c == 13) {
					clearLine();
					state = State::DisplayChooseID;
				} else if (c == ' ' || c == '\n') {
					clearLine();
					configManager[selectedID].offset = newOffset;
					configManager.save();
					state = State::DisplayChooseID;
				}
			}
			break;
		}
		}

		utils::delay(0.01*seconds);
	}
}
