project(chancho.mandel)

set(TARGET chancho)

set(APP_NAME ${TARGET})
set(DESKTOP_FILE "${TARGET}.desktop")
set(ICON ${TARGET}.png)
set(APP_HARDCODE ${TARGET})

if(CLICK_MODE)
    if(NOT DEFINED GIT_SOURCE)
        set(GIT_SOURCE "git@github.com:mandel-macaque/chancho.git")
        message("-- Setting GIT_SOURCE to ${GIT_SOURCE}")
    endif(NOT DEFINED GIT_SOURCE)

    #set the installation paths to be a click dir
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/click)
    set(CMAKE_INSTALL_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/click)
    set(DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/click)
    set(EXEC ${APP_HARDCODE})
    set(DESKTOP_DIR ${DATA_DIR})


else(CLICK_MODE)
    set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/${APP_HARDCODE})
    set(EXEC ${APP_HARDCODE})
    set(ICON ${CMAKE_INSTALL_PREFIX}/${DATA_DIR}/${ICON})
        message("-- Setting ICON to ${ICON}")
    set(DESKTOP_DIR ${CMAKE_INSTALL_DATADIR}/applications)
endif(CLICK_MODE)

# copy the blob to the install dir
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_HARDCODE} DESTINATION ${CMAKE_INSTALL_BINDIR})

# copy the .desktop file to the desktop dir
configure_file(${DESKTOP_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE} DESTINATION ${DESKTOP_DIR})

# copy all the other resources to the data dir
file(GLOB SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.png)
install(FILES ${SRC_FILES}  DESTINATION ${DATA_DIR})

#installation steps for click node
if(CLICK_MODE)
    # set the required files for the click creation
    configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json ${CMAKE_CURRENT_SOURCE_DIR}/apparmor.json
            DESTINATION ${CMAKE_INSTALL_PREFIX})

endif(CLICK_MODE)

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
    qml/components/Walkthrough.qml
    qml/components/WelcomeWizard.qml
    qml/components/WizardAccountsPage.qml
    qml/components/WizardCategoriesPage.qml
    qml/components/WizardLastPage.qml
    qml/components/dialogs/ConfirmationDialog.qml
    qml/components/dialogs/ErrorDialog.qml
    qml/components/dialogs/WizardNewAccountDialog.qml
    qml/components/dialogs/WizardNewCategoryDialog.qml
    qml/components/dialogs/js/accounts_wizard.js
    qml/components/dialogs/js/categories_wizard.js
    qml/components/jbQuick/Chart/qmldir
    qml/components/jbQuick/Chart/QChart.js
    qml/components/jbQuick/Chart/QChart.qml
    qml/components/js/accounts.js
    qml/components/js/accounts_wizard.js
    qml/components/js/categories.js
    qml/components/js/categories_wizard.js
    qml/components/js/last_page_wizard.js
    qml/components/models/CategoryTypeModel.qml
    qml/components/models/DefaultColors.qml
    qml/components/models/DefaultExpenseModel.qml
    qml/components/models/DefaultIncomeModel.qml
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
