//
// Created by bobi on 30. 03. 26.
//

#pragma once
#include <cstdint>

#include "IP.h"
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdexcept>

namespace Utils::Network {
    enum class Protocol : uint8_t {
        TCP = 0,
        UDP = 1,

    };

    struct Config {
        IP destination;
        uint16_t port;
    };

    template <Protocol TProtocol>
    struct Client {
        int m_socket = -1;
        Config m_config;
        sockaddr_in m_destination{};

        Client(const Config& config)
        {
            m_config = config;
        }

        void createSocket() {
            close(m_socket);
            if (TProtocol == Protocol::TCP) {
                m_socket = socket(AF_INET, SOCK_STREAM, 0);
            }
            else if (TProtocol == Protocol::UDP) {
                m_socket = socket(AF_INET, SOCK_DGRAM, 0);
            }

            if (m_socket < 0) {
                throw std::runtime_error("Socket creation failed");
            }

        }

        void initMulticast(const IP& localIP) {
            if (m_socket == -1) {
                createSocket();
            }
            sockaddr_in local{};
            inet_pton(AF_INET, localIP.str().c_str(), &local.sin_addr);
            if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_IF, &local, sizeof(local)))
            {
                throw std::runtime_error("setsockopt failed");
            }
        }

        void begin() {
            m_destination.sin_family = AF_INET;
            m_destination.sin_port = htons(m_config.port);

            // IP ip("239.255.0.0");
            // ip[3] = universe;

            // inet_pton(AF_INET, ip.cstr(), &m_destinationIp.sin_addr);
            inet_pton(AF_INET, m_config.destination.str().c_str(), &m_destination.sin_addr);

        }

        void send(void* data, uint32_t length) {
            ssize_t sent = sendto(m_socket, data, length, 0, reinterpret_cast<sockaddr *>(&m_destination), sizeof(m_destination));
            if (sent != sizeof(length))
            {
                throw std::runtime_error("Sacn send failed");
            }
        }



    };
}
