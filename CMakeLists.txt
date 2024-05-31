cmake_minimum_required(VERSION 3.16)
project(plot_shapes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-w")

# Find Python 3.12
find_package(Python3 3.12 REQUIRED COMPONENTS Development NumPy)
include_directories(${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})

# Find Boost
find_package(Boost 1.71 REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# Find nlohmann_json
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp)
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}  # To include matplotlibcpp.h in the current directory
    /opt/homebrew/lib/python3.12/site-packages/numpy/core/include
)

# Source file
set(SOURCE_FILES task.cpp)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${Python3_LIBRARIES}
    Python3::NumPy
    Boost::system
    Boost::filesystem
)

