//
// Created by bobi on 7. 02. 26.
//

#include "Network/Interfaces.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

namespace Utils::Network {
    std::unordered_map<std::string, IP> Interfaces::m_ips;

    void Interfaces::scan()
    {
        rescan();
    }

    void Interfaces::rescan()
    {
        m_ips.clear();
        ifaddrs* ifaddr;

        getifaddrs(&ifaddr);

        for (ifaddrs* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr) continue;

            if (ifa->ifa_addr->sa_family == AF_INET) {
                char ip[INET_ADDRSTRLEN];
                auto* addr = (sockaddr_in*)ifa->ifa_addr;
                inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));

                // Skip loopback
                if (strncmp(ip, "127.", 4) == 0)
                    continue;

                m_ips[ifa->ifa_name] = std::string(ip);
            }
        }
    }

    std::unordered_map<std::string, IP> Interfaces::IPs()
    {
        std::unordered_map<std::string, IP> ret;

        ifaddrs* ifaddr;
        getifaddrs(&ifaddr);

        for (ifaddrs* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr) continue;

            if (ifa->ifa_addr->sa_family == AF_INET) {
                char ip[INET_ADDRSTRLEN];
                auto* addr = (sockaddr_in*)ifa->ifa_addr;
                inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));

                // Skip loopback
                if (strncmp(ip, "127.", 4) == 0)
                    continue;

                ret[ifa->ifa_name] = std::string(ip);
            }
        }

        return ret;
    }


    IP Interfaces::primaryIP()
    {
        int sock = socket(AF_INET, SOCK_DGRAM, 0);

        sockaddr_in serv{};
        serv.sin_family = AF_INET;
        serv.sin_port = htons(53);
        inet_pton(AF_INET, "8.8.8.8", &serv.sin_addr);

        connect(sock, (sockaddr*)&serv, sizeof(serv));

        sockaddr_in name{};
        socklen_t len = sizeof(name);
        getsockname(sock, (sockaddr*)&name, &len);

        close(sock);

        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &name.sin_addr, buf, sizeof(buf));

        return IP(std::string(buf));
    }
}