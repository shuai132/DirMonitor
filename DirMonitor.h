#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include <set>

namespace DirMonitor {

struct Path {
    std::string name;
    bool isDir;

    bool operator<(const Path& e) const {
        return name < e.name;
    }
};

struct Event {
    enum Type {
        REMOVE = 0,
        ADD    = 1,
    };
    Type type;
    Path path;
};

class Monitor {
    using OnFileHandle = std::function<void(Event event)>;
    using Scheduler = std::function<void(std::function<void()>)>;

public:
    Monitor(std::string monitorDir, OnFileHandle handle, const Scheduler& scheduler = nullptr);
    ~Monitor();

public:
    // noncopyable
    Monitor(const Monitor&) = delete;
    void operator=(const Monitor&) = delete;

private:
    std::string monitorDir_;

    OnFileHandle onFileHandle_;

    std::unique_ptr<std::thread> monitorThread_;
    std::atomic_bool running_ { true };

    std::set<Path> paths_;
};

}
