#include "schedule.h"
#include "task.h"
#include <thread>
#include "lark.h"

using namespace async_flow::worker;
// using namespace async_flow::frmwork;

int main() {
    drogon::app().loadConfigFile("./worker/config/config.json");
    TaskMgr mgr("lark", "http:127.0.0.1:39002");

    mgr.Init();

    drogon::app().run();
}
