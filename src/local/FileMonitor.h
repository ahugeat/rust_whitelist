#ifndef _FILE_MONITOR_H
#define _FILE_MONITOR_H

#include <string>
#include <vector>

#include "InotifyWrapper.h"

class FileMonitor {
public:
    FileMonitor(const std::string &logDirectory, const std::string &whitelistFilename);

    void kickUnknowSteamIDs();
    void watch();

private:
    std::string getSteamID(const std::string &line) const;
    bool isWhitelisted(const std::string &steamID) const;
    void loadSteamIDsWhitelisted();
    std::string getBaseDir(const std::string &filename) const;

private:
    InotifyWrapper m_logWatcher;
    std::string m_logFilename;
    std::size_t m_logFileIndex;
    std::vector<std::string> m_whitelist;
    InotifyWrapper m_whitelistWatcher;
    std::string m_whitelistFilename;
    int m_maxFileDescriptor;
};

#endif // _FILE_MONITOR_H
