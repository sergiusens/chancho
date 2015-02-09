set(TARGET chancho-priv)

set(SOURCES
    com/chancho/account.cpp
    com/chancho/book.cpp
    com/chancho/category.cpp
    com/chancho/transaction.cpp
    com/chancho/system/database_factory.cpp
)

set(HEADERS
    com/chancho/account.h
    com/chancho/book.h
    com/chancho/category.h
    com/chancho/static_init.h
    com/chancho/transaction.h
    com/chancho/system/database.h
    com/chancho/system/database_factory.h
    com/chancho/system/query.h
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

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
    ${Boost_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
)
