#include "pch.h"
#include <ws2ipdef.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <webstur/arp.h>

std::string ARPHelper::prettyPrintPhysicalAddress(unsigned char* addr, int addr_size) {
    std::ostringstream result;

    for (int i = 0; i < addr_size; i++) {
        if (i == addr_size - 1) {
            result << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) addr[i];
        } else {
            result << std::setfill('0') << std::setw(2) << std::hex << (unsigned int) addr[i] << ":";
        }
    }

    return result.str();
}

std::string ARPHelper::getInterfaceIpAddressByIndex(int dwIndex) {
    PIP_ADAPTER_ADDRESSES p_addresses = NULL;
    unsigned long out_buf_len = 0;
    int dw_ret_val = 0;
    auto flags = GAA_FLAG_SKIP_ANYCAST & GAA_FLAG_SKIP_MULTICAST & GAA_FLAG_SKIP_DNS_SERVER;

    // Получить необходимое место для адреса 
    dw_ret_val = GetAdaptersAddresses(AF_INET,
        flags,
        NULL,
        NULL,
        &out_buf_len);

    // Выделить память для адресов
    p_addresses = (PIP_ADAPTER_ADDRESSES)malloc(out_buf_len);

    // Получаем таблицу интерфейсов
    dw_ret_val = GetAdaptersAddresses(AF_INET,
        flags,
        NULL,
        p_addresses,
        &out_buf_len);

    if (dw_ret_val != ERROR_SUCCESS) {
        free(p_addresses);
        throw std::runtime_error("Unable to get adapters table: " + std::to_string(dw_ret_val));
    }

    // Пройтись по всем интерфейсам
    PIP_ADAPTER_ADDRESSES p_curr_addresses = p_addresses;
    while (p_curr_addresses) {
        // Если индекс совпадает с искомым
        if (p_curr_addresses->IfIndex == dwIndex) {
            PIP_ADAPTER_UNICAST_ADDRESS p_unicast = p_curr_addresses->FirstUnicastAddress;
            int i = 0;

            while (p_unicast != NULL) {
                char result[20] = {};

                if (p_unicast->Address.lpSockaddr->sa_family == AF_INET) {
                    // Скопировать IPv4 адрес и вернуть его
                    sockaddr_in* sa_in = (sockaddr_in*)p_unicast->Address.lpSockaddr;
                    inet_ntop(AF_INET, &(sa_in->sin_addr), result, sizeof(result));
                    free(p_addresses);

                    return std::string(result, result + sizeof(result));
                }

                p_unicast = p_unicast->Next;
            }

            break;
        }

        p_curr_addresses = p_curr_addresses->Next;
    }

    free(p_addresses);
    return "";
}


