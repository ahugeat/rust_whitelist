#include "FileMonitor.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>

#include "Rcon.h"

FileMonitor::FileMonitor(const std::string &logDirectory, const std::string &whitelistFilename, const std::string &hostname, const std::string &port, const std::string &password)
: m_logWatcher(logDirectory, InotifyWrapper::Created | InotifyWrapper::Modified)
, m_logFilename("")
, m_logFileIndex(0)
, m_whitelistWatcher(getBaseDir(whitelistFilename), InotifyWrapper::Modified)
, m_whitelistFilename(whitelistFilename)
, m_hostname(hostname)
, m_port(port)
, m_password(password) {
    // Load whitelist
    loadSteamIDsWhitelisted();

    // Get the max value for the select
    m_maxFileDescriptor = std::max({ m_logWatcher.getFileDescriptor(), m_whitelistWatcher.getFileDescriptor() });
}

void FileMonitor::kickUnknowSteamIDs() {
    // Open the log file
    std::ifstream logFile(m_logWatcher.getBaseDir() + m_logFilename);

    // Check if the open fail
    if (logFile.fail()) {
        std::cerr << "FileMonitor::kickUnknowSteamIDs(): Error open() " << strerror(errno) << std::endl;
        std::exit(-1);
    }

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
            if (steamID != "" && !isWhitelisted(steamID)) {
                RCon::kickSteamID(steamID, m_hostname, m_port, m_password);
                std::cout << "The player: '" << steamID << "' has been kicked" << std::endl;
            }

            line.clear();
        }
    }
}

void FileMonitor::watch() {
    int returnValue;
    fd_set descriptors;

    // Set the fd set
    FD_ZERO(&descriptors);
    FD_SET(m_logWatcher.getFileDescriptor(), &descriptors);
    FD_SET(m_whitelistWatcher.getFileDescriptor(), &descriptors);

    returnValue = select(m_maxFileDescriptor + 1, &descriptors, NULL, NULL, NULL);

    if (returnValue < 0) {
        perror("FileMonitor::watch(): Error select()");
        std::exit(-1);
    }

    if (FD_ISSET(m_logWatcher.getFileDescriptor(), &descriptors)) {
        auto events = m_logWatcher.getEvents();

        // Check type of event
        for (auto event: events) {
            // If a file is modified
            if (event.mask & InotifyWrapper::Modified) {
                // If the log filename is unset
                if (m_logFilename == "") {
                    m_logFilename = event.name;
                    m_logFileIndex = 0;
                }

                // Check if the modified file is the current log file
                if(m_logFilename == event.name) {
                    kickUnknowSteamIDs();
                }
            }
            // If a file was created, we set the new log filename
            else if (event.mask & InotifyWrapper::Created) {
                m_logFilename = event.name;
                m_logFileIndex = 0;
            }
        }
    }

    if (FD_ISSET(m_whitelistWatcher.getFileDescriptor(), &descriptors)) {
        // Get the new intofy events
        auto events = m_whitelistWatcher.getEvents();

        // Check if the file was updated
        for (auto event: events) {
            if (event.mask & InotifyWrapper::Modified) {
                loadSteamIDsWhitelisted();
            }
        }
    }
}

std::string FileMonitor::getSteamID(const std::string &line) const {
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

bool FileMonitor::isWhitelisted(const std::string &steamID) const {
    return std::find(m_whitelist.begin(), m_whitelist.end(), steamID) != m_whitelist.end();
}

void FileMonitor::loadSteamIDsWhitelisted() {
    // Try to open the whitelist file
    std::ifstream file(m_whitelistFilename);
    if (file.fail()) {
        return;
    }

    // Clear all previous steamID
    m_whitelist.clear();

    // Read line by line the whitelisted steamID
    while (!file.eof()) {
        std::string steamID;
        file >> steamID;

        // To avoid to add a empty string when eof is reached
        if (steamID != "") {
            m_whitelist.push_back(steamID);
        }
    }

    // Remove duplicate steamID
    std::sort(m_whitelist.begin(), m_whitelist.end());
    m_whitelist.erase(std::unique(m_whitelist.begin(), m_whitelist.end()), m_whitelist.end());
}

std::string FileMonitor::getBaseDir(const std::string &filename) const {
    std::string directory = "";

    // Get the base dir
    const size_t lastSlashIndex = filename.rfind('/');
    if (std::string::npos != lastSlashIndex) {
        directory = filename.substr(0, lastSlashIndex);
    }

    // If the file is in current dir
    if (directory == "") {
        directory = "./";
    }

    return directory;
}
