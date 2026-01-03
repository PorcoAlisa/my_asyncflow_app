#pragma once

#include <drogon/drogon.h>
#include <string>

namespace async_flow::db {

class Dao {
public:
    Dao() {
        clientPtr_ = drogon::app().getDbClient("porcodb");
    }
    virtual ~Dao() = default;
protected:
    drogon::orm::DbClientPtr clientPtr_;
};

}

std::string GetTaskTableName(const std::string& taskType);