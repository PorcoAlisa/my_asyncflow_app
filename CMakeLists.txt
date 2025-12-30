# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "gcc-13")
set(CMAKE_CXX_COMPILER "g++-13")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(hello_drogon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPM_DOWNLOAD_VERSION 0.34.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake")
if(NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
  file(DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
    ${CPM_DOWNLOAD_LOCATION}
  )
endif()
include(${CPM_DOWNLOAD_LOCATION})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

# Make CPM available
include(cmake/CPM.cmake)

# Pull Drogon via CPM
CPMAddPackage(
  NAME drogon
  GIT_REPOSITORY https://github.com/drogonframework/drogon.git
  GIT_TAG v1.9.8   # you can use master, but a tag is more stable
)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork/http
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork/const
  ${CMAKE_CURRENT_SOURCE_DIR}/flowsvr/src/ctrl
  ${CMAKE_CURRENT_SOURCE_DIR}/flowsvr/src/model
  ${CMAKE_CURRENT_SOURCE_DIR}/flowsvr/src/dao
)
target_link_libraries(flowsvr PRIVATE drogon proto)

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
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork/http
  ${CMAKE_CURRENT_SOURCE_DIR}/frmwork/const
  ${CMAKE_CURRENT_SOURCE_DIR}/worker/src/sdk
  ${CMAKE_CURRENT_SOURCE_DIR}/worker/src/tasks/lark
)
target_link_libraries(worker PRIVATE drogon proto)