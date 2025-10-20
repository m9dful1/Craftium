cmake_minimum_required(VERSION 3.16)
project(Craftium LANGUAGES CXX OBJCXX)

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
set(SOURCES
    src/main.cpp
    src/controllerapp.cpp
    src/playbackworker.cpp
    include/controllerapp.h
    include/playbackworker.h
)

# Add macOS-specific Objective-C++ helper on Apple platforms
if(APPLE)
    list(APPEND SOURCES
        src/macos_window_helper.mm
        include/macos_window_helper.h
    )
endif()

add_executable(Craftium ${SOURCES})

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
    # Link required macOS frameworks
    target_link_libraries(Craftium PRIVATE
        "-framework CoreGraphics"
        "-framework Carbon"
        "-framework AppKit"
    )
    message(STATUS "Linked with -framework CoreGraphics, -framework Carbon, and -framework AppKit")
endif() 