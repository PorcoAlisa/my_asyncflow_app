#include "dao.h"
#include "comm.h"
#include <cstdio>


std::string GetTaskTableName(const std::string& taskType) {
    return Sprintf("t_%s_task", taskType.c_str());
}