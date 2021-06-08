cmake_minimum_required(VERSION 3.16)

set(Prototype ${PROJECT_NAME})
set(0.1.0 ${PROJECT_VERSION})

project(Prototype VERSION ${PROJECT_VERSION})

include(CTest)
enable_testing()

include_directories(include)
file(GLOB SOURCES "*.cpp")
file(GLOB TESTS "./tests/*.cpp")

add_executable(Prototype 
    ${SOURCES} ${TESTS}
)
include(CPack)
