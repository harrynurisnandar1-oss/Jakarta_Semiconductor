cmake_minimum_required(VERSION 3.20)
project(sts_secure_crypto C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)
enable_testing()

find_package(OpenSSL 3.0 REQUIRED COMPONENTS Crypto)

add_library(sts_crypto STATIC
    src/crypto.c
)
target_include_directories(sts_crypto PUBLIC include)
target_link_libraries(sts_crypto PUBLIC OpenSSL::Crypto)
target_compile_options(sts_crypto PRIVATE -Wall -Wextra -Wpedantic)

add_library(sts_core STATIC
    src/kmc_core.c
    src/secure_sm.c
    src/nonsecure_sm.c
)
target_include_directories(sts_core PUBLIC include)
target_link_libraries(sts_core PUBLIC sts_crypto OpenSSL::Crypto)
target_compile_options(sts_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(sts_kmc src/kmc_app.c)
target_link_libraries(sts_kmc PRIVATE sts_core)

add_executable(sts_meter src/nonsecure_app.c)
target_link_libraries(sts_meter PRIVATE sts_core)

add_executable(sts_tests tests/test_sts.c)
target_link_libraries(sts_tests PRIVATE sts_core sts_crypto OpenSSL::Crypto)
target_compile_options(sts_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME sts_crypto_tests COMMAND sts_tests)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(sts_crypto PRIVATE -fstack-protector-strong)
  target_compile_options(sts_core PRIVATE -fstack-protector-strong)
endif()

