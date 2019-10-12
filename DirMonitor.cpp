#include <utility>
#include <algorithm>

#include <dirent.h>

#include "DirMonitor.h"
#include "log.h"

DirMonitor::DirMonitor(std::string monitorDir, OnFileHandle handle, const Scheduler& scheduler)
    : monitorDir_(std::move(monitorDir)),
    onFileHandle_(std::move(handle)) {

    if (monitorDir_[monitorDir_.length() - 1] != '/') {
        monitorDir_ += "/";
    }

    monitorThread_ = std::unique_ptr<std::thread>(new std::thread([this, scheduler] {
        LOGD("start monitor dir: %s", monitorDir_.c_str());
        for(; running_; std::this_thread::sleep_for(std::chrono::seconds(1))) {
            const auto& before = paths_;

            using GatherType = std::set<std::string>;
            GatherType current;

            auto dir = opendir(monitorDir_.c_str());
            if (dir == nullptr) return;

            while (true) {
                auto p = readdir(dir);
                if (p == nullptr) break;
                if (p->d_type != DT_DIR) continue;

                std::string dirName(p->d_name);
                if (dirName == "." || dirName == "..") continue;

                current.insert(std::move(dirName));
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

            auto cb = [this, &scheduler](DirEvent::Event event, std::string path) {
                if (onFileHandle_) {
                    if (scheduler) {
                        scheduler([=]{
                            onFileHandle_(DirEvent {event, monitorDir_ + path});
                        });
                    } else {
                        onFileHandle_(DirEvent {event, monitorDir_ + path});
                    }
                }
            };
            for (const auto& path : gatherAdded) {
                LOGD("gatherAdded.size()=%zu", gatherAdded.size());
                cb(DirEvent::ADD, path);
            }
            for (const auto& path : gatherRemoved) {
                LOGD("gatherRemoved.size()=%zu", gatherRemoved.size());
                cb(DirEvent::REMOVE, path);
            }

            paths_.swap(current);
        }
    }));
}

DirMonitor::~DirMonitor() {
    running_ = false;
    monitorThread_->join();
}
