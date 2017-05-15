#include "local/FileMonitor.h"
#include "local/RCon.h"

int main() {
    // Monitoring the log file
    FileMonitor logMonitor("/tmp/test.txt", "/tmp/whitelist.txt");

    for (;;) {
        auto steamIDs = logMonitor.getUnknowSteamIDs();
        for (auto steamID: steamIDs) {
            RCon::kickSteamID(steamID);
        }
    }

    return 0;
}
