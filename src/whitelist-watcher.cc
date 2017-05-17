#include "local/FileMonitor.h"

int main() {
    // Monitoring the log file
    FileMonitor logMonitor("test.log", "/tmp/whitelist.txt");

    for (;;) {
        logMonitor.watch();
    }

    return 0;
}
