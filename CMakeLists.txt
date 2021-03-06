project(chancho)

cmake_minimum_required(VERSION 2.8)

option(CLICK_MODE "Installs to a contained location" off)
option(SANITIZERS "Add the sanitizer check to the code to find bugs in the application" off)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -pthread")

if(SANITIZERS)
    # add the address sanitazer which can be used to crash the application and get information
    # for wrong used variables.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O3 -fno-omit-frame-pointer")
	message(STATUS "Setting build type to use sanitizers.")
endif(SANITIZERS)


# Standard install paths
include(GNUInstallDirs)

include(EnableCoverageReport)
#####################################################################
# Enable code coverage calculation with gcov/gcovr/lcov
# Usage:
#  * Switch build type to coverage (use ccmake or cmake-gui)
#  * Invoke make, make test, make coverage
#  * Find html report in subdir coveragereport
#  * Find xml report feasible for jenkins in coverage.xml
#####################################################################
IF(CMAKE_BUILD_TYPE MATCHES [cC][oO][vV][eE][rR][aA][gG][eE])
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs" )
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ftest-coverage -fprofile-arcs" )
  SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -coverage" )
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -coverage" )
  ENABLE_COVERAGE_REPORT(TARGETS ${movie-tracker-priv} ${movie-tracker-cli})
ENDIF(CMAKE_BUILD_TYPE MATCHES [cC][oO][vV][eE][rR][aA][gG][eE])

execute_process(
    COMMAND qmake -query QT_INSTALL_QML
    OUTPUT_VARIABLE QT_INSTALL_QML
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CHANCHO_VERSION_MAJOR 0)
set(CHANCHO_VERSION_MINOR 2)
set(CHANCHO_VERSION_PATCH 6)

#required bin helpers
find_program(INTLTOOL_MERGE intltool-merge)
find_program(INTLTOOL_EXTRACT intltool-extract)

find_package(Gtest REQUIRED)
include_directories(${GMOCK_INCLUDE_DIR} ${GTEST_INCLUDE_DIR})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLOG REQUIRED libglog)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++11 -Werror -O2 -Wall -W -D_REENTRANT -fPIC -Wextra")
#add_definitions("-DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_lower)

if(cmake_build_type_lower MATCHES relwithdebinfo) # workaround for http://pub    lic.kitware.com/Bug/view.php?id=14696
	add_definitions(-DQT_NO_DEBUG)
endif()

add_subdirectory(src)
add_subdirectory(tests)
