#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include <set>

struct DirEvent {
    enum Event {
        REMOVE = 0,
        ADD    = 1,
    };
    Event event;
    std::string path;

    static const char* getEventName(const Event& e) {
        switch (e) {
            case Event::ADD:
                return "add";
            case Event::REMOVE:
                return "remove";
        }
    }
};

class DirMonitor {
public:
    using OnFileHandle = std::function<void(DirEvent event)>;

public:
    DirMonitor(std::string monitorDir, OnFileHandle handle);
    ~DirMonitor();

public:
    // noncopyable
    DirMonitor(const DirMonitor&) = delete;
    void operator=(const DirMonitor&) = delete;

private:
    std::string monitorDir_;

    OnFileHandle onFileHandle_;

    std::unique_ptr<std::thread> monitorThread_;
    std::atomic_bool running_ { true };

    std::set<std::string> paths_;
};

