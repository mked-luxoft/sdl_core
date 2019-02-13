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

include(ExternalProject)

set(BOOST_AGL_PROJECT_CONFIG_JAM
  "using gcc : agl : x86_64-agl-linux-gcc -march=corei7 -mtune=corei7 "
  "-mfpmath=sse -msse4.2 --sysroot=/opt/agl-sdk/6.0.2-corei7-64/sysroots/corei7-64-agl-linux : ")

set(CONFIGURE_COMMAND
  CC=\"\" ./bootstrap.sh
    --with-libraries=system,thread,date_time,filesystem
    --prefix=${3RD_PARTY_INSTALL_PREFIX}
    COMMAND echo ${BOOST_AGL_PROJECT_CONFIG_JAM} $<SEMICOLON> >> ./project-config.jam)

set(BUILD_COMMAND
  ./b2 toolset=gcc-agl)

ExternalProject_Add(
  Boost
  URL http://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
  DOWNLOAD_DIR ${BOOST_SOURCE_DIRECTORY}
  SOURCE_DIR ${BOOST_SOURCE_DIRECTORY}
  CONFIGURE_COMMAND ${CONFIGURE_COMMAND}
  BUILD_COMMAND ${BUILD_COMMAND}
  BUILD_IN_SOURCE true
  INSTALL_COMMAND ""
)

set(INSTALL_COMMAND
  ${BUILD_COMMAND} install > boost_install.log)

if (${3RD_PARTY_INSTALL_PREFIX} MATCHES "/usr/local")
  set(INSTALL_COMMAND sudo ${INSTALL_COMMAND})
endif()

string (REPLACE ";" " " INSTALL_COMMAND "${INSTALL_COMMAND}")
install(
  CODE "execute_process(
    WORKING_DIRECTORY ${BOOST_SOURCE_DIRECTORY}
    COMMAND /bin/bash -c \"${INSTALL_COMMAND}\"
  )"
)
