# Copyright (c) 2014, Ford Motor Company
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

set(LOG4CXX_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/apache-log4cxx-0.10.0)
set(LOG4CXX_USE_SYSTEM_PATH 0)
find_package(Log4cxx)

if ((LOG4CXX_FOUND OR NO_REBUILD_3RD_PARTY_LOGGER) AND
    NOT FORCE_3RD_PARTY_LOGGER)
  add_custom_target(liblog4cxx)
else()
  message(STATUS "Did not find Log4CXX. Installing Log4CXX")

  # Since logger is not found, apr and apr-util has to be rebuild as parts of logger
  set(NO_REBUILD_3RD_PARTY_LOGGER OFF PARENT_SCOPE)
  set(FORCE_3RD_PARTY_LOGGER ON PARENT_SCOPE)

  set(LOG4CXX_BUILD_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/apache-log4cxx-build)
  file(MAKE_DIRECTORY
    ${LOG4CXX_BUILD_DIRECTORY}
  )

  set(APR_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/../apr/apr-build)
  set(APR_UTIL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/../apr-util/apr-util-build)

  set(LOG4CXX_SOURCE_INCLUDE_DIRECTORY ${LOG4CXX_SOURCE_DIRECTORY}/src/main/include)
  set(LOG4CXX_BUILD_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}/src/main/include)

  set(LOG4CXX_INCLUDE_DIRECTORY 
    ${LOG4CXX_SOURCE_INCLUDE_DIRECTORY} ${LOG4CXX_BUILD_INCLUDE_DIRECTORY} CACHE INTERNAL "Installation path of Log4CXX headers" FORCE)
  set(LOG4CXX_LIBS_DIRECTORY 
    ${LOG4CXX_BUILD_DIRECTORY}/src/main/cpp/.libs  CACHE INTERNAL "Installation path of Log4CXX libraries" FORCE)

  include(${TARGET_PLATFORM})

  add_custom_target(liblog4cxx ALL make
    DEPENDS ${LOG4CXX_BUILD_DIRECTORY}/Makefile
    WORKING_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}
  )

  set(APPEND_COMMIT_HASH_CMD
    "bash ${CMAKE_CURRENT_SOURCE_DIR}/helpers.sh "
    "AppendCommitHash "
    "${LOG4CXX_SOURCE_DIRECTORY} "
    "${LOG4CXX_LIBS_DIRECTORY}"
  )
  string (REPLACE ";" " " APPEND_COMMIT_HASH_CMD "${APPEND_COMMIT_HASH_CMD}")
  add_custom_command(TARGET liblog4cxx POST_BUILD
    COMMAND /bin/bash -c ${APPEND_COMMIT_HASH_CMD}
  )

  install(
    DIRECTORY ${LOG4CXX_LIBS_DIRECTORY}/
    DESTINATION ${3RD_PARTY_INSTALL_PREFIX_ARCH}/lib
    USE_SOURCE_PERMISSIONS
    FILES_MATCHING PATTERN liblog4cxx.so*
  )

  install(
    DIRECTORY ${LOG4CXX_SOURCE_INCLUDE_DIRECTORY}/
    DESTINATION ${3RD_PARTY_INSTALL_PREFIX}/include
    USE_SOURCE_PERMISSIONS
    FILES_MATCHING PATTERN *.h
  )

  install(
    DIRECTORY ${LOG4CXX_BUILD_INCLUDE_DIRECTORY}/
    DESTINATION ${3RD_PARTY_INSTALL_PREFIX}/include
    USE_SOURCE_PERMISSIONS
    FILES_MATCHING PATTERN *.h
  )
endif()
