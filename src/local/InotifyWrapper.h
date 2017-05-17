#ifndef _INOTIFY_WRAPPER_H
#define _INOTIFY_WRAPPER_H

#include <string>
#include <vector>

#include <sys/inotify.h>

typedef struct inotify_event InotifyEvent;

class InotifyWrapper {
public:
    enum EventType {
        Modified = IN_MODIFY,
        Created = IN_CREATE,
    };

public:
    InotifyWrapper(const std::string &filename, EventType watchType);
    virtual ~InotifyWrapper();

    std::vector<InotifyEvent> getEvents() const;

private:
    std::string m_filename;
    int m_inotifyFileDescriptor;
    int m_watcherDescriptor;
};

#endif // _INOTIFY_WRAPPER_H
