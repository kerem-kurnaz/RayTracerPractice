cmake_minimum_required(VERSION 3.10)

# 1. Name the project
project(inOneWeekend VERSION 1.0)

# 2. Specify the C++ standard (The book uses modern C++ features)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Add the executable
# This assumes your main code is in a file named main.cpp
add_executable(inOneWeekend main.cpp)

# 4. Include current directory for headers
# This helps the compiler find your .h files (like vec3.h, ray.h) 
# when you start splitting code into multiple files.
target_include_directories(inOneWeekend PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 5. Optional: Add optimization flags for non-Windows compilers
# This ensures that even if you forget the "Release" flag, 
# it tries to stay relatively fast.
if(NOT MSVC)
    target_compile_options(inOneWeekend PRIVATE -Wall -Wextra -O3)
endif()