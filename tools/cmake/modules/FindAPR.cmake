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

if (APR_FOUND)
  message(STATUS "Found Apr: ${APR_INCLUDE_DIR}")
  return()
endif()

if (NOT DEFINED APR_USE_SYSTEM_PATH)
  set(APR_USE_SYSTEM_PATH 1)
endif()

set (Apr_FOUND 1)

###########################
# Looking for Apr headers
###########################

find_path(APR_INCLUDE
  NAMES
    apr_version.h
  PATHS
    ${3RD_PARTY_INSTALL_PREFIX}
  PATH_SUFFIXES
    include
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)
if (APR_USE_SYSTEM_PATH)
  find_path(APR_INCLUDE
    NAMES
      apr_version.h
    PATH_SUFFIXES
      include
  )
endif()

###########################

###########################
# Looking for Apr libs
###########################

find_library(APR_LIBRARY
  NAMES
    apr-1
  PATHS
    ${3RD_PARTY_INSTALL_PREFIX_ARCH}
  PATH_SUFFIXES
    lib
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)
if (APR_USE_SYSTEM_PATH)
  find_library(APR_LIBRARY
    NAMES
      apr-1
    PATH_SUFFIXES
      lib
  )
endif()

###########################

###########################
# Check components
###########################

if (APR_INCLUDE AND APR_LIBRARY)
  get_filename_component(APR_LIBRARY_DIR ${APR_LIBRARY} DIRECTORY)

  foreach(v MAJOR MINOR PATCH)
    file(STRINGS "${APR_INCLUDE}/apr_version.h" _apr_VERSION_H_CONTANT_${v} REGEX "#define APR_${v}_VERSION ")
    string(REGEX REPLACE "[A-z# ]" "" _apr_${v}_VERSION ${_apr_VERSION_H_CONTANT_${v}})
  endforeach()

  set(Apr_VERSION "${_apr_MAJOR_VERSION}.${_apr_MINOR_VERSION}.${_apr_PATCH_VERSION}")
  unset(_apr_MAJOR_VERSION)
  unset(_apr_MINOR_VERSION)
  unset(_apr_PATCH_VERSION)

  if (Apr_VERSION STREQUAL "")
    message(WARNING "Apr version could not be defined properly from ${APR_INCLUDE}/apr_version.h")
    set(Apr_FOUND 0)
  endif()

  if (Apr_VERSION VERSION_LESS APR_FIND_VERSION)
    message(WARNING "Detected version of Apr is too old (${Apr_VERSION}). Requested version was ${APR_FIND_VERSION}")
    set(Apr_FOUND 0)
  endif()

else()
  message(STATUS "Failed to find Apr components.")
  set(Apr_FOUND 0)
endif()

if (Apr_FOUND)
  set(APR_FOUND ${Apr_FOUND} CACHE INTERNAL "Apr found" FORCE)
  set(APR_INCLUDE_DIR ${APR_INCLUDE} CACHE INTERNAL "Apr include path" FORCE)
  set(APR_LIBS_DIRECTORY ${APR_LIBRARY_DIR} CACHE INTERNAL "Apr library path" FORCE)
  set(APR_VERSION ${Apr_VERSION} CACHE INTERNAL "Apr version" FORCE)

  message(STATUS "Found Apr: ${APR_INCLUDE_DIR} (found version \"${APR_VERSION}\")")
endif()

mark_as_advanced(APR_INCLUDE APR_LIBRARY Apr_VERSION Apr_FOUND)
