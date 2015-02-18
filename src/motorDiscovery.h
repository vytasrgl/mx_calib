/*
 * motorDiscovery.h
 *
 *  Created on: 18.02.2015
 *      Author: lutz
 */

#ifndef MOTORDISCOVERY_H_
#define MOTORDISCOVERY_H_

#include "usb2dynamixel/dynamixel/USB2Dynamixel.h"
#include <stdint.h>
#include <vector>
#include <utility>

class MotorDiscovery {
public:
	MotorDiscovery(USB2Dynamixel &usb2dynamixel);
	virtual ~MotorDiscovery();

	 /**
	  * fetch dynamixel motors for a set of baudrates
	  */
	std::vector<std::pair<dynamixel::motorID, uint32_t>> scanForMotors(dynamixel::motorID startID, dynamixel::motorID endID, std::vector<uint32_t> baudrates);

private:
	USB2Dynamixel &m_usb2dynamixel;
};

#endif /* MOTORDISCOVERY_H_ */
