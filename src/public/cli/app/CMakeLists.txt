set(TARGET chancho-cli)

set(HEADERS
)

set(SOURCES
    main.cpp
)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/priv)

add_executable(${TARGET}
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${TARGET}
    ${Boost_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    chancho-priv
)
