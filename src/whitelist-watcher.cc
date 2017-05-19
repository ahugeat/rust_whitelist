#include <iostream>

#include "local/FileMonitor.h"

int main(int argc, char *argv[]) {
    // If a argument is missing
    if (argc != 3) {
        std::cerr << "Error: Missing argument" << std::endl;
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\t" << argv[0] << " LOG_DIRECTORY WHITELIST_FILE" << std::endl;

        return -1;
    }

    // Monitoring the log file
    FileMonitor logMonitor(argv[1], argv[2]);

    for (;;) {
        logMonitor.watch();
    }

    return 0;
}
