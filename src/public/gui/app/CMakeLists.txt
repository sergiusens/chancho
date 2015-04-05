project(chancho-main-app)

set(TARGET chancho-app)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
include_directories(${Qt5Sql_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/priv)
include_directories(${CMAKE_SOURCE_DIR}/src/public/gui/lib)

set(HEADERS
)

set(SOURCES
    main.cpp
)

set(QML_SOURCES
    qml/components/js/accounts.js
    qml/components/js/categories.js
    qml/MainWindow.qml
    qml/components/AccountComponent.qml
    qml/components/AccountForm.qml
    qml/components/AccountsPage.qml
    qml/components/BillingPerDay.qml
    qml/components/CategoriesPage.qml
    qml/components/CategoryComponent.qml
    qml/components/CategoryForm.qml
    qml/components/CategoryStatsPage.qml
    qml/components/ColorChooser.qml
    qml/components/DateHeaderComponent.qml
    qml/components/DefaultColors.qml
    qml/components/EditAccount.qml
    qml/components/EditCategory.qml
    qml/components/EditTransaction.qml
    qml/components/MainPage.qml
    qml/components/NewAccount.qml
    qml/components/NewCategory.qml
    qml/components/NewTransaction.qml
    qml/components/PageWithBottomEdge.qml
    qml/components/TransactionComponent.qml
    qml/components/TransactionForm.qml
    qml/components/jbQuick/Chart/qmldir
    qml/components/jbQuick/Chart/QChart.js
    qml/components/jbQuick/Chart/QChart.qml
)

qt5_add_resources(chancho-app-resources ui.qrc)

add_executable(${TARGET}
    ${HEADERS}
    ${SOURCES}
    ${QML_SOURCES}
    ${chancho-app-resources}
)

add_dependencies(${TARGET} chancho-app-priv)

qt5_use_modules(${TARGET} Gui Core Qml Quick DBus Sql)

target_link_libraries(${TARGET}
    ${GLOG_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    ${Qt5Sql_LIBRARIES}
    chancho-priv
    chancho-app-priv
)

install(TARGETS ${CHANCHO_APP_BIN}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

