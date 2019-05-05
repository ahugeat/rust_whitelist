#include "Rcon.h"

#include <cassert>

#include "RconClient.h"

void RCon::kickSteamID(const std::string &steamID, const std::string &hostname, const std::string &port, const std::string &password) {
    // Connect to server
    RconClient rcon(hostname.c_str(), port.c_str());
    bool authOK = rcon.authenticate(password);
    assert(authOK);

    bool commandOK = rcon.sendCommand("kick " + steamID + " 'You are not whitelisted'");
    assert(commandOK);
}
