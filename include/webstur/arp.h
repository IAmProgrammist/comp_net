#pragma once

#include <ostream>
#include <webstur/utils.h>

class DLLEXPORT ARPHelper {
public:
	ARPHelper();

	static std::ostream& printARPTable(std::ostream& out);
	static void addArpEntry(std::string ip, std::string address, int interface_index);
};