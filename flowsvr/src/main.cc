#include <cstdlib>  // for std::srand
#include <ctime>    // for std::time

#include "ctrl.h"
#include "create_task.h"
#include "hold_tasks.h"
#include "set_task.h"
#include "get_task_schedule_cfg_list.h"

#include <iostream>

using namespace async_flow::flowsvr;
using namespace async_flow::frmwork;

int main() {
    std::srand(std::time(nullptr));
    RegisterHandler<CreateTaskHandler>(drogon::app(), "/create_task");
    RegisterHandler<HoldTasksHandler>(drogon::app(), "/hold_tasks");
    RegisterHandler<SetTaskHandler>(drogon::app(), "/set_task");
    RegisterHandler<GetTaskScheduleCfgListHandler>(drogon::app(), "/get_task_schedule_cfg_list");
    drogon::app().loadConfigFile("./flowsvr/config/config.json").run();
    return 0;
}