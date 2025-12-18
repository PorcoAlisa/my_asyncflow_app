#include "dao.h"
#include "comm.h"
#include <cstdio>

using namespace async_flow::db;

std::string GetTaskTableName(const std::string& taskType) {
    return Sprintf("t_%s_task", taskType.c_str());
}