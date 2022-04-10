cmake_minimum_required(VERSION 3.16)


project(tscl VERSION 0.1.1.1
        DESCRIPTION "Thukisdo's software conception library. Intended for personnal use"
        LANGUAGES CXX
        )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckCXXCompilerFlag)

# Messy way to setup extra error messages, but i don't want to spend time on it
CHECK_CXX_COMPILER_FLAG("-Wall -Wpedantic -Wextra -Wno-sign-compare" COMPILER_SUPPORT_WALL_WERROR_WPEDANTIC_WEXTRA)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wno-sign-compare")

find_package(Threads REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include/tscl)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)

write_basic_package_version_file(
        tsclConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(TARGETS tscl
        EXPORT tsclTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tscl
        )

install(EXPORT tsclTargets FILE tsclTargets.cmake NAMESPACE tscl:: DESTINATION lib/cmake/tscl)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/tsclConfigVersion.cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tsclConfig.cmake
        DESTINATION lib/cmake/tscl
        )