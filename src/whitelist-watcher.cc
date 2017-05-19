#include <cstring>
#include <errno.h>
#include <fstream>
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

    // Redirect the standard output
    std::ofstream coutFile("rust-whitelist.cout.txt");
    std::ofstream cerrFile("rust-whitelist.cerr.txt");

    // Check if the file was created
    if (coutFile.fail() || cerrFile.fail()) {
        std::cerr << "Error: Impossible to open redirect stream: " << strerror(errno) << std::endl;
    }

    std::cout.rdbuf(coutFile.rdbuf());
    std::cerr.rdbuf(cerrFile.rdbuf());
    fclose(stdin);

    // Monitoring the log file
    FileMonitor logMonitor(argv[1], argv[2]);

    for (;;) {
        logMonitor.watch();
    }

    return 0;
}
