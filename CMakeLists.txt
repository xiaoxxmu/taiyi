cmake_minimum_required(VERSION 3.16)

project("client_taiyi_1.0.0")

set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${CMAKE_SOURCE_DIR}/deps/ctp_api
)

set(
    SRCS
    src/main.cc
    src/ctpapi.cc
    src/instrument.cc
    src/mdspi.cc
    src/tradespi.cc
)
add_executable(${PROJECT_NAME} ${SRCS})

link_directories(${CMAKE_SOURCE_DIR}/deps/ctp_api/linux_x64)
target_link_libraries(
    ${PROJECT_NAME}
    pthread
    thostmduserapi_se
    thosttraderapi_se
)
