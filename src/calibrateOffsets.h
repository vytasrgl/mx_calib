#ifndef CALIBRATEOFFSETS_H
#define CALIBRATEOFFSETS_H

#include <functional>
#include <vector>


/**
 * @param _allMotorIDs all availble motors
 * @param _readMotorValueFunc function has to return (current motorvalue - current offset) of _motorID
 * @param _readMotorOffsetFunc function has to return (current offset) of _motorID
 * @param _setMotorOffsetFunc function has to overwrite offset of given _motorID
 */

void calibrateOffsets(std::vector<int> _allMotorIDs, std::function<int(int _motorID)> _readMotorValueFunc,
                                                     std::function<int(int _motorID)> _readMotorOffsetFunc,
                                                     std::function<void(int _motorID, int _offset)> _setMotorOffsetFunc);


#endif
