#include "RCon.h"

#include <cstdlib>

void RCon::kickSteamID(const std::string &steamID) {
    // Call ruco tool
    std::string command = "/usr/bin/ruco rcon \"kick " + steamID + " 'You are not whitelisted'\"";
    std::system(command.c_str());
}
