#include "lark.h"

namespace async_flow {
namespace worker {

TaskPtr Lark::CreateLark(const api::TaskData& data, std::string_view host) {
    return std::make_shared<Lark>(data, host);
}

}
}