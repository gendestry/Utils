#include "Network/IP.h"
#include "Text/Stream.h"
#include <format>
#include <exception>


namespace Utils::Network {
IP::IP(const IP &other) {
    a=other.a;
    b=other.b;
    c=other.c;
    d=other.d;
}


IP::IP(const std::string& ip)
{
    setIP(ip);
};

IP& IP::operator=(const std::string& ip) {
    setIP(ip);
    return *this;
}

void IP::setIP(const std::string& ip) {
    const auto spl = String::split(ip, ".");
    if(spl.size() != 4)
    {
        throw std::runtime_error("Invalid ip size");
    }

    for(int i = 0; i < spl.size(); i++)
    {
        bytes[i] = static_cast<uint8_t>(std::stoi(spl[i]));
    }
}



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
    // Text::Stream s;
    // s.addFormatted("{}.{}.{}.{}", a, b, c, d);
    // return s.end();

    return std::format("{}.{}.{}.{}", a, b, c, d);
}

// const char* IP::cstr() const {
//     return str().c_str();
// }

}