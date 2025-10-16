cmake_minimum_required(VERSION 3.16)
project(Craftium LANGUAGES CXX)

# Export compile commands for linters/IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Remove hardcoded paths - let find_package handle Qt detection
# set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt/6.7.3")
# set(Qt6_DIR "/opt/homebrew/Cellar/qt/6.7.3/lib/cmake/Qt6")
# include_directories("/opt/homebrew/Cellar/qt/6.7.3/include") # Use target_include_directories instead

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically run MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find required Qt packages
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Define the executable
add_executable(Craftium
    src/main.cpp
    src/controllerapp.cpp
    src/playbackworker.cpp
    include/controllerapp.h
    include/playbackworker.h
)

# Link against Qt libraries first
target_link_libraries(Craftium PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

# Include directories for project headers
target_include_directories(Craftium PRIVATE
    include
    ${Qt6Widgets_INCLUDE_DIRS}
)

# Platform specific libraries
if(WIN32)
    target_link_libraries(Craftium PRIVATE user32)
elif(APPLE)
    # We now only need CoreGraphics for CGEvent functions
    target_link_libraries(Craftium PRIVATE "-framework CoreGraphics" "-framework Carbon")
    message(STATUS "Linked with -framework CoreGraphics and -framework Carbon")
endif() 