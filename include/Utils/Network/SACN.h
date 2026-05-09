//
// Created by bobi on 30. 03. 26.
//

#pragma once
#include <arpa/inet.h>
#include <array>
#include <string>

#include "IP.h"

namespace Utils::Network
{

    class SacnSender
    {
        uint8_t m_universe = 0;
        uint8_t m_sequence = 0;

        bool m_inited = false;
        int m_socket = -1;
        sockaddr_in m_destinationIp{};
        std::array<uint8_t, 638> m_packet = {0};

        void initializePacket();

    public:
        SacnSender();
        SacnSender(uint8_t universe, const IP& localIp);
        // SacnSender(SacnSender &&other);

        ~SacnSender();

        void begin(uint8_t universe, const IP& localIp);
        void send(const std::array<uint8_t, 512>& packet);

        void setIP(const IP& ip);
        void setSourceName(const std::string& name);

    };
}
