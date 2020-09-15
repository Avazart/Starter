cmake_minimum_required(VERSION 3.5)

project(Starter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(. SRC_LIST)
aux_source_directory(ArgParse SRC_LIST)
aux_source_directory(Bicycle/Win/Common SRC_LIST)
aux_source_directory(Bicycle/Win/Process SRC_LIST)
aux_source_directory(Bicycle/Win/Fs SRC_LIST)
aux_source_directory(Bicycle/Algorithm SRC_LIST)

find_library(Shlwapi_LIBRARY Shlwapi REQUIRED)
find_library(User32_LIBRARY User32 REQUIRED)

add_executable(Starter ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} ${Shlwapi_LIBRARY} ${User32_LIBRARY})

