

set(HEADERS
        "${INCLUDE_DIR}/Version.hpp"
        "${INCLUDE_DIR}/Time.hpp"
        "${INCLUDE_DIR}/ThreadPool.hpp"
        "${INCLUDE_DIR}/Logger.hpp"
        "${INCLUDE_DIR}/tscl.hpp"
        )

add_library(tscl STATIC
        Logger.cpp
        ThreadPool.cpp
        Time.cpp
        Version.cpp
        ${HEADERS}
        )

target_include_directories(tscl
        PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include/tscl>
        )

target_link_libraries(tscl
        PUBLIC
        Threads::Threads
        )

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tscl)