cmake_minimum_required(VERSION 3.8)

set(PROJECTNAME netlib)
project(${PROJECTNAME} C CXX ASM)
#添加库,添加源代码
file(GLOB NETLIB_HEADER_FILES *.h)
file(GLOB NETLIB_SOURCES_FILES *.c *.cpp)
add_library(appnetlib SHARED  ${NETLIB_SOURCES_FILES} ${NETLIB_HEADER_FILES} )
#设置C++标准
set_property(TARGET appnetlib PROPERTY CXX_STANDARD 20)

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
		target_link_libraries(${TESTEXE}.exe  appnetlib)
		target_include_directories(${TESTEXE}.exe PUBLIC ${CMAKE_SOURCE_DIR})
	endif()
endforeach()


include(FindPkgConfig)
#pthread库
find_package( Threads REQUIRED)
target_link_libraries(appnetlib  ${CMAKE_THREAD_LIBS_INIT} )



include(GNUInstallDirs)
install(TARGETS appnetlib
  EXPORT  appnetlibConfig
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT appnetlibConfig
   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/appnetlib 
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/appnetlibConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion
)

install(FILES "${PROJECT_BINARY_DIR}/appnetlibConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/appnetlib
)

install(FILES ${NETLIB_HEADER_FILES}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

