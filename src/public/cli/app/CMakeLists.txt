set(TARGET chancho-cli)

set(SOURCES
    main.cpp
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${TARGET}
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries(${TARGET}
        ${Boost_LIBRARIES}
        ${Qt5Core_LIBRARIES}
)
