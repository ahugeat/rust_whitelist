#include "FileMonitor.h"

#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>

#include <sys/inotify.h>
#include <unistd.h>

static constexpr std::size_t EVENT_SIZE  = sizeof (struct inotify_event);
static constexpr std::size_t BUF_LEN     = 1024 * ( EVENT_SIZE + 16 );
static constexpr std::size_t STREAM_LEN  = 8192;

FileMonitor::FileMonitor(const std::string &filename, const std::string &whitelist)
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

    // Load whitelist
    loadSteamIDsWhitelisted(whitelist);
}

FileMonitor::~FileMonitor() {
    // Cleanup inotify
    inotify_rm_watch(m_fd, m_wd);
    close(m_fd);
}

std::vector<std::string> FileMonitor::getUnknowSteamIDs() {
    char buffer[BUF_LEN] = {0};
    std::vector<std::string> steamIDs;

    // Get the new intofy events
    std::int64_t length = read(m_fd, buffer, BUF_LEN);
    if ( length < 0 ) {
        perror("FileMonitor::getUnknowSteamIDs(): Error read()");
        std::exit(-1);
    }

    // Parse the inotify events
    int i = 0;
    while (i < length) {
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

                    // Extracts the steamID if it exits
                    getSteamID(steamIDs, line);

                    line.clear();
                }
            }

            // Remove eof char
            --m_logFileIndex;
        }

        i += EVENT_SIZE + event->len;
    }

    return steamIDs;
}

void FileMonitor::getSteamID(std::vector<std::string> &steamIDs, const std::string &line) {
    // Check if a new player connect
    std::regex steamIDRegex(".+\\[[0-9]+/([0-9]+)\\].+");
    std::smatch steamIDMatch;

    // If it isn't a new player
    if (!std::regex_match(line, steamIDMatch, steamIDRegex)) {
        return;
    }

    // Error in regex, we haven't to never enter here!
    if (steamIDMatch.size() != 2) {
        std::cerr << "Error during the exctraction of player name! FIX IT!" << std::endl;
        return;
    }
    // The first sub_match is the whole string; the next
    // sub_match is the first parenthesized expression.
    std::ssub_match steamIDSubMatch = steamIDMatch[1];
    std::string steamID = steamIDSubMatch.str();

    // Add the steamID to the list
    if (!steamIDisWhitelisted(steamID)) {
        steamIDs.push_back(steamID);
    }
}

bool FileMonitor::steamIDisWhitelisted(const std::string &steamID) const {
    return std::find(m_whitelist.begin(), m_whitelist.end(), steamID) != m_whitelist.end();
}

void FileMonitor::loadSteamIDsWhitelisted(const std::string &filename) {
    std::ifstream file(filename);

    if (file.fail()) {
        return;
    }

    while (!file.eof()) {
        std::string steamID;
        file >> steamID;
        m_whitelist.push_back(steamID);
    }
}
