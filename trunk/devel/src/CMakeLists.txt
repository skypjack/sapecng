include(CheckIncludeFiles)
check_include_files("string.h;stdlib.h" STDC_HEADERS)
check_include_files(strings.h HAVE_STRING_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(errno.h HAVE_ERRNO_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

set(spcng_SOURCES
  common.h error.c xmemrep.c
  list.h list.c
#  core.h core.c
  circapi.h circapi.c
  circuit.h circuit.c
  expr.h expr.c
  lexer.c parser.h parser.c
  sapec-ng.c )

add_executable(sapec-ng ${spcng_SOURCES})
