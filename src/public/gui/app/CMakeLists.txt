project(chancho.mandel)

set(TARGET chancho)

# set the diff values so that we can build them automatically
set(APP_NAME ${TARGET})
set(DESKTOP_FILE "${TARGET}.desktop")
set(ICON ${TARGET}.png)
set(APP_HARDCODE ${TARGET})

# configure the project one way or another if we are going to be building for a click package or a
# normal debian pacakge application.
if(CLICK_MODE)
    if(NOT DEFINED GIT_SOURCE)
        set(GIT_SOURCE "git@github.com:mandel-macaque/chancho.git")
        message("-- Setting GIT_SOURCE to ${GIT_SOURCE}")
    endif(NOT DEFINED GIT_SOURCE)

    #set the installation paths to be a click dir which can later be used to build the click package
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/click)
    set(CMAKE_INSTALL_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/click)
    set(CMAKE_INSTALL_LOCALEDIR ${CMAKE_CURRENT_BINARY_DIR}/click/share/locale)
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

message("-- Setting I18N_DIRECTORY to ${CMAKE_INSTALL_PREFIX}/share/locale")

# copy the blob to the install dir
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_HARDCODE} DESTINATION ${CMAKE_INSTALL_BINDIR})

# copy the .desktop file to the desktop dir
configure_file(${DESKTOP_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE} DESTINATION ${DESKTOP_DIR})

set(TRANSLATABLE_SRC
    components/dialogs/*.qml
    components/dialogs/js/*.js
    components/js/*.js
    components/models/*.qml
    components/*.qml
    *.qml)

file(GLOB_RECURSE I18N_SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/po
     ${TRANSLATABLE_SRC})

list(SORT I18N_SRC_FILES)

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
    qml/components/images/chancho_logo.png
    qml/components/images/chancho_wordmark.png
    qml/MainWindow.qml
    qml/components/accounts/Account.qml
    qml/components/accounts/Edit.qml
    qml/components/accounts/Form.qml
    qml/components/accounts/New.qml
    qml/components/accounts/Page.qml
    qml/components/accounts/js/accounts.js
    qml/components/categories/Category.qml
    qml/components/categories/Edit.qml
    qml/components/categories/Form.qml
    qml/components/categories/New.qml
    qml/components/categories/Page.qml
    qml/components/categories/StatsPage.qml
    qml/components/categories/js/categories.js
    qml/components/recurrent_transactions/CategoryHeader.qml
    qml/components/recurrent_transactions/GeneratedTransactionsPage.qml
    qml/components/recurrent_transactions/RecurrentTransaction.qml
    qml/components/recurrent_transactions/Page.qml
    qml/components/recurrent_transactions/RecurrentTransactionsPerCategory.qml
    qml/components/transactions/BillingPerDay.qml
    qml/components/transactions/DateHeader.qml
    qml/components/transactions/Edit.qml
    qml/components/transactions/Form.qml
    qml/components/transactions/New.qml
    qml/components/transactions/Page.qml
    qml/components/transactions/Transaction.qml
    qml/components/wizard/WelcomeWizard.qml
    qml/components/wizard/AccountsPage.qml
    qml/components/wizard/CategoriesPage.qml
    qml/components/wizard/LastPage.qml
    qml/components/wizard/js/accounts_wizard.js
    qml/components/wizard/js/categories_wizard.js
    qml/components/wizard/js/last_page_wizard.js
    qml/components/ColorChooser.qml
    qml/components/PageWithBottomEdge.qml
    qml/components/SplashScreen.qml
    qml/components/Walkthrough.qml
    qml/components/dialogs/ConfirmationDialog.qml
    qml/components/dialogs/ErrorDialog.qml
    qml/components/dialogs/RecurrenceDialog.qml
    qml/components/dialogs/WizardNewAccountDialog.qml
    qml/components/dialogs/WizardNewCategoryDialog.qml
    qml/components/dialogs/js/accounts_wizard.js
    qml/components/dialogs/js/categories_wizard.js
    qml/components/jbQuick/Chart/qmldir
    qml/components/jbQuick/Chart/QChart.js
    qml/components/jbQuick/Chart/QChart.qml
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

add_subdirectory(po)
