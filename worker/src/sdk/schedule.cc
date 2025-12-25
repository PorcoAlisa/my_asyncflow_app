#include "schedule.h"
#include <drogon/drogon.h>

namespace async_flow {
namespace worker {

void LoadCfg() {

}

void TaskMgr::Init() {
    LOG_INFO << "Init";
    loadCfgJt_ = std::jthread([](std::stop_token st) {
        while (!st.stop_requested()) {
            LOG_INFO << "keep load cfg";
            LoadCfg();
        }
    });

}

}
}