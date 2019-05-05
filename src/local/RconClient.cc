#include "RconClient.h"

#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

using boost::asio::ip::tcp;

RconPacket::RconPacket()
: size(0)
, id(0)
, type(PacketType::InvalidPacket) {
    clear();
}

void RconPacket::writeBody(const std::string &command) {
    int32_t bodySize = (command.length() + 1) * sizeof(uint8_t);

    if (bodySize + 1 > MaxBodySize) {
        throw std::runtime_error("RconPacket: Body size exceeded");
    }

    // Copy the string including the null character
    for(int32_t i = 0; i < bodySize; ++i) {
        body[i] = static_cast<uint8_t>(command[i]);
    }
    body[bodySize + 1] = 0;


    size = 2 * sizeof(int32_t) + bodySize + sizeof(uint8_t);
}

void RconPacket::clear() {
    size = 0;
    id = 0;
    type = PacketType::InvalidPacket;
    std::memset(body, 0, MaxBodySize);
}

RconClient::RconClient(char *hostname, char *port)
: m_socket(m_ioService) {
    tcp::resolver resolver(m_ioService);
    boost::asio::connect(m_socket, resolver.resolve({ hostname, port }));
}

bool RconClient::authenticate(const std::string &password) {
    RconPacket packet;
    packet.id = 0;
    packet.type = PacketType::ServerdataAuth;
    packet.writeBody(password);

    // Send the password
    bool sendOK = sendPacket(packet);
    assert(sendOK);

    // BUG: No response receive but command executed
    // // Reveive the server response
    // packet.clear();
    // bool receiveOK = receivePacket(packet);
    // assert(receiveOK);
    //
    // std::cout << "Size: " << packet.size << std::endl;
    // std::cout << "ID: " << packet.id << std::endl;
    // std::cout << "Type: " << static_cast<int32_t>(packet.type) << std::endl;
    // std::cout << "body: " << packet.body << std::endl;
    std::this_thread::sleep_for(1s);

    return true;
}

bool RconClient::sendCommand(const std::string &command) {
    RconPacket packet;
    packet.id = 0;
    packet.type = PacketType::ServerdataExecommand;
    packet.writeBody(command);

    // Send the password
    bool sendOK = sendPacket(packet);
    assert(sendOK);

    // BUG: No response receive but command executed
    // // Reveive the server response
    // packet.clear();
    // bool receiveOK = receivePacket(packet);
    // assert(receiveOK);
    //
    // std::cout << "Size: " << packet.size << std::endl;
    // std::cout << "ID: " << packet.id << std::endl;
    // std::cout << "Type: " << static_cast<int32_t>(packet.type) << std::endl;
    // std::cout << "body: " << packet.body << std::endl;
    std::this_thread::sleep_for(1s);

    return true;
}

bool RconClient::sendPacket(const RconPacket &packet) {
    size_t effectiveSize = static_cast<size_t>(packet.size + sizeof(int32_t));
    size_t requestSize = boost::asio::write(m_socket, boost::asio::buffer(&packet, effectiveSize));

    return requestSize == effectiveSize;
}

bool RconClient::receivePacket(RconPacket &packet) {
    size_t replySize = boost::asio::read(m_socket, boost::asio::buffer(&packet, MaxPacketSize));

    return replySize == static_cast<size_t>(packet.size + sizeof(int32_t));
}
