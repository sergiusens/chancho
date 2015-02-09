set(PRIVATE_TESTS
    test_account
    test_book
    test_book_account
    test_book_category
    test_book_mocked
    test_category
    test_transaction
)

foreach(test ${PRIVATE_TESTS})
    # set the sources per test
    set(${test}_PRIVATE_SOURCES
            ${test}.cpp
    )

    set(${test}_PRIVATE_HEADERS
            ${test}.h
    )

endforeach(test)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${Qt5Test_INCLUDE_DIRS})
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/priv)
include_directories(${CMAKE_SOURCE_DIR}/tests/common)

link_directories(${NIH_DBUS_LIBDIR})
link_directories(${GLOG_DBUS_LIBDIR})

set(PRIVATE_TESTS_LIBS
    ${GLOG_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    ${Qt5Test_LIBRARIES}
    chancho-priv
    chancho-test-lib
)

foreach(test ${PRIVATE_TESTS})
    # set targets, linked libs and test
    add_executable(${test}
        ${${test}_PRIVATE_SOURCES}
        ${${test}_PRIVATE_HEADERS}
    )

    target_link_libraries(${test}
        ${PRIVATE_TESTS_LIBS}
    )

    ADD_TEST(NAME ${test} COMMAND dbus-test-runner -m 180 --task=${CMAKE_CURRENT_BINARY_DIR}/${test})

endforeach(test)
