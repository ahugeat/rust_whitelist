#include "FileMonitor.h"

#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>

#include "RCon.h"

FileMonitor::FileMonitor(const std::string &logFilename, const std::string &whitelist)
: m_logWatcher(logFilename, InotifyWrapper::Modified)
, m_logFilename(logFilename)
, m_logFileIndex(0) {
    // Load whitelist
    loadSteamIDsWhitelisted(whitelist);

    // Get the max value for the select
    m_maxFileDescriptor = m_logWatcher.getFileDescriptor();
}

void FileMonitor::kickUnknowSteamIDs() {
    // Get the new intofy events
    auto events = m_logWatcher.getEvents();

    for (auto event: events) {
        if (event.mask == IN_MODIFY) {
            // Open the log file
            std::ifstream logFile(m_logFilename);

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
                    std::string steamID = getSteamID(line);

                    // If the steamID was not whitelisted
                    if (steamID != "" && !steamIDisWhitelisted(steamID)) {
                        // RCon::kickSteamID(steamID);
                        std::cout << steamID << std::endl;
                    }

                    line.clear();
                }
            }
        }
    }
}

void FileMonitor::watch() {
    int returnValue;
    fd_set descriptors;

    // Set the fd set
    FD_ZERO(&descriptors);
    FD_SET(m_logWatcher.getFileDescriptor(), &descriptors);

    returnValue = select(m_maxFileDescriptor + 1, &descriptors, NULL, NULL, NULL);

    if (returnValue < 0) {
        perror("FileMonitor::watch(): Error select()");
        std::exit(-1);
    }
    else if (FD_ISSET(m_logWatcher.getFileDescriptor(), &descriptors)) {
        kickUnknowSteamIDs();
    }
}

std::string FileMonitor::getSteamID(const std::string &line) {
    // Check if a new player connect
    std::regex steamIDRegex(".+\\[[0-9]+/([0-9]+)\\].+");
    std::smatch steamIDMatch;

    // If it isn't a new player
    if (!std::regex_match(line, steamIDMatch, steamIDRegex)) {
        return "";
    }

    // Error in regex, we haven't to never enter here!
    if (steamIDMatch.size() != 2) {
        std::cerr << "Error during the exctraction of player name! FIX IT!" << std::endl;
        return "";
    }
    // The first sub_match is the whole string; the next
    // sub_match is the first parenthesized expression.
    std::ssub_match steamIDSubMatch = steamIDMatch[1];
    std::string steamID = steamIDSubMatch.str();

    // Return the steamID or empty string
    return steamID;
}

bool FileMonitor::steamIDisWhitelisted(const std::string &steamID) const {
    return std::find(m_whitelist.begin(), m_whitelist.end(), steamID) != m_whitelist.end();
}

void FileMonitor::loadSteamIDsWhitelisted(const std::string &filename) {
    std::cout << "FileMonitor::loadSteamIDsWhitelisted()" << std::endl;

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
