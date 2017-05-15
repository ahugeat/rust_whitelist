#include <iostream>

#include "local/FileMonitor.h"

int main() {
    // Monitoring the log file
    FileMonitor logMonitor("/tmp/test.txt", "/tmp/whitelist.txt");

    for (;;) {
        auto steamIDs = logMonitor.getUnknowSteamIDs();
        for (auto steamID: steamIDs) {
            std::cout << steamID << std::endl;
        }
    }

    return 0;
}
