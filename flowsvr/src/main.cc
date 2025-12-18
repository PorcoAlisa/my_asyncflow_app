#include <cstdlib>  // for std::srand
#include <ctime>    // for std::time

#include "handler_base.h"
#include "create_task.h"
#include "ctrl.h"

#include <iostream>

using namespace async_flow::flowsvr;
using namespace async_flow::frmwork;

int main() {
    std::srand(std::time(nullptr));
    RegisterHandler<CreateTaskHandler>(app(), "/create_task");
    // RegisterHandler<HoldTasksCtrl>(app(), "/hold_tasks");
    drogon::app().loadConfigFile("./flowsvr/config/config.json").run();
    return 0;
}