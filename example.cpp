#include "DirMonitor.h"
#include "log.h"

using namespace DirMonitor;

int main() {
    Monitor monitor(".", [](const Event& event) {
        LOGI("Monitor: path:%s, type:%s, isAdd:%s",
                event.path.name.c_str(),
                event.path.isDir ? "dir" : "file",
                event.type == Event::Type::ADD ? "add" : "remove"
                );
    });
    for(;;) { std::this_thread::sleep_for(std::chrono::seconds(1)); };
    return 0;
}
