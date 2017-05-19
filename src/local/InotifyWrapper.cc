#include "InotifyWrapper.h"

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

static constexpr std::size_t EVENT_SIZE  = sizeof(InotifyEvent);
static constexpr std::size_t BUF_LEN     = 1024 * (EVENT_SIZE + 16);

InotifyWrapper::InotifyWrapper(const std::string &filename, EventType watchType)
: m_filename(filename)
, m_inotifyFileDescriptor(-1)
, m_watcherDescriptor(-1) {
    // Init the inotify descriptor
    m_inotifyFileDescriptor = inotify_init();
    if (m_inotifyFileDescriptor < 0) {
        perror("InotifyWrapper::InotifyWrapper(): Error inotify_init()");
        std::exit(-1);
    }

    // Add watch
    m_watcherDescriptor = inotify_add_watch(m_inotifyFileDescriptor, m_filename.c_str(), watchType);
    if (m_watcherDescriptor < 0) {
        perror("InotifyWrapper::InotifyWrapper(): Error inotify_add_watch()");
        std::exit(-1);
    }
}

InotifyWrapper::~InotifyWrapper() {
    inotify_rm_watch(m_inotifyFileDescriptor, m_watcherDescriptor);
    close(m_inotifyFileDescriptor);
}

std::string InotifyWrapper::getBaseDir() const {
    return m_filename;
}

std::vector<InotifyEvent> InotifyWrapper::getEvents() const {
    char buffer[BUF_LEN] = {0};
    std::vector<InotifyEvent> events;

    // Get the new intofy events
    std::int64_t length = read(m_inotifyFileDescriptor, buffer, BUF_LEN);
    if (length < 0) {
        perror("InotifyWrapper::getEvents(): Error read()");
        std::exit(-1);
    }

    // Parse the inotify events
    int i = 0;
    while (i < length) {
        struct inotify_event *event = reinterpret_cast<struct inotify_event*>(&buffer[i]);

        // Create a wrapper
        InotifyEvent eventWrapper;
        eventWrapper.wd = event->wd;
        eventWrapper.mask = event->mask;
        eventWrapper.name = std::string(event->name);

        events.push_back(eventWrapper);

        i += EVENT_SIZE + event->len;
    }

    return events;
}

int InotifyWrapper::getFileDescriptor() const {
    return m_inotifyFileDescriptor;
}
