KMOD=   dvkm
SRCS=   dvkm.h utils.h dvkm.c \
	buffer_overflow.c \
	heap_use_after_free.c \
	stack_use_after_free.c \
	arbitrary.c \
	double_fetch.c \
	disable_mitigations.c

DEBUG_FLAGS=-g
COPTFLAGS=-O0
CFLAGS=-O0 -pipe # -DDEBUG

.OBJDIR: ${.CURDIR}/obj

.include <bsd.kmod.mk>
