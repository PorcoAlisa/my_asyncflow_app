#include "schedule.h"
#include "task.h"
#include <thread>
#include "lark.h"

using namespace async_flow::worker;
// using namespace async_flow::frmwork;

int main() {
    std::thread t([](){
        TaskMgr mgr("lark", "http:127.0.0.1:39002", Lark::CreateLark);
    });
    t.join();
}