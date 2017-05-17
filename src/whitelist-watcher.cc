#include "local/FileMonitor.h"
#include "local/RCon.h"

#include <iostream>

int main() {
    // Monitoring the log file
    FileMonitor logMonitor("test.log", "/tmp/whitelist.txt");

    for (;;) {
        auto steamIDs = logMonitor.getUnknowSteamIDs();
        for (auto steamID: steamIDs) {
            std::cout << steamID << std::endl;
            // RCon::kickSteamID(steamID);
        }
    }

    return 0;
}
