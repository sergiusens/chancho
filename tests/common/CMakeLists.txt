set(TARGET chancho-test-lib)

set(SOURCES
        base_testcase.cpp
)

set(HEADERS
        base_testcase.h
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/priv)

link_directories(${GLOG_DBUS_LIBDIR})

add_library(${TARGET} STATIC
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${TARGET}
    ${GLOG_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Test_LIBRARIES}
    ${GMOCK_LIBRARY}
    ${GTEST_BOTH_LIBRARIES}
    chancho-priv
)
