cmake_minimum_required(VERSION 3.20)
project(DLMS_COSEM_Firmware C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build Host Unit Tests" ON)

include_directories(protocols/dlms/inc)

add_library(dlms_core STATIC
    protocols/dlms/src/dlms_association.c
    protocols/dlms/src/dlms_transport.c
    protocols/dlms/src/dlms_apdu.c
    protocols/dlms/src/dlms_obis.c
    protocols/dlms/src/dlms_tamper_log.c
    protocols/dlms/src/dlms_hdlc.c
    protocols/dlms/src/dlms_server.c
    protocols/dlms/src/dlms_task.c
    protocols/dlms/src/dlms_nvm.c
    drivers/bsp/src/bsp_lpuart_dma.c
)

 
target_include_directories(dlms_core PUBLIC 
protocols/dlms/inc
drivers/bsp/inc )

if(BUILD_TESTS)
    enable_testing()

    add_executable(test_dlms_association tests/protocols/test_dlms_association.c)
    target_link_libraries(test_dlms_association PRIVATE dlms_core)
    add_test(NAME test_dlms_association COMMAND $<TARGET_FILE:test_dlms_association>)

    add_executable(test_dlms_apdu tests/protocols/test_dlms_apdu.c)
    target_link_libraries(test_dlms_apdu PRIVATE dlms_core)
    add_test(NAME test_dlms_apdu COMMAND $<TARGET_FILE:test_dlms_apdu>)

    add_executable(test_dlms_integration tests/protocols/test_dlms_integration.c)
    target_link_libraries(test_dlms_integration PRIVATE dlms_core)
    add_test(NAME test_dlms_integration COMMAND $<TARGET_FILE:test_dlms_integration>)

    add_executable(test_dlms_tamper_log tests/protocols/test_dlms_tamper_log.c)
    target_link_libraries(test_dlms_tamper_log PRIVATE dlms_core)
    add_test(NAME test_dlms_tamper_log COMMAND $<TARGET_FILE:test_dlms_integration>)

    add_executable(test_dlms_hdlc tests/protocols/test_dlms_hdlc.c)
    target_link_libraries(test_dlms_hdlc PRIVATE dlms_core)
    add_test(NAME test_dlms_hdlc COMMAND $<TARGET_FILE:test_dlms_integration>)

    add_executable(test_dlms_server tests/protocols/test_dlms_server.c)
    target_link_libraries(test_dlms_server PRIVATE dlms_core)
    add_test(NAME test_dlms_server COMMAND $<TARGET_FILE:test_dlms_integration>)

    add_executable(test_dlms_task tests/protocols/test_dlms_task.c)
    target_link_libraries(test_dlms_task PRIVATE dlms_core)
    add_test(NAME test_dlms_task COMMAND $<TARGET_FILE:test_dlms_integration>)

    add_executable(test_dlms_nvm tests/protocols/test_dlms_nvm.c)
    target_link_libraries(test_dlms_nvm PRIVATE dlms_core)
    add_test(NAME test_dlms_nvm COMMAND $<TARGET_FILE:test_dlms_integration>)

    add_executable(test_bsp_lpuart tests/drivers/test_bsp_lpuart.c)
    target_link_libraries(test_bsp_lpuart PRIVATE dlms_core)
    add_test(NAME test_bsp_lpuart COMMAND $<TARGET_FILE:test_dlms_integration>)

endif()