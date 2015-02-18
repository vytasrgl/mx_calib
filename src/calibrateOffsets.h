#ifndef CALIBRATEOFFSETS_H
#define CALIBRATEOFFSETS_H

#include <functional>
#include <vector>

#include "usb2dynamixel/dynamixel/MotorConfigurations.h"
#include "usb2dynamixel/dynamixel/USB2Dynamixel.h"

/**
 * @param _allMotorIDs all availble motors
 * @param _readMotorValueFunc function has to return (current motorvalue - current offset) of _motorID
 * @param _readMotorOffsetFunc function has to return (current offset) of _motorID
 * @param _setMotorOffsetFunc function has to overwrite offset of given _motorID
 */

void calibrateOffsets(MotorConfigurationsManager &configManager, USB2Dynamixel &usb2dynamixel);


#endif
