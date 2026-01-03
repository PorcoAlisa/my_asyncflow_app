# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

#set(CMAKE_C_COMPILER "gcc-13")
#set(CMAKE_CXX_COMPILER "g++-13")
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(asyncflow_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon CONFIG REQUIRED)

add_subdirectory(proto)

file(GLOB FRMWORK_SRC "frmwork/*.cc" "frmwork/const/*.cc")
file(GLOB FLOWSVR_SRC
  "flowsvr/src/*.cc"
  "flowsvr/src/ctrl/*.cc"
  "flowsvr/src/model/*.cc"
  "flowsvr/src/dao/*.cc"
)

add_executable(flowsvr
  ${FLOWSVR_SRC}
  ${FRMWORK_SRC}
)

target_include_directories(flowsvr PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork/const
  ${CMAKE_CURRENT_SOURCE_DIR}/flowsvr/src/ctrl
  ${CMAKE_CURRENT_SOURCE_DIR}/flowsvr/src/model
  ${CMAKE_CURRENT_SOURCE_DIR}/flowsvr/src/dao
)
target_link_libraries(flowsvr PRIVATE Drogon::Drogon proto)

file(GLOB WORKER_SRC
  "worker/src/*.cc"
  "worker/src/sdk/*.cc"
  "worker/src/tasks/lark/*.cc"
)

add_executable(worker
  ${WORKER_SRC}
  ${FRMWORK_SRC}
)
target_include_directories(worker PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork/const
  ${CMAKE_CURRENT_SOURCE_DIR}/worker/src/sdk
  ${CMAKE_CURRENT_SOURCE_DIR}/worker/src/tasks/lark
)
target_link_libraries(worker PRIVATE Drogon::Drogon proto)