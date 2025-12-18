#pragma once

#include <drogon/drogon.h>
#include <string>

namespace async_flow {
namespace db {

class Dao {
public:
    Dao() {
        clientPtr_ = drogon::app().getDbClient("porcodb");
    }
    virtual ~Dao() = default;
protected:
    drogon::orm::DbClientPtr clientPtr_;
};

}; // namespace db
}; // namespace async_flow

std::string GetTaskTableName(const std::string& taskType);