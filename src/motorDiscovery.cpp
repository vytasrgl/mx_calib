/*
 * motorDiscovery.cpp
 *
 *  Created on: 18.02.2015
 *      Author: lutz
 */

#include "motorDiscovery.h"
#include <iostream>

MotorDiscovery::MotorDiscovery(USB2Dynamixel &usb2dynamixel)
	: m_usb2dynamixel(usb2dynamixel)
{
}

MotorDiscovery::~MotorDiscovery() {
}


std::vector<std::pair<dynamixel::motorID, uint>> MotorDiscovery::scanForMotors(dynamixel::motorID startID, dynamixel::motorID endID, std::vector<uint> baudrates)
{
	std::vector<std::pair<dynamixel::motorID, uint>> ret;

	for (uint baudrate : baudrates)
	{
		std::cout << "testing baudrate " << baudrate << std::endl;
		m_usb2dynamixel.setBaudrate(baudrate);

		for (dynamixel::motorID motor(startID); motor < endID; ++motor)
		{
			std::mutex mutex;
			m_usb2dynamixel.ping(motor, 0.01 * seconds, [&](dynamixel::motorID motor, bool success, uint8_t, const uint8_t*, uint8_t)
				{
					if (success)
					{
						ret.push_back({motor, baudrate});
						std::cout << "found motor " << uint(motor) << " at baudrate " << baudrate << std::endl;
					}
				}, &mutex);
			// wait for the transaction to be completed
			mutex.lock();
		}
	}

	return ret;
}
