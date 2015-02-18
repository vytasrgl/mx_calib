/*
 * MotorConfigurations.h
 *
 *  Created on: 18.02.2015
 *      Author: lutz
 */

#ifndef MOTORCONFIGURATIONS_H_
#define MOTORCONFIGURATIONS_H_

#include "usb2dynamixel/dynamixel/dynamixel.h"
#include <map>
#include <string>

struct MotorConfiguration
{
	uint baudrate{0};
	int offset{0};
	std::string name{""};
};

class MotorConfigurationsManager {
public:
	MotorConfigurationsManager(std::string filename);
	virtual ~MotorConfigurationsManager();

	void save();

	void updateMotor(MotorConfiguration const& motor);

	MotorConfiguration& operator[](dynamixel::motorID id);
	MotorConfiguration const& operator[](dynamixel::motorID id) const;

	std::map<dynamixel::motorID, MotorConfiguration> const& getConfigs() const {
		return motorConfigs;
	}

private:
	std::map<dynamixel::motorID, MotorConfiguration> motorConfigs;
	std::string m_filename;
};

#endif /* MOTORCONFIGURATIONS_H_ */
