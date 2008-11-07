BUILT_SOURCES = parser.h
AM_YFLAGS = -d -p spcng_
AM_CFLAGS = -Wall -Wextra -g -O0
# AM_CFLAGS = -Wall -O2
CLEANFILES = parser.h parser.c lexer.c
bin_PROGRAMS = sapec-ng
sapec_ng_SOURCES = common.h circuit.h circuit.c error.c \
		list.h list.c xmemrep.c parser.y lexer.l \
		expr.h expr.c circapi.h circapi.c \
		sapec-ng.c
