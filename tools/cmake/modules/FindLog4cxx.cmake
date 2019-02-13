# Copyright (c) 2019, Ford Motor Company
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

if (LOG4CXX_FOUND)
  message(STATUS "Found Log4cxx: ${LOG4CXX_INCLUDE_DIR}")
  return()
endif()

if (NOT DEFINED LOG4CXX_USE_SYSTEM_PATH)
  set(LOG4CXX_USE_SYSTEM_PATH 1)
endif()

set (Log4cxx_FOUND 1)

###########################
# Looking for log4cxx headers
###########################

find_path(LOG4CXX_INCLUDE
  NAMES
    log4cxx/log4cxx.h
  PATHS
    ${3RD_PARTY_INSTALL_PREFIX}
  PATH_SUFFIXES
    include
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)
if (LOG4CXX_USE_SYSTEM_PATH)
  find_path(LOG4CXX_INCLUDE
    NAMES
      log4cxx.h
    PATH_SUFFIXES
      include
  )
endif()

###########################

###########################
# Looking for log4cxx libs
###########################

find_library(LOG4CXX_LIBRARY
  NAMES
    log4cxx
  PATHS
    ${3RD_PARTY_INSTALL_PREFIX_ARCH}
  PATH_SUFFIXES
    lib
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)
if (LOG4CXX_USE_SYSTEM_PATH)
  find_library(LOG4CXX_LIBRARY
    NAMES
      log4cxx
    PATH_SUFFIXES
      lib
  )
endif()

###########################

###########################
# Check components
###########################

if (LOG4CXX_INCLUDE AND LOG4CXX_LIBRARY)
  get_filename_component(LOG4CXX_LIBRARY_DIR ${LOG4CXX_LIBRARY} DIRECTORY)
  set(CHECK_VERSION_CMD
    "bash ${CMAKE_CURRENT_SOURCE_DIR}/helpers.sh "
    "IsActualVersion ${LOG4CXX_SOURCE_DIRECTORY} ${LOG4CXX_LIBRARY_DIR}")
  string (REPLACE ";" " " CHECK_VERSION_CMD "${CHECK_VERSION_CMD}")
  execute_process(
    COMMAND /bin/bash -c ${CHECK_VERSION_CMD}
    RESULT_VARIABLE IS_LOGGER_ACTUAL
  )
  if (NOT ${IS_LOGGER_ACTUAL})
    message(WARNING "Detected version of Log4CXX is too old.")
    set(Log4cxx_FOUND 0)
  endif()
else()
  message(STATUS "Failed to find Log4cxx components.")
  set(Log4cxx_FOUND 0)
endif()

if (Log4cxx_FOUND)
  set(LOG4CXX_FOUND ${Log4cxx_FOUND} CACHE INTERNAL "Log4cxx found" FORCE)
  set(LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_INCLUDE} CACHE INTERNAL "Installation path of Log4CXX headers" FORCE)
  set(LOG4CXX_LIBS_DIRECTORY ${LOG4CXX_LIBRARY_DIR} CACHE INTERNAL "Installation path of Log4CXX libraries" FORCE)

  message(STATUS "Found Log4cxx: ${LOG4CXX_INCLUDE_DIRECTORY}")
endif()

mark_as_advanced(LOG4CXX_INCLUDE LOG4CXX_LIBRARY Log4cxx_FOUND)
