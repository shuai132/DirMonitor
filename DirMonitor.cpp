#include <utility>
#include <algorithm>

#include <dirent.h>

#if __WIN32
#include <windows.h>
#define FILE_SEPARATOR '\\'
#else
#define FILE_SEPARATOR '/'
#endif

#include "DirMonitor.h"
#include "log.h"

namespace DirMonitor {

Monitor::Monitor(std::string monitorDir, OnFileHandle handle, const Scheduler& scheduler)
    : monitorDir_(std::move(monitorDir)),
    onFileHandle_(std::move(handle)) {

    if (monitorDir_[monitorDir_.length() - 1] != FILE_SEPARATOR) {
        monitorDir_ += FILE_SEPARATOR;
    }

    monitorThread_ = std::unique_ptr<std::thread>(new std::thread([this, scheduler] {
        LOGD("start monitor dir: %s", monitorDir_.c_str());
        for(; running_; std::this_thread::sleep_for(std::chrono::seconds(1))) {
            const auto& before = paths_;

            using GatherType = std::set<Path>;
            GatherType current;

            auto dir = opendir(monitorDir_.c_str());
            if (dir == nullptr) return;

            while (true) {
                auto p = readdir(dir);
                if (p == nullptr) break;
                bool isDir = [&]{
                #if __WIN32
                    auto attr = GetFileAttributes(p->d_name);
                    return attr & FILE_ATTRIBUTE_DIRECTORY; // NOLINT(hicpp-signed-bitwise)
                #else
                    return p->d_type == DT_DIR;
                #endif
                }();

                std::string dirName(p->d_name);
                if (dirName == "." || dirName == "..") continue;

                current.insert(Path{monitorDir_ + dirName, isDir});
            }
            closedir(dir);

            GatherType intersection;
            std::set_intersection(before.cbegin(), before.cend(), current.cbegin(), current.cend(),
                    std::inserter<GatherType>(intersection, intersection.begin()));

            GatherType gatherAdded;
            GatherType gatherRemoved;
            std::set_difference(current.cbegin(), current.cend(), intersection.cbegin(), intersection.cend(),
                    std::inserter<GatherType>(gatherAdded, gatherAdded.begin()));
            std::set_difference(before.cbegin(), before.cend(), intersection.cbegin(), intersection.cend(),
                    std::inserter<GatherType>(gatherRemoved, gatherRemoved.begin()));

            auto cb = [this, &scheduler](Event::Type event, const Path& path) {
                if (onFileHandle_) {
                    if (scheduler) {
                        scheduler([=]{
                            onFileHandle_(Event {event, path});
                        });
                    } else {
                        onFileHandle_(Event {event, path});
                    }
                }
            };
            for (const auto& path : gatherAdded) {
                LOGD("gatherAdded.size()=%zu", gatherAdded.size());
                cb(Event::ADD, path);
            }
            for (const auto& path : gatherRemoved) {
                LOGD("gatherRemoved.size()=%zu", gatherRemoved.size());
                cb(Event::REMOVE, path);
            }

            paths_.swap(current);
        }
    }));
}

Monitor::~Monitor() {
    running_ = false;
    monitorThread_->join();
}

}
