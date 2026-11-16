cmake_minimum_required(VERSION 3.16)
project(OnlineExamCppBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(online_exam_server server.cpp)

if(WIN32)
    target_link_libraries(online_exam_server PRIVATE ws2_32 advapi32)
endif()

