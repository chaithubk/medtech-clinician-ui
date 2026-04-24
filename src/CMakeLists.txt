set(SOURCES
    mqtt/mqtt_client.cpp
    mqtt/mqtt_payload.cpp
    ui/dashboard_window.cpp
    ui/vital_renderer.cpp
    models/vital_reading.cpp
    models/ui_model.cpp
    utils/logger.cpp
    utils/config.cpp
    main.cpp
)

set(HEADERS
    mqtt/mqtt_client.h
    mqtt/mqtt_payload.h
    ui/dashboard_window.h
    ui/vital_renderer.h
    models/vital_reading.h
    models/ui_model.h
    utils/logger.h
    utils/config.h
)

add_library(medtech_lib ${SOURCES} ${HEADERS})
target_link_libraries(medtech_lib
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    ${MOSQUITTO_LIBRARIES}
)

# Expose to parent
set(MEDTECH_SOURCES ${SOURCES} PARENT_SCOPE)
set(MEDTECH_HEADERS ${HEADERS} PARENT_SCOPE)