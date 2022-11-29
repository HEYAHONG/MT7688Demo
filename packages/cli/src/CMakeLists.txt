cmake_minimum_required(VERSION 3.8)

set(PROJECTNAME cli)
project(${PROJECTNAME} C CXX ASM)
#添加库,添加源代码
file(GLOB NETLIB_HEADER_FILES *.h)
file(GLOB NETLIB_SOURCES_FILES *.c *.cpp)
add_executable(cli  ${NETLIB_SOURCES_FILES} ${NETLIB_HEADER_FILES} )
#设置C++标准
set_property(TARGET cli PROPERTY CXX_STANDARD 20)

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
target_link_libraries(cli  appnetlib)

#pthread库
find_package( Threads REQUIRED)
target_link_libraries(cli  ${CMAKE_THREAD_LIBS_INIT} )



include(GNUInstallDirs)
install(TARGETS cli
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


