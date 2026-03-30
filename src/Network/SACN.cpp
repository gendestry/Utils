//
// Created by bobi on 30. 03. 26.
//

#include "Network/SACN.h"
#include <sys/socket.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
#include <cstring>

namespace Utils::Network {
    SacnSender::SacnSender()
        : m_sequence(0)
    {
    }

    SacnSender::SacnSender(uint8_t universe, const IP& localIp)
        : m_universe(universe), m_sequence(0)
    {
        begin(universe, localIp);
    };

    // SacnSender::SacnSender(SacnSender &&other)
    // {
    //     m_universe = other.m_universe;
    //     m_sequence = other.m_sequence;
    //     m_socket = other.m_socket;
    //     m_destinationIp = other.m_destinationIp;
    //     m_inited = other.m_inited;
    //     other.m_socket = -1;
    // }

    SacnSender::~SacnSender()
    {
        close(m_socket);
    }

    void SacnSender::begin(uint8_t universe, const IP& localIp)
    {
        m_universe = universe;
        setIP(localIp);

        m_destinationIp.sin_family = AF_INET;
        m_destinationIp.sin_port = htons(5568);

        IP ip("239.255.0.0");
        ip[3] = universe;

        // inet_pton(AF_INET, ip.cstr(), &m_destinationIp.sin_addr);
        inet_pton(AF_INET, ip.str().c_str(), &m_destinationIp.sin_addr);

        initializePacket();
        setSourceName("bobar");

        m_inited = true;
    }

    void SacnSender::send(const std::array<uint8_t, 512> &packet) {
        if (!m_inited)
        {
            return;
        }

        m_packet[111] = m_sequence++;
        memcpy(&m_packet[126], packet.data(), 512);
        ssize_t sent = sendto(m_socket, m_packet.data(), sizeof(m_packet), 0, reinterpret_cast<sockaddr *>(&m_destinationIp), sizeof(m_destinationIp));
        if (sent != sizeof(m_packet))
        {
            throw std::runtime_error("Sacn send failed");
        }
    }

    void SacnSender::initializePacket()
    {
        // m_packet.empty();

        m_packet[0] = 0x00;
        m_packet[1] = 0x10;
        m_packet[2] = 0x00;
        m_packet[3] = 0x00;

        memcpy(&m_packet[4], "ASC-E1.17\0\0\0", 12);
        m_packet[16] = 0x70;
        m_packet[17] = 0x64;
        m_packet[18] = 0x00;
        m_packet[19] = 0x00;
        m_packet[38] = 0x00;
        m_packet[39] = 0x01;
        m_packet[113] = m_universe; // Universe 1
        m_packet[125] = 0x02;       // DMX start code
    }

    void SacnSender::setIP(const IP& ip)
    {
        close(m_socket);
        m_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (m_socket < 0)
        {
            throw std::runtime_error("Socket creation failed");
        }

        sockaddr_in local{};
        inet_pton(AF_INET, ip.str().c_str(), &local.sin_addr);
        if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_IF, &local, sizeof(local)))
        {
            throw std::runtime_error("setsockopt failed");
        }

        // unsigned char loop = 1;
        // if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
        // {
        //     throw std::runtime_error("setsockopt2 failed");
        // }
    }

    // Max 63 chars + null terminator
    void SacnSender::setSourceName(const std::string& name)
    {
        char src[64] = {0};
        std::strncpy(src, name.c_str(), 63);
        memcpy(&m_packet[44], src, 64);
    }
}
