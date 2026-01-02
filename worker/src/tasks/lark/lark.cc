#include "lark.h"

namespace async_flow::worker {

TaskPtr Lark::CreateLark(api::TaskData data, std::string_view host) {
    return std::make_shared<Lark>(std::move(data), host);
}

}