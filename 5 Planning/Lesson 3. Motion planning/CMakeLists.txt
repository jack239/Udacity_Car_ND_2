cmake_minimum_required(VERSION 3.5)
project(Lesson_3_Motion_planning)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -Wno-sign-compare")

add_executable(3.09.simpson_rule main_9.cpp)

add_executable(3.15.offset_goal main_15.cpp)

add_executable(3.27.velocity_profile main_27.cpp)

add_executable(3.33.collision_detection main_33.cpp)

