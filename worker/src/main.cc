#include "schedule.h"
#include "task.h"
#include <thread>
#include "lark.h"

using namespace async_flow::worker;
// using namespace async_flow::frmwork;

int main() {
    drogon::app().loadConfigFile("./worker/config/config.json");

    std::string taskType = "lark";
    std::string taskHost = "http:127.0.0.1:39002";

    auto config = drogon::app().getCustomConfig();
    if (config.isMember("app_settings")) {
        taskType = config["app_settings"].get("type", taskType).asString();
        taskHost = config["app_settings"].get("host", taskHost).asString();
    }
    auto mgr = std::make_shared<TaskMgr>(taskType, taskHost);

    mgr->Init();

    drogon::app().run();
}
