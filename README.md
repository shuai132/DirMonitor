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
```
