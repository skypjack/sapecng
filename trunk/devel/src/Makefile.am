BUILT_SOURCES = parser.h
AM_YFLAGS = -d
# AM_CFLAGS = -Wall -Wextra -g -O0
AM_CFLAGS = -Wall -O2
# CLEANFILES = parser.h parser.c lexer.c
bin_PROGRAMS = sapec-ng
sapec_ng_SOURCES = common.h circuit.h circuit.c error.c \
		list.h list.c xmemrep.c parser.y lexer.l \
		expr.h expr.c sapec-ng.c
