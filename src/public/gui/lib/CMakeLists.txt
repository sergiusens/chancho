set(TARGET chancho-app-priv)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/priv)

set(HEADERS
    com/chancho/book_model.h
    com/chancho/chancho_app.h
    com/chancho/day_model.h
    com/chancho/month_model.h
    com/chancho/transaction_model.h
)

set(SOURCES
    com/chancho/book_model.cpp
    com/chancho/chancho_app.cpp
    com/chancho/day_model.cpp
    com/chancho/month_model.cpp
    com/chancho/transaction_model.cpp
)


add_library(${TARGET} STATIC
    ${HEADERS}
    ${SOURCES}
)

set_target_properties(
    ${TARGET}

    PROPERTIES
    VERSION ${CHANCHO_VERSION_MAJOR}.${CHANCHO_VERSION_MINOR}.${CHANCHO_VERSION_PATCH}
    SOVERSION ${CHANCHO_VERSION_MAJOR}
)

target_link_libraries(${TARGET}
    ${GLOG_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    chancho-priv
)
