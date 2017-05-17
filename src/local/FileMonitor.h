#ifndef _FILE_MONITOR_H
#define _FILE_MONITOR_H

#include <string>
#include <vector>

#include "InotifyWrapper.h"

class FileMonitor {
public:
    FileMonitor(const std::string &filename, const std::string &whitelist);

    std::vector<std::string> getUnknowSteamIDs();

private:
    void getSteamID(std::vector<std::string> &steamIDs, const std::string &line);
    bool steamIDisWhitelisted(const std::string &steamID) const;
    void loadSteamIDsWhitelisted(const std::string &filename);

private:
    // int m_fd;
    // int m_wd;
    InotifyWrapper m_logWatcher;
    std::string m_filename;
    std::size_t m_logFileIndex;
    std::vector<std::string> m_whitelist;
};

#endif // _FILE_MONITOR_H
