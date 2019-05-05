#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "local/FileMonitor.h"

void smooth_exit(int sigNum) {
    std::string sigName;

    switch (sigNum) {
        case SIGHUP:
            sigName = "SIGHUP";
            break;
        case SIGINT:
            sigName = "SIGINT";
            break;
        case SIGTERM:
            sigName = "SIGTERM";
            break;
        default:
            sigName = "Unknow signal";
    }

    std::cout << sigName << " was raised: shutdown" << std::endl;

    std::exit(0);
}

int main(int argc, char *argv[]) {
    // If a argument is missing
    if (argc != 6) {
        std::cerr << "Error: Missing argument" << std::endl;
        std::cerr << "Usage:" << std::endl;
        std::cerr << "\t" << argv[0] << " LOG_DIRECTORY WHITELIST_FILE HOSTNAME PORT PASSWORD" << std::endl;

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

    // Lanch the deamon
    pid_t pid, sid;

    // //Fork the Parent Process
    pid = fork();

    if (pid < 0) {
        std::cerr << "Impossible to fork" << std::endl;
        std::exit(-1);
    }

    //We got a good pid, Close the Parent Process
    if (pid > 0) {
        std::exit(0);
    }

    //Change File Mask
    umask(0);

    //Create a new Signature Id for our child
    sid = setsid();
    if (sid < 0) {
        std::cerr << "Impossible to set the session ID" << std::endl;
        std::exit(-1);
    }

    signal(SIGHUP, smooth_exit);
    signal(SIGINT, smooth_exit);
    signal(SIGTERM, smooth_exit);

    // We create the log monitor after change directory
    char *pwd;
    pwd = get_current_dir_name();

    // Create absolute path
    std::string logDirPath = std::string(pwd) + "/" + argv[1];
    std::string whitelistPath = std::string(pwd) + "/" + argv[2];

    //Change Directory
    //If we cant find the directory we exit with failure.
    if (chdir("/") < 0) {
        std::cerr << "Impossible to change directory" << std::endl;
        std::exit(-1);
    }

    // Monitoring the log file
    std::string hostname(argv[3]);
    std::string port(argv[4]);
    std::string password(argv[5]);

    FileMonitor logMonitor(logDirPath, whitelistPath, hostname, port, password);

    std::cout << "The monitor is started" << std::endl;
    for (;;) {
        logMonitor.watch();
    }

    return 0;
}
