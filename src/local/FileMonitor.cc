#include "FileMonitor.h"

#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>

FileMonitor::FileMonitor(const std::string &filename, const std::string &whitelist)
: m_logWatcher(filename, InotifyWrapper::Modified)
, m_filename(filename)
, m_logFileIndex(0) {
    // Load whitelist
    loadSteamIDsWhitelisted(whitelist);
}

std::vector<std::string> FileMonitor::getUnknowSteamIDs() {
    // char buffer[BUF_LEN] = {0};
    std::vector<std::string> steamIDs;

    // Get the new intofy events
    auto events = m_logWatcher.getEvents();

    for (auto event: events) {
        if (event.mask == IN_MODIFY) {
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
        }
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
