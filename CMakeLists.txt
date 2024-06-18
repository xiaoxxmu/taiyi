cmake_minimum_required(VERSION 3.16)

project("client_taiyi_1.0.0")

set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${CMAKE_SOURCE_DIR}/deps/ctp_api
    ${CMAKE_SOURCE_DIR}/src/common
)

set(
    SRCS
    src/taiyi_main.cc
    src/common/container.cc
    src/common/module.cc
    src/common/proc.cc
    src/common/mem.cc
    src/ctp_service/ctp_service_intf.cc
    src/ctp_service/ctp_service.cc
    src/ctp_service/md_spi.cc
    src/ctp_service/trade_spi.cc
    src/md/instrument_md.cc
    src/trade/instrument_trade.cc
)
add_executable(${PROJECT_NAME} ${SRCS})

link_directories(${CMAKE_SOURCE_DIR}/deps/ctp_api/linux_x64)
target_link_libraries(
    ${PROJECT_NAME}
    pthread
    thostmduserapi_se
    thosttraderapi_se
)
