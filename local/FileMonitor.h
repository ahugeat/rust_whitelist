#ifndef _FILE_MONITOR_H
#define _FILE_MONITOR_H

#include <string>
#include <vector>

class FileMonitor {
public:
    FileMonitor(const std::string &filename);
    virtual ~FileMonitor();

    std::vector<std::string> getUnknowPlayers();

private:
    int m_fd;
    int m_wd;
    std::string m_filename;
    std::size_t m_logFileIndex;
};

#endif // _FILE_MONITOR_H
