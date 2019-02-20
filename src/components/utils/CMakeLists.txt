# Copyright (c) 2016, Ford Motor Company
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following
# disclaimer in the documentation and/or other materials provided with the
# distribution.
#
# Neither the name of the Ford Motor Company nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

include(${CMAKE_SOURCE_DIR}/tools/cmake/helpers/platform.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/helpers/sources.cmake)

# dbms
set(DBMS_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/sqlite_wrapper
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite_wrapper
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/qdb_wrapper
  ${CMAKE_CURRENT_SOURCE_DIR}/src/qdb_wrapper
)

set(EXCLUDE_PATHS
  thread_manager.cc
  pulse_thread_delegate.cc
  ${DBMS_PATHS}
)

set(LIBRARIES ConfigProfile)

if(NOT BUILD_BACKTRACE_SUPPORT)
  list(APPEND EXCLUDE_PATHS
    back_trace.cc
  )
endif()

if(ENABLE_LOG)
  list(APPEND LIBRARIES
    log4cxx -L${LOG4CXX_LIBS_DIRECTORY}
    apr-1 -L${APR_LIBS_DIRECTORY}
    aprutil-1 -L${APR_UTIL_LIBS_DIRECTORY}
  )
else()
  list(APPEND EXCLUDE_PATHS
    push_log.cc
    log_message_loop_thread.cc
    logger_status.cc
    auto_trace.cc
    logger.cc
  )
endif()

if(NOT BUILD_BT_SUPPORT)
  list(APPEND EXCLUDE_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/bluetooth
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bluetooth
  )
endif()

set(PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

collect_sources(SOURCES "${PATHS}" "${EXCLUDE_PATHS}")
list(APPEND SOURCES
  "${CMAKE_CURRENT_LIST_DIR}/src/threads/thread_posix.cc"
  "${CMAKE_CURRENT_LIST_DIR}/src/signals_posix.cc"
  "${CMAKE_CURRENT_LIST_DIR}/src/rwlock_posix.cc")

if (CMAKE_SYSTEM_NAME STREQUAL "QNX")
  list(REMOVE_ITEM EXCLUDE_PATHS
    pulse_thread_delegate.cc)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
    # --- QDB Wrapper
    add_subdirectory(./src/qdb_wrapper)
else()
    # --- SQLite Wrapper
    add_subdirectory(./src/sqlite_wrapper)
endif()

if(UNIX AND NOT QNXNTO)
  list(APPEND LIBRARIES pthread dl rt)
endif()

add_library("Utils" ${SOURCES})
add_dependencies("Utils" liblog4cxx)
add_dependencies("Utils" Boost)

list(APPEND LIBRARIES boost_system -L${BOOST_LIBS_DIRECTORY})
list(APPEND LIBRARIES boost_thread -L${BOOST_LIBS_DIRECTORY})
list(APPEND LIBRARIES boost_date_time -L${BOOST_LIBS_DIRECTORY})
list(APPEND LIBRARIES boost_filesystem -L${BOOST_LIBS_DIRECTORY})

target_link_libraries("Utils" ${LIBRARIES})
target_include_directories ("Utils" PUBLIC
  ${COMPONENTS_DIR}/utils/include
  ${COMPONENTS_DIR}/config_profile/include
  ${COMPONENTS_DIR}/media_manager/include
  ${COMPONENTS_DIR}/protocol_handler/include
  ${JSONCPP_INCLUDE_DIRECTORY}
  ${LOG4CXX_INCLUDE_DIRECTORY}
  ${BOOST_INCLUDE_DIRECTORY}
  ${APR_INCLUDE_DIRECTORY}
  ${CMAKE_SOURCE_DIR}/src
)

if(BUILD_TESTS)
  add_subdirectory(test)
endif()

install(TARGETS "Utils"
  DESTINATION bin
  PERMISSIONS
    OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
)
