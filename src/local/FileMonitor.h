#ifndef _FILE_MONITOR_H
#define _FILE_MONITOR_H

#include <string>
#include <vector>

#include "InotifyWrapper.h"

class FileMonitor {
public:
    FileMonitor(const std::string &logFilename, const std::string &whitelist);

    void kickUnknowSteamIDs();
    void watch();

private:
    std::string getSteamID(const std::string &line);
    bool steamIDisWhitelisted(const std::string &steamID) const;
    void loadSteamIDsWhitelisted(const std::string &filename);

private:
    InotifyWrapper m_logWatcher;
    std::string m_logFilename;
    std::size_t m_logFileIndex;
    std::vector<std::string> m_whitelist;
    int m_maxFileDescriptor;
};

#endif // _FILE_MONITOR_H
