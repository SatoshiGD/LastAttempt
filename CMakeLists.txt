cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(LastAttempt)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(LastAttempt SHARED ${SOURCE_FILES})

target_include_directories(LastAttempt PRIVATE
  libraries/minhook/include
  libraries/includes/cocos2dx/
  libraries/includes/cocos2dx/include
  libraries/includes/cocos2dx/kazmath/include
  libraries/includes/cocos2dx/platform/win32
  libraries/includes/cocos2dx/platform/third_party/win32
  libraries/includes/cocos2dx/platform/third_party/win32/OGLES
  libraries/includes/extensions
  libraries/includes/gd
  libraries/includes/gd/include
)

add_subdirectory(libraries/minhook)

target_link_libraries(LastAttempt 
  ${CMAKE_SOURCE_DIR}/libraries/lib/libcocos2d.lib
  minhook
  opengl32
)