std::ostream& ARPHelper::printARPTable(std::ostream& out) {
	// Получить ARP-таблицу
	PMIB_IPNET_TABLE2 pipTable = NULL;
	auto status = GetIpNetTable2(AF_INET, &pipTable);
	if (status != NO_ERROR) 
		throw std::runtime_error("GetIpNetTable for IPv4 table returned error: " + std::to_string(status));
	
    std::string last_interface = "";

    // Пройтись по всем записям в таблице
    for (int i = 0; i < pipTable->NumEntries; i++) {
        std::string interface = getInterfaceIpAddressByIndex(pipTable->Table[i].InterfaceIndex);

        if (interface != last_interface) {
            // Вывести информацию об интерфейсе
            out << "\nИнтерфейс: " << getInterfaceIpAddressByIndex(pipTable->Table[i].InterfaceIndex) <<
                " --- 0x" << std::hex << pipTable->Table[i].InterfaceIndex << "\n" << std::dec;
            // Вывести колонки
            out << "  " <<
                std::left << std::setfill(' ') << std::setw(24) << "Адрес в Интернете" <<
                std::left << std::setfill(' ') << std::setw(24) << "Физический адрес" <<
                std::left << std::setfill(' ') << std::setw(24) << "Тип" << std::endl;
            
            last_interface = interface;
        }

        // Вывести IP адрес соседа
        char temp_string[16] = {};
        inet_ntop(AF_INET, &pipTable->Table[i].Address.Ipv4.sin_addr, temp_string, sizeof(temp_string));
        out << "  " << std::left << std::setfill(' ') << std::setw(24) << std::string(temp_string, temp_string + strlen(temp_string));

        // Вывести MAC адрес соседа
        out << std::left << std::setfill(' ') << std::setw(24) << prettyPrintPhysicalAddress(pipTable->Table[i].PhysicalAddress, pipTable->Table[i].PhysicalAddressLength);
        
        // Вывести тип
        std::string type;
        switch (pipTable->Table[i].State) {
        case NlnsUnreachable:
            type = "недопустимый";
            break;
        case NlnsIncomplete:
            type = "незавершённый";
            break;
        case NlnsProbe:
            type = "исследуется";
            break;
        case NlnsDelay:
            type = "задержан";
            break;
        case NlnsStale:
            type = "устаревший";
            break;
        case NlnsReachable:
            type = "динамический";
            break;
        case NlnsPermanent:
            type = "статический";
            break;
        case NlnsMaximum:
            type = "максимальный";
            break;
        }
        out << std::left << std::setfill(' ') << std::setw(24) << type;

        out << "\n";
    }

    FreeMibTable(pipTable);

	out.flush();
	return out;
}

MIB_IPNET_ROW2 constructArpRow(std::string ip, std::string address, int interface_index) {
    // Инициализировать запись в ARP таблице
    MIB_IPNET_ROW2 entry = {};

    // Скопировать IP адрес в запись ARP таблицы
    entry.Address.si_family = AF_INET;
    inet_pton(AF_INET, &address[0], &entry.Address.Ipv4.sin_addr);

    // Скопировать MAC адрес в запись ARP таблицы
    entry.PhysicalAddressLength = 6;
    sscanf_s(&address[0], "%02X:%02X:%02X:%02X:%02X:%02X", 
        &entry.PhysicalAddress[0],
        &entry.PhysicalAddress[1], 
        &entry.PhysicalAddress[2], 
        &entry.PhysicalAddress[3], 
        &entry.PhysicalAddress[4], 
        &entry.PhysicalAddress[5]);

    // Скопировать индекс интерфейса в запись ARP таблицы
    entry.InterfaceIndex = interface_index;

    return entry;
}

void ARPHelper::addArpEntry(std::string ip, std::string address, int interface_index) {
    // Сконструировать запись для ARP таблицы
    auto new_entry = constructArpRow(ip, address, interface_index);

    // Добавить запись
    int return_code;
    if ((return_code = CreateIpNetEntry2(&new_entry)) != NO_ERROR)
        throw std::runtime_error("Unable to add entry with error code " + std::to_string(return_code));
} 

void ARPHelper::deleteArpEntry(std::string ip, std::string address, int interface_index) {
    // Сконструировать запись для ARP таблицы
    auto new_entry = constructArpRow(ip, address, interface_index);

    // Удалить запись
    int return_code;
    if ((return_code = DeleteIpNetEntry2(&new_entry)) != NO_ERROR)
        throw std::runtime_error("Unable to add entry with error code " + std::to_string(return_code));
}

std::string ARPHelper::sendARP(std::string find) {
    unsigned long size = sizeof(unsigned long) * 2;
    unsigned char mac[sizeof(unsigned long) * 2];
    memset(mac, 0xff, sizeof(mac));
    unsigned long ip;
    inet_pton(AF_INET, &find[0], &ip);

    // Отправить ARP запрос
    int return_code;
    if ((return_code = SendARP((IPAddr) &ip, ADDR_ANY, mac, &size)) == NO_ERROR)
        throw std::runtime_error("Unable to add entry with error code " + std::to_string(return_code));

    // Преобразовать адрес в строку
    return ARPHelper::prettyPrintPhysicalAddress(mac, size);
}