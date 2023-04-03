cmake_minimum_required(VERSION 3.20)

set(PROJECTNAME appubus)
project(${PROJECTNAME} C CXX ASM)
#添加库,添加源代码
file(GLOB APPUBUS_HEADER_FILES *.h)
file(GLOB APPUBUS_SOURCES_FILES *.c *.cpp)
add_library(appubus SHARED  ${APPUBUS_SOURCES_FILES} ${APPUBUS_HEADER_FILES} )
#设置C++/C标准
set_property(TARGET appubus PROPERTY CXX_STANDARD 20)
set_property(TARGET appubus PROPERTY C_STANDARD 20)

#获取程序版本
find_package(Git)
if(Git_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD 
		  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		  OUTPUT_VARIABLE PROJECT_VERSION)
else()
  set(PROJECT_VERSION testing_version)
endif()
add_definitions(" -DVERSION=${PROJECT_VERSION} ")
if(DEFINED PACKAGE_VERSION)
add_definitions(" -DPACKAGE_VERSION=${PACKAGE_VERSION} ")
endif()
option(OPENWRT "build in openwrt" OFF)
if(OPENWRT)
add_definitions(" -DOPENWRT ")
endif()

#添加一些测试应用，用于测试库
file(GLOB TESTS  ${CMAKE_SOURCE_DIR}/tests/*)
foreach(TEST ${TESTS})
	if(EXISTS ${TEST})
                file(RELATIVE_PATH TESTEXE  ${CMAKE_SOURCE_DIR}/tests/  ${TEST})
                add_executable(${TESTEXE}.exe ${TEST})
                target_link_libraries(${TESTEXE}.exe  appubus)
                target_include_directories(${TESTEXE}.exe PUBLIC ${CMAKE_SOURCE_DIR})
        endif()
endforeach()


include(FindPkgConfig)
#libjson-c库
pkg_check_modules(Json-C REQUIRED  IMPORTED_TARGET json-c)
target_link_libraries(appubus  PkgConfig::Json-C)

#pthread库
find_package( Threads REQUIRED)
target_link_libraries(appubus  ${CMAKE_THREAD_LIBS_INIT} )

include(CheckIncludeFileCXX)

#libubus库
check_include_file_cxx("libubus.h" HAVE_LIBUBUS_H )
if(HAVE_LIBUBUS_H)
target_link_libraries(appubus ubus )
else()
message(FATAL_ERROR "libubus is not found!")
endif()

#libubox
check_include_file_cxx("libubox/blobmsg_json.h"  HAVE_LIBUBOX_BLOBMSG_JSON_H)
if(HAVE_LIBUBOX_BLOBMSG_JSON_H)
target_link_libraries(appubus ubox blobmsg_json)
else()
message(FATAL_ERROR "libbox is not found!")
endif()

include(GNUInstallDirs)
install(TARGETS appubus
  EXPORT  appubusConfig
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT appubusConfig
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/appubus
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/appubusConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
)

install(FILES "${PROJECT_BINARY_DIR}/appubusConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/appubus
)

install(FILES ${APPUBUS_HEADER_FILES}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

