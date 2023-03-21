cmake_minimum_required(VERSION 3.8)

set(PROJECTNAME OneNETDaemon)
project(${PROJECTNAME} C CXX ASM)
#添加库,添加源代码
file(GLOB NETLIB_HEADER_FILES *.h)
file(GLOB NETLIB_SOURCES_FILES *.c *.cpp)
add_executable(${PROJECTNAME}   ${NETLIB_SOURCES_FILES} ${NETLIB_HEADER_FILES} )
#设置C++标准
set_property(TARGET ${PROJECTNAME}   PROPERTY CXX_STANDARD 20)

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

include(FindPkgConfig)

#netlib
find_package( appnetlib REQUIRED)
target_link_libraries(${PROJECTNAME}   appnetlib)

#pthread库
find_package( Threads REQUIRED)
target_link_libraries(${PROJECTNAME}   ${CMAKE_THREAD_LIBS_INIT} )

#libmosquitto库（MQTT客户端）
pkg_check_modules(LIBMOSQUITTO REQUIRED IMPORTED_TARGET libmosquitto)
target_link_libraries( ${PROJECTNAME}  PkgConfig::LIBMOSQUITTO)

#argtable3
find_package(Argtable3 REQUIRED)
target_link_libraries( ${PROJECTNAME}  argtable3::argtable3)

#mbedtls
#鉴于大多数嵌入式中加密库均是由mbedtls实现,因此采用mbedtls作为加密库
include(CheckIncludeFile)
set(HAVE_MBEDTLS TRUE)
check_include_file("mbedtls/base64.h" HAVE_MBEDTLS_BASE64)
if(NOT HAVE_MBEDTLS_BASE64)
set(HAVE_MBEDTLS FALSE)
endif()
check_include_file("mbedtls/md.h" HAVE_MBEDTLS_MD)
if(NOT HAVE_MBEDTLS_MD)
set(HAVE_MBEDTLS FALSE)
endif()
if(HAVE_MBEDTLS)
target_link_libraries(${PROJECTNAME}  mbedcrypto)
else()
message(FATAL_ERROR "mbedtls is not found!")
endif()


include(GNUInstallDirs)
install(TARGETS ${PROJECTNAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


