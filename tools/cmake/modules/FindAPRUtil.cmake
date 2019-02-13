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

if (APR_UTIL_FOUND)
  message(STATUS "Found Apr-Util: ${APR_UTIL_INCLUDE_DIRECTORY}")
  return()
endif()

if (NOT DEFINED APR_UTIL_USE_SYSTEM_PATH)
  set(APR_UTIL_USE_SYSTEM_PATH 1)
endif()

set (Apr_Util_FOUND 1)

###########################
if (APR_UTIL_FIND_VERSION)
  string(REGEX MATCHALL "([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ ${APR_UTIL_FIND_VERSION})
  set(_apu_find_ver_major ${CMAKE_MATCH_1})
  set(_apu_find_ver_minor ${CMAKE_MATCH_2})
  set(_apu_find_ver_patch ${CMAKE_MATCH_3})
  if (TARGET_PLATFORM STREQUAL "QNX7")
    set(_apr_util_lib_suffix "-${_apu_find_ver_major}.so.${_apu_find_ver_minor}")
  else()
    set(_apr_util_lib_suffix "-${_apu_find_ver_major}.so.0.${_apu_find_ver_minor}.${_apu_find_ver_patch}")
  endif()
else()
  set(_apr_util_lib_suffix "-1")
endif()

###########################
# Looking for Apr Util libs
###########################

find_library(APR_UTIL_LIBRARY
  NAMES
    aprutil${_apr_util_lib_suffix}
    libaprutil${_apr_util_lib_suffix}
  PATHS
    ${3RD_PARTY_INSTALL_PREFIX_ARCH}
  PATH_SUFFIXES
    lib
  NO_DEFAULT_PATH
  NO_CMAKE_FIND_ROOT_PATH
)
if (APR_UTIL_USE_SYSTEM_PATH)
  find_library(APR_UTIL_LIBRARY
    NAMES
      aprutil${_apr_util_lib_suffix}
      libaprutil${_apr_util_lib_suffix}
    PATH_SUFFIXES
      lib
  )
endif()

###########################

###########################
# Check components
###########################

if (APR_UTIL_LIBRARY)
  get_filename_component(APR_UTIL_LIBRARY_DIR ${APR_UTIL_LIBRARY} DIRECTORY)

else()
  message(STATUS "Failed to find Apr util components.")
  set(Apr_Util_FOUND 0)
endif()

if (Apr_Util_FOUND)
  set(APU_FOUND ${Apr_Util_FOUND} CACHE INTERNAL "Apr util found" FORCE)
  set(APR_UTIL_LIBS_DIRECTORY ${APR_UTIL_LIBRARY_DIR} CACHE INTERNAL "Apr util library path" FORCE)
  set(APU_VERSION ${Apr_util_VERSION} CACHE INTERNAL "Apr util version" FORCE)

  message(STATUS "Found Apr Util: ${APU_LIBRARY_DIR}")
endif()

mark_as_advanced(APU_INCLUDE_DIR APR_UTIL_LIBRARY APU_LIBRARY_DIR APU_VERSION APU_FOUND)
