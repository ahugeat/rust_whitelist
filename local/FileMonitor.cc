#include "FileMonitor.h"

#include <array>
#include <fstream>
#include <iostream>
#include <limits>

#include <sys/inotify.h>
#include <unistd.h>

static constexpr std::size_t EVENT_SIZE  = sizeof (struct inotify_event);
static constexpr std::size_t BUF_LEN     = 1024 * ( EVENT_SIZE + 16 );
static constexpr std::size_t STREAM_LEN  = 8192;

FileMonitor::FileMonitor(const std::string &filename)
: m_fd(-1)
, m_wd(-1)
, m_filename(filename)
, m_logFileIndex(0) {
    // Create the inotify descriptor
    m_fd = inotify_init();
    if (m_fd < 0) {
        perror("FileMonitor::FileMonitor(): Error inotify_init()");
        std::exit(-1);
    }

    // Add watch
    m_wd = inotify_add_watch(m_fd, filename.c_str(), IN_MODIFY);
    if (m_wd < 0) {
        perror("FileMonitor::FileMonitor(): Error inotify_add_watch()");
        std::exit(-1);
    }
}

FileMonitor::~FileMonitor() {
    // Cleanup inotify
    inotify_rm_watch(m_fd, m_wd);
    close(m_fd);
}

std::vector<std::string> FileMonitor::getUnknowPlayers() {
    char buffer[BUF_LEN] = {0};

    // Get the new intofy events
    std::int64_t length = read(m_fd, buffer, BUF_LEN);
    if ( length < 0 ) {
        perror("FileMonitor::getUnknowPlayers(): Error read()");
        std::exit(-1);
    }

    // Parse the inotify events
    int i = 0;
    while ( i < length ) {
        struct inotify_event *event = reinterpret_cast<struct inotify_event*>(&buffer[i]);

        // Check if the file log was modified
        if (event->mask == IN_MODIFY) {
            // Open the log file
            std::ifstream logFile(m_filename);

            // Jump to the next unread data
            logFile.seekg(m_logFileIndex);

            // Read line by line
            std::string line;
            while (!logFile.eof()) {
                // Read char by char
                char c = 0;
                logFile.read(&c, sizeof(char));
                line.push_back(c);
                ++m_logFileIndex;

                // When the line is finished
                if (c == '\n') {
                    // Remove last '\n'
                    line.pop_back();

                    std::cout << line << std::endl;
                    line.clear();
                }
            }

            // Remove eof char
            --m_logFileIndex;
        }

        i += EVENT_SIZE + event->len;
    }

    return std::vector<std::string>();
}
