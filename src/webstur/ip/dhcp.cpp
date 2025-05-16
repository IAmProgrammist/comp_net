#include "pch.h"
#include <iostream>
#include <dhcpcsdk.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <webstur/ip/dhcp.h>

void DHCPHelper::init() {
    std::clog << "Loading WSA library" << std::endl;
    loadWSA();

    std::clog << "Loading DHCP library" << std::endl;
    DWORD dwVersion;
    if (DhcpCApiInitialize(&dwVersion) != 0) {
        throw std::invalid_argument(getErrorTextWithWSAErrorCode("Unable to load DHCP library"));
    }
}

void DHCPHelper::detach() {
    std::clog << "Unloading WSA library" << std::endl;
    unloadWSA();

    std::clog << "Unloading DHCP library" << std::endl;
    DhcpCApiCleanup();
}

addrinfo* DHCPHelper::getAddrInfo(std::string name) {
    addrinfo* res = nullptr;

    if (getaddrinfo(name.c_str(), nullptr, nullptr, &res) != 0)
        throw std::invalid_argument(getErrorTextWithWSAErrorCode("Unable to get addr info"));

    return res;
}

std::string DHCPHelper::getNameInfo(sockaddr_in ip) {
    char host[512];

    if (getnameinfo((sockaddr*) &ip, sizeof(ip), host, sizeof(host), nullptr, 0, 0) != 0)
        throw std::invalid_argument(getErrorTextWithWSAErrorCode("Unable to get name info"));

    return std::string(host, host + strlen(host));
}

std::ostream& DHCPHelper::printDHCPServerInfo(std::ostream& out) {
    ULONG ulOutBufLen = 0;
    PIP_ADAPTER_INFO pInfo = (PIP_ADAPTER_INFO) malloc(sizeof(IP_ADAPTER_INFO));
    PIP_ADAPTER_INFO pAdapter;
    if (GetAdaptersInfo(NULL, &ulOutBufLen) != NO_ERROR) {
        free(pInfo);
        pInfo = (PIP_ADAPTER_INFO) malloc(ulOutBufLen);
    }
    if (GetAdaptersInfo(pInfo, &ulOutBufLen) != NO_ERROR) {
        free(pInfo);
        throw std::invalid_argument(getErrorTextWithWSAErrorCode("Can't get interface info"));
    }
    
    for (pAdapter = pInfo; pAdapter != NULL; pAdapter = pAdapter->Next) {
        if (pAdapter->DhcpEnabled) {
            out << pAdapter->DhcpServer.IpAddress.String << std::endl;
        
            free(pInfo);
            return out;
        }
    }

    free(pInfo);

    return out;
}

void DHCPHelper::renewIP() {
    std::clog << "Getting interface info" << std::endl;
    ULONG ulOutBufLen = 0;
    PIP_INTERFACE_INFO pInfo;
    pInfo = (IP_INTERFACE_INFO*) malloc(sizeof(IP_INTERFACE_INFO));
    if (GetInterfaceInfo(NULL, &ulOutBufLen) != NO_ERROR) {
        free(pInfo);
        pInfo = (IP_INTERFACE_INFO*) malloc(ulOutBufLen);
    }
    if (GetInterfaceInfo(pInfo, &ulOutBufLen) != NO_ERROR) {
        free(pInfo);
        throw std::invalid_argument(getErrorTextWithWSAErrorCode("Can't get interface info"));
    }

    std::clog << "Resetting DHCP settings" << std::endl;
    if (IpReleaseAddress(&pInfo->Adapter[0]) != NO_ERROR)
        std::cerr << getErrorTextWithWSAErrorCode("Can't reset ip address") << std::endl;

    if (IpRenewAddress(&pInfo->Adapter[0]) != NO_ERROR)
        std::cerr << getErrorTextWithWSAErrorCode("Can't reset ip address") << std::endl;

    if (pInfo != NULL) 
        free(pInfo);

    return;
}