#include "pch.h"
#include <sstream>
#include <WinSock2.h>
#include <iostream>
#include <webstur/iclient.h>
#include <webstur/utils.h>

void IClient::init() {
    std::clog << "Loading WSA library" << std::endl;
    loadWSA();
}

void IClient::detach() {
    std::clog << "Unloading WSA library" << std::endl;
    unloadWSA();
}

IClient::IClient() {
}

IClient::~IClient() {
}

void IClient::request() {
    return this->request(nullptr, 0);
}