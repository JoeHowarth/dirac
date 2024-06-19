# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

project(BaseTemplate VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Debug, Release, RelWithDebInfo, MinSizeRel
# Debug enables stack traces with line numbers in lldb
set(CMAKE_BUILD_TYPE Debug) 

# Enable vcpkg integration
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Include FetchContent module
include(FetchContent)

# Configure FetchContent to fetch Flecs
FetchContent_Declare(
  flecs
  GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
  GIT_TAG v4
)

# Make FetchContent download the specified content
FetchContent_MakeAvailable(flecs)

# Add source files
add_executable(
    ${PROJECT_NAME} 
    src/main.cpp 
    )

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find and link dependencies (example: fmt)
find_package(fmt CONFIG REQUIRED)
# find_package(flecs CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        fmt::fmt
        flecs::flecs
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)