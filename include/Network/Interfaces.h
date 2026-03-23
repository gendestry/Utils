#pragma once
#include <string>
#include <unordered_map>
#include "Network/IP.h"

namespace Utils::Network
{
    struct Interfaces {
        static std::unordered_map<std::string, IP> m_ips;
    
        static void scan();
        static void rescan();
    
        static std::unordered_map<std::string, IP> IPs();
    
        static IP primaryIP();
    };
}