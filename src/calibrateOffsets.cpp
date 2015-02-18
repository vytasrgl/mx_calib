#include "calibrateOffsets.h"

#include "utils/utils.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <limits>



void calibrateOffsets(std::vector<int> _allMotorIDs, std::function<int(int _motorID)> _readMotorValueFunc,
                                                     std::function<int(int _motorID)> _readMotorOffsetFunc,
                                                     std::function<void(int _motorID, int _offset)> _setMotorOffsetFunc) {
	if (_allMotorIDs.size() < 1) {
		std::cerr<<"No Motors to calibrate found"<<std::endl;
		return;
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
			std::cout<<std::endl<<"ChooseID (";
			std::cout<<_allMotorIDs[0];
			for (int i(1); i<int(_allMotorIDs.size()); ++i) {
				std::cout<<", "<<_allMotorIDs[i];
			}
			std::cout<<"): "<<std::endl;
			state = State::ChooseID;
			break;
		case State::ChooseID: {
			utils::nonblock(false);
			int id {0};
			std::cin>>id;
			if (std::find(_allMotorIDs.begin(), _allMotorIDs.end(), id) == _allMotorIDs.end()) {
				std::cout<<"Not a valid motor"<<std::endl;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				state = State::DisplayChooseID;
			} else {
				state = State::DisplayCalibrating;
				selectedID = id;
			}
			utils::nonblock(true);
			break;
		}
		case State::DisplayCalibrating: {
			std::cout<<std::endl;
			std::cout<<"-----------------------------------------"<<std::endl;
			std::cout<<"Press Space or Enter to accept or ESC to reject"<<std::endl;
			std::cout<<"Calbrating motor: "<<selectedID<<std::endl;
			state = State::Calibrating;
			break;
		}
		case State::Calibrating: {
			for (int i(0); i<77; ++i) std::cout<<" "; std::cout<<"\r";
			auto offset   = _readMotorOffsetFunc(selectedID);
			auto position = _readMotorValueFunc(selectedID);
			auto rawPosition = position + offset;
			auto newOffset = rawPosition;
			std::cout<<"Raw Position: "<<std::setw(5)<<rawPosition<<"; ";
			std::cout<<"Norm Position: "<<std::setw(5)<<position<<"; ";
			std::cout<<"Last Offset: "<<std::setw(5)<<offset<<"; ";
			std::cout<<"New Offset: "<<std::setw(5)<<newOffset<<"; ";

			std::cout<<"\r"<<std::flush;
			if (utils::kbhit()) {
				auto c = std::cin.get();
				if (c == 13) {
					for (int i(0); i<77; ++i) std::cout<<" "; std::cout<<"\r";
					state = State::DisplayChooseID;
				} else if (c == ' ' || c == '\n') {
					for (int i(0); i<77; ++i) std::cout<<" "; std::cout<<"\r";
					_setMotorOffsetFunc(selectedID, newOffset);
					state = State::DisplayChooseID;
				}
			}
			break;
		}
		}

		utils::delay(0.01*seconds);
	}
}
