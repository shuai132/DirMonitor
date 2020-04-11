#include "DirMonitor.h"
#include "log.h"

int main() {
    DirMonitor dirMonitor(".", [](const DirEvent& event) {
        LOGI("DirMonitor: event:%d, path:%s", event.event, event.path.c_str());
    });
    for(;;) { std::this_thread::sleep_for(std::chrono::seconds(1)); };
    return 0;
}
