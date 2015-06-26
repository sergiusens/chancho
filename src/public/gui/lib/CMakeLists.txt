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
    com/chancho/qml/recurrent_transaction.h
    com/chancho/qml/transaction.h
    com/chancho/qml/models/accounts.h
    com/chancho/qml/models/categories.h
    com/chancho/qml/models/day.h
    com/chancho/qml/models/month.h
    com/chancho/qml/workers/accounts.h
    com/chancho/qml/workers/categories.h
    com/chancho/qml/workers/transactions.h
    com/chancho/qml/workers/worker.h
    com/chancho/qml/workers/worker_thread.h
    com/chancho/qml/workers/accounts/multi_store.h
    com/chancho/qml/workers/accounts/single_remove.h
    com/chancho/qml/workers/accounts/single_store.h
    com/chancho/qml/workers/accounts/single_update.h
    com/chancho/qml/workers/categories/multi_store.h
    com/chancho/qml/workers/categories/single_remove.h
    com/chancho/qml/workers/categories/single_store.h
    com/chancho/qml/workers/categories/single_update.h
    com/chancho/qml/workers/transactions/generate_recurrent.h
    com/chancho/qml/workers/transactions/single_remove.h
    com/chancho/qml/workers/transactions/single_store.h
    com/chancho/qml/workers/transactions/single_update.h
)

set(SOURCES
    com/chancho/chancho_app.cpp
    com/chancho/qml/account.cpp
    com/chancho/qml/book.cpp
    com/chancho/qml/category.cpp
    com/chancho/qml/transaction.cpp
    com/chancho/qml/recurrent_transaction.cpp
    com/chancho/qml/models/accounts.cpp
    com/chancho/qml/models/categories.cpp
    com/chancho/qml/models/day.cpp
    com/chancho/qml/models/month.cpp
    com/chancho/qml/workers/accounts.cpp
    com/chancho/qml/workers/categories.cpp
    com/chancho/qml/workers/transactions.cpp
    com/chancho/qml/workers/accounts/multi_store.cpp
    com/chancho/qml/workers/accounts/single_remove.cpp
    com/chancho/qml/workers/accounts/single_store.cpp
    com/chancho/qml/workers/accounts/single_update.cpp
    com/chancho/qml/workers/categories/multi_store.cpp
    com/chancho/qml/workers/categories/single_remove.cpp
    com/chancho/qml/workers/categories/single_store.cpp
    com/chancho/qml/workers/categories/single_update.cpp
    com/chancho/qml/workers/transactions/generate_recurrent.cpp
    com/chancho/qml/workers/transactions/single_remove.cpp
    com/chancho/qml/workers/transactions/single_store.cpp
    com/chancho/qml/workers/transactions/single_update.cpp
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
