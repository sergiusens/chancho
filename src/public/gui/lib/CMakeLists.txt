set(TARGET chancho-app-priv)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/public/gui/lib)

set(HEADERS
    com/chancho/chancho_app.h
    com/chancho/qml/account.h
    com/chancho/qml/book.h
    com/chancho/qml/category.h
    com/chancho/qml/models/accounts.h
    com/chancho/qml/models/categories.h
    com/chancho/qml/models/day.h
    com/chancho/qml/models/month.h
    com/chancho/qml/transaction.h
)

set(SOURCES
    com/chancho/chancho_app.cpp
    com/chancho/qml/account.cpp
    com/chancho/qml/book.cpp
    com/chancho/qml/category.cpp
    com/chancho/qml/models/accounts.cpp
    com/chancho/qml/models/categories.cpp
    com/chancho/qml/models/day.cpp
    com/chancho/qml/models/month.cpp
    com/chancho/qml/transaction.cpp
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
