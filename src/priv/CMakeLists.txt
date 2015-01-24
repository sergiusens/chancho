set(TARGET chancho-priv)

set(SOURCES
    com/chancho/category.cpp
)

set(HEADERS
    com/chancho/category.h
)

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
)
