set(TEST_SOURCES
    test_mqtt_client.cpp
    test_vital_renderer.cpp
)

set(TEST_HEADERS
    test_fixtures.h
)

# Include source files (to test)
list(APPEND TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/mqtt/mqtt_client.cpp
    ${CMAKE_SOURCE_DIR}/src/mqtt/mqtt_payload.cpp
    ${CMAKE_SOURCE_DIR}/src/models/vital_reading.cpp
    ${CMAKE_SOURCE_DIR}/src/models/ui_model.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/logger.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/config.cpp
)

add_executable(test_mqtt_client ${TEST_SOURCES} ${TEST_HEADERS})

target_link_libraries(test_mqtt_client
    Qt6::Core
    Qt6::Gui
    ${MOSQUITTO_LIBRARIES}
)

target_include_directories(test_mqtt_client PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
)

add_test(NAME mqtt_client_test COMMAND test_mqtt_client)