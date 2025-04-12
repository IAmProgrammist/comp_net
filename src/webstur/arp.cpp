#include "pch.h"
#include <ws2ipdef.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <webstur/arp.h>

std::string prettyPrintPhysicalAddress(unsigned char* addr, int addr_size) {
    std::ostringstream result;

    for (int i = 0; i < addr_size; i++) {
        if (i == addr_size - 1) {
            result << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)addr[i];
        } else {
            result << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) addr[i] << ":";
        }
    }

    return result.str();
}

std::string getInterfaceIpAddressByIndex(int dwIndex) {
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    unsigned long outBufLen = 0;
    int dwRetVal = 0;
    auto flags = GAA_FLAG_SKIP_ANYCAST & GAA_FLAG_SKIP_MULTICAST & GAA_FLAG_SKIP_DNS_SERVER;

    // �������� ����������� ����� ��� ������ 
    dwRetVal = GetAdaptersAddresses(AF_INET,
        flags,
        NULL,
        NULL,
        &outBufLen);

    // �������� ������ ��� �������
    pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);

    // �������� ������� �����������
    dwRetVal = GetAdaptersAddresses(AF_INET,
        flags,
        NULL,
        pAddresses,
        &outBufLen);

    if (dwRetVal != ERROR_SUCCESS) {
        free(pAddresses);
        throw std::runtime_error("Unable to get adapters table: " + std::to_string(dwRetVal));
    }

    // �������� �� ���� �����������
    PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses;
    while (pCurrAddresses) {
        // ���� ������ ��������� � �������
        if (pCurrAddresses->IfIndex == dwIndex) {
            PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress;
            int i = 0;

            while (pUnicast != NULL) {
                char result[20] = {};

                if (pUnicast->Address.lpSockaddr->sa_family == AF_INET) {
                    // ����������� IPv4 ����� � ������� ���
                    sockaddr_in* sa_in = (sockaddr_in*)pUnicast->Address.lpSockaddr;
                    inet_ntop(AF_INET, &(sa_in->sin_addr), result, sizeof(result));
                    free(pAddresses);

                    return std::string(result, result + sizeof(result));
                }

                pUnicast = pUnicast->Next;
            }

            break;
        }

        pCurrAddresses = pCurrAddresses->Next;
    }

    free(pAddresses);
    return "";
}


std::ostream& ARPHelper::printARPTable(std::ostream& out) {
	// �������� ARP-�������
	PMIB_IPNET_TABLE2 pipTable = NULL;
	auto status = GetIpNetTable2(AF_INET, &pipTable);
	if (status != NO_ERROR) 
		throw std::runtime_error("GetIpNetTable for IPv4 table returned error: " + std::to_string(status));
	
    std::string last_interface = "";

    // �������� �� ���� ������� � �������
    for (int i = 0; i < pipTable->NumEntries; i++) {
        std::string interface = getInterfaceIpAddressByIndex(pipTable->Table[i].InterfaceIndex);

        if (interface != last_interface) {
            // ������� ���������� �� ����������
            out << "\n���������: " << getInterfaceIpAddressByIndex(pipTable->Table[i].InterfaceIndex) <<
                " --- 0x" << std::hex << pipTable->Table[i].InterfaceIndex << "\n" << std::dec;
            // ������� �������
            out << "  " <<
                std::left << std::setfill(' ') << std::setw(24) << "����� � ���������" <<
                std::left << std::setfill(' ') << std::setw(24) << "���������� �����" <<
                std::left << std::setfill(' ') << std::setw(24) << "���" << std::endl;
            
            last_interface = interface;
        }

        // ������� IP ����� ������
        char temp_string[16] = {};
        inet_ntop(AF_INET, &pipTable->Table[i].Address.Ipv4.sin_addr, temp_string, sizeof(temp_string));
        out << "  " << std::left << std::setfill(' ') << std::setw(24) << std::string(temp_string, temp_string + strlen(temp_string));

        // ������� MAC ����� ������
        out << std::left << std::setfill(' ') << std::setw(24) << prettyPrintPhysicalAddress(pipTable->Table[i].PhysicalAddress, pipTable->Table[i].PhysicalAddressLength);
        
        // ������� ���
        std::string type;
        switch (pipTable->Table[i].State) {
        case NlnsUnreachable:
            type = "������������";
            break;
        case NlnsIncomplete:
            type = "�������������";
            break;
        case NlnsProbe:
            type = "�����������";
            break;
        case NlnsDelay:
            type = "��������";
            break;
        case NlnsStale:
            type = "����������";
            break;
        case NlnsReachable:
            type = "������������";
            break;
        case NlnsPermanent:
            type = "�����������";
            break;
        case NlnsMaximum:
            type = "������������";
            break;
        }
        out << std::left << std::setfill(' ') << std::setw(24) << type;

        out << "\n";
    }

    FreeMibTable(pipTable);

	out.flush();
	return out;
}

void ARPHelper::addArpEntry(std::string ip, std::string address, int interface_index) {

}