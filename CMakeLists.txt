cmake_minimum_required(VERSION 3.21)
project(medtech-clinician-ui LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt6 MOC, RCC, UIC automation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find Qt6
find_package(Qt6 COMPONENTS
    Core
    Gui
    Qml
    Quick
    OpenGL
    Svg
    REQUIRED
)

# Find MQTT library
find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO libmosquitto REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MOSQUITTO_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    src/main.cpp
    src/mqtt/mqtt_client.cpp
    src/mqtt/mqtt_payload.cpp
    src/ui/dashboard_window.cpp
    src/ui/vital_renderer.cpp
    src/models/vital_reading.cpp
    src/models/ui_model.cpp
    src/utils/logger.cpp
    src/utils/config.cpp
)

set(HEADERS
    src/mqtt/mqtt_client.h
    src/mqtt/mqtt_payload.h
    src/ui/dashboard_window.h
    src/ui/vital_renderer.h
    src/models/vital_reading.h
    src/models/ui_model.h
    src/utils/logger.h
    src/utils/config.h
)

# QML files (will be compiled into resource)
set(QML_FILES
    qml/main.qml
    qml/VitalCard.qml
    qml/AlarmPanel.qml
    qml/Dashboard.qml
)

# Create resource file for QML
set(RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/resources.qrc)
file(WRITE ${RESOURCE_FILE} "<!DOCTYPE RCC>
<RCC>
    <qresource prefix=\"/\">
")

foreach(QML_FILE ${QML_FILES})
    # Normalize path separators to forward slashes (important on Windows where
    # CMake variables may contain backslashes) and XML-escape special characters
    # so the generated .qrc file is well-formed on all platforms.
    string(REPLACE "\\" "/" QML_ALIAS "${QML_FILE}")
    string(REPLACE "\\" "/" QML_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${QML_FILE}")

    # XML character escaping (order matters: & must come first)
    foreach(VAR QML_ALIAS QML_SOURCE_PATH)
        string(REPLACE "&" "&amp;" ${VAR} "${${VAR}}")
        string(REPLACE "<" "&lt;"  ${VAR} "${${VAR}}")
        string(REPLACE ">" "&gt;"  ${VAR} "${${VAR}}")
        string(REPLACE "\"" "&quot;" ${VAR} "${${VAR}}")
        string(REPLACE "'" "&apos;" ${VAR} "${${VAR}}")
    endforeach()

    file(APPEND ${RESOURCE_FILE} "        <file alias=\"${QML_ALIAS}\">${QML_SOURCE_PATH}</file>\n")
endforeach()

file(APPEND ${RESOURCE_FILE} "    </qresource>
</RCC>
")

# Add resource file to sources
list(APPEND SOURCES ${RESOURCE_FILE})

# Create executable
add_executable(dashboard ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(dashboard
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::OpenGL
    Qt6::Svg
    ${MOSQUITTO_LIBRARIES}
)

# Include directories for target
target_include_directories(dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MOSQUITTO_INCLUDE_DIRS}
)

# Compiler flags
if(MSVC)
    target_compile_options(dashboard PRIVATE /W4 /WX)
else()
    target_compile_options(dashboard PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic 
        -Werror=return-type
        -Wno-deprecated-declarations
    )
endif()

# Enable testing
enable_testing()

# Test executable
set(TEST_SOURCES
    tests/test_mqtt_client.cpp
    tests/test_vital_renderer.cpp
    src/mqtt/mqtt_client.cpp
    src/mqtt/mqtt_payload.cpp
    src/ui/vital_renderer.cpp
    src/models/vital_reading.cpp
    src/models/ui_model.cpp
    src/utils/logger.cpp
    src/utils/config.cpp
)

set(TEST_HEADERS
    src/mqtt/mqtt_client.h
    src/mqtt/mqtt_payload.h
    src/models/vital_reading.h
    src/models/ui_model.h
    src/utils/logger.h
    src/utils/config.h
    tests/test_fixtures.h
)

add_executable(test_mqtt_client ${TEST_SOURCES} ${TEST_HEADERS})

target_link_libraries(test_mqtt_client
    Qt6::Core
    Qt6::Gui
    ${MOSQUITTO_LIBRARIES}
)

target_include_directories(test_mqtt_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${MOSQUITTO_INCLUDE_DIRS}
)

# Register tests
add_test(NAME mqtt_client_test COMMAND test_mqtt_client)

# Install targets (optional)
install(TARGETS dashboard RUNTIME DESTINATION bin)
install(DIRECTORY qml/ DESTINATION share/medtech-clinician-ui/qml)

# Print configuration summary
message(STATUS "")
message(STATUS "========== MedTech Clinician UI ==========")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "MQTT Library: ${MOSQUITTO_LIBRARIES}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "==========================================")
message(STATUS "")