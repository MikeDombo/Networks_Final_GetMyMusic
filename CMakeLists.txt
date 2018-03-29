cmake_minimum_required (VERSION 2.8.11)
set (CMAKE_CXX_STANDARD 11)
project (Project4)

add_executable(Project4Server src/Project4Client.cpp src/Project4Common.cpp src/HappyPathJSON.cpp src/lib/CCRC32.cpp)
add_executable(Project4Client src/Project4Server.cpp src/Project4Common.cpp src/HappyPathJSON.cpp src/lib/CCRC32.cpp)
add_executable(JSONTest src/JSONTest.cpp src/HappyPathJSON.cpp)
add_executable(OfflineTester src/OfflineTester.cpp src/Project4Common.cpp)

target_compile_options(Project4Server PUBLIC -std=c++11 -Wall)
target_compile_options(Project4Client PUBLIC -std=c++11 -Wall)
target_compile_options(JSONTest PUBLIC -std=c++11 -Wall)
target_compile_options(OfflineTester PUBLIC -std=c++11 -Wall)

target_include_directories(Project4Client PUBLIC . src/lib/)
target_include_directories(Project4Server PUBLIC . src/lib/)
