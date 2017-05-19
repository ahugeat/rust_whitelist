#ifndef _INOTIFY_WRAPPER_H
#define _INOTIFY_WRAPPER_H

#include <string>
#include <vector>

#include <sys/inotify.h>

struct InotifyEvent {
    int wd;
    uint32_t mask;
    uint32_t cookie;
    std::string name;
};

class InotifyWrapper {
public:
    enum EventType: int {
        Modified = IN_MODIFY,
        Created = IN_CREATE,
        Deleted = IN_DELETE,
    };

public:
    InotifyWrapper(const std::string &filename, EventType watchType);
    virtual ~InotifyWrapper();

    std::string getBaseDir() const;

    std::vector<InotifyEvent> getEvents() const;
    int getFileDescriptor() const;

private:
    std::string m_filename;
    int m_inotifyFileDescriptor;
    int m_watcherDescriptor;
};

inline InotifyWrapper::EventType operator|(InotifyWrapper::EventType a, InotifyWrapper::EventType b) {
    return static_cast<InotifyWrapper::EventType>(static_cast<int>(a) | static_cast<int>(b));
}

#endif // _INOTIFY_WRAPPER_H
