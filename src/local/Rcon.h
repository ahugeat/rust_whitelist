#ifndef _RECON_H
#define _RECON_H

#include <string>

class RCon {
public:
    static void kickSteamID(const std::string &steamID, const std::string &hostname, const std::string &port, const std::string &password);
};

#endif // _RECON_H
