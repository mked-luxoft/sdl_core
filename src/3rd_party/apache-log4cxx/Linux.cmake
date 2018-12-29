set(CONFIGURE_FLAGS
  "--with-apr=${APR_BUILD_DIR}"
  "--with-apr-util=${APR_UTIL_BUILD_DIR}"
  "LDFLAGS=-L${EXPAT_LIBS_DIRECTORY}"
)

add_custom_command(OUTPUT ${LOG4CXX_BUILD_DIRECTORY}/Makefile
  COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ${LOG4CXX_SOURCE_DIRECTORY}/configure ${CONFIGURE_FLAGS}
  DEPENDS libapr-1
  DEPENDS apr-util
  WORKING_DIRECTORY ${LOG4CXX_BUILD_DIRECTORY}
)