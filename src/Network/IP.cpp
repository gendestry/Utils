#include "Network/IP.h"
#include "Text/Stream.h"
#include <exception>


namespace Utils::Network {


IP::IP(std::string ip)
{
    auto spl = Utils::String::split(ip, ".");
    if(spl.size() != 4)
    {
        throw std::runtime_error("Invalid ip size");
    }

    for(int i = 0; i < spl.size(); i++)
    {
        bytes[i] = static_cast<uint8_t>(std::stoi(spl[i]));
    }
};

uint8_t& IP::operator[](uint8_t index)
{
    if(index > 3)
    {
        throw std::runtime_error("Index to big");
    }

    return bytes[index];
}

std::string IP::str() const
{
    Utils::Text::Stream s;
    s.addFormatted("%d.%d.%d.%d", a, b, c, d);
    // s << a << ".";
    // s << b << ".";
    // s << c << ".";
    // s << d;
    return s.end();
}
}