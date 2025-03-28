#include "pch.h"
#include <sstream>
#include <WinSock2.h>
#include <iostream>
#include <webstur/iserver.h>
#include <webstur/utils.h>

void IServer::init() {
    std::clog << "Loading WSA library" << std::endl;
    loadWSA();
}

void IServer::detach() {
    std::clog << "Unloading WSA library" << std::endl;
    unloadWSA();
}

IServer::IServer() {
}

IServer::~IServer() {
}