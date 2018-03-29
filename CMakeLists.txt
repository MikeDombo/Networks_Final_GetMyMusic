cmake_minimum_required (VERSION 2.8.11)
set (CMAKE_CXX_STANDARD 11)
project (Project4)

add_executable(Project4Server Project4Client.cpp Project4Common.cpp HappyPathJSON.cpp lib/CCRC32.cpp)
add_executable(Project4Client Project4Server.cpp Project4Common.cpp HappyPathJSON.cpp lib/CCRC32.cpp)
add_executable(JSONTest JSONTest.cpp HappyPathJSON.cpp)

target_compile_options(Project4Server PUBLIC -std=c++11 -Wall)
target_compile_options(Project4Client PUBLIC -std=c++11 -Wall)
target_compile_options(JSONTest PUBLIC -std=c++11 -Wall)

target_include_directories(Project4Client PUBLIC . lib/)
target_include_directories(Project4Server PUBLIC . lib/)
