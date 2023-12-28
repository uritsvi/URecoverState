#include <iostream>

#include "..\..\include\URSInterface.h"

#pragma comment(lib, "URS.lib")

#define SAVE_STATE_COMMAND "S"
#define RECOVER_STATE_COMMAND "R"

std::string ToUpper(std::string Str) {
	std::string out = "";

	for (int i = 0; i < Str.length(); i++) {
		out += std::toupper(Str[i]);
	}

	return out;
}

int main() {

	bool res = true;
	do {
		std::cout << "Enter pid:\n";

		std::string pidStr;
		std::cin >> pidStr;

		DWORD pid = atoi(pidStr.c_str());

		res = InitURS(pid);
		if (!res) {
			std::cout << "Error\n";
		}
	} while (!res);

	while (true) {
		std::string cmd;
		std::cin >> cmd;

		if (ToUpper(cmd) == SAVE_STATE_COMMAND) {
			bool res = URSTakeSnapshot();
			if (!res) {
				std::cout << "Failed to take snapshot\n";
			}
		}
		else if (ToUpper(cmd) == RECOVER_STATE_COMMAND) {
			bool res = URSRevertToSnapshot();
			if (!res) {
				std::cout << "Failed to revert to snapshot\n";
			}
		}
		else {
			std::cout << "Unknown command\n";
		}
	}
}