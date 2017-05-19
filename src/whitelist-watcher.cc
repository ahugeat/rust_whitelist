#include "local/FileMonitor.h"

int main() {
    // Monitoring the log file
    FileMonitor logMonitor("log/", "whitelist.txt");

    for (;;) {
        logMonitor.watch();
    }

    return 0;
}
