KMOD=   dvkm
SRCS=   dvkm.h utils.h dvkm.c \
	buffer_overflow.c

.OBJDIR: ${.CURDIR}/obj

.include <bsd.kmod.mk>
