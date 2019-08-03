# DirMonitor

[![Build Status](https://www.travis-ci.org/shuai132/DirMonitor.svg?branch=master)](https://www.travis-ci.org/shuai132/DirMonitor)

监控目录变化

## Requirements:
* C++11

## Build:
```bash
mkdir build && cd build && cmake .. && make
```

## Usage:
```cpp
#include "DirMonitor.h"
#include "log.h"

int main() {
    DirMonitor dirMonitor("./", [](DirEvent event) {
        LOGI("DirMonitor: event:%d, path:%s", event.event, event.path.c_str());
    });
    for(;;) { std::this_thread::sleep_for(std::chrono::seconds(1)); };
    return 0;
}
```
