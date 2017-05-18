#include "local/FileMonitor.h"

int main() {
    // Monitoring the log file
    FileMonitor logMonitor("test.log", "whitelist.txt");

    for (;;) {
        logMonitor.watch();
    }

    return 0;
}
