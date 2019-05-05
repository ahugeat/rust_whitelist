#ifndef RCON_CLIENT_H
#define RCON_CLIENT_H

#include <string>

#include <boost/asio.hpp>

namespace {
    constexpr int MaxPacketSize = 4096;
    constexpr int MaxBodySize = MaxPacketSize - 3 * sizeof(int32_t);
}

enum class PacketType: int32_t {
    InvalidPacket           = -1,
    ServerdataResponseValue =  0,
    ServerdataExecommand    =  2,
    ServerdataAuthResponse  =  2,
    ServerdataAuth          =  3,
};

struct RconPacket {
    int32_t size;
    int32_t id;
    PacketType type;
    uint8_t body[MaxBodySize];

    RconPacket();
    void writeBody(const std::string &body);
    void clear();
};

class RconClient {
public:
    RconClient(char *hostname, char *port);

    RconClient(const RconClient&) = delete;
    RconClient& operator=(const RconClient&) = delete;

    RconClient(RconClient&& other) = delete;
    RconClient& operator=(RconClient&& other) = delete;

    bool authenticate(const std::string &password);

private:
    bool sendPacket(const RconPacket &packet);
    bool receivePacket(RconPacket &packet);

private:
    boost::asio::io_service m_ioService;
    boost::asio::ip::tcp::socket m_socket;
};

#endif // RCON_CLIENT_H
