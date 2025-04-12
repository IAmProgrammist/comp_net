#include <iostream>
#include <webstur/arp.h>

int main() {
	setlocale(LC_ALL, "Russian");
	ARPHelper::printARPTable(std::cout);

	return 0;
}