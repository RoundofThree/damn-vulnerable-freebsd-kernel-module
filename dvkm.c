/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2024, Zhuo Ying Jiang Li
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "dvkm.h"

/* Character device entry points */
static struct cdevsw dvkm_cdevsw = {
	.d_name = "dvkm",
	.d_version = D_VERSION,
    .d_flags = D_TRACKCLOSE,
	.d_open = dvkm_open,
	.d_close = dvkm_close,
	.d_read = dvkm_read,
	.d_write = dvkm_write,
    .d_ioctl = dvkm_ioctl,
};

static struct cdev *dvkm_cdev;

static int
dvkm_open(struct cdev *dev, int oflags, int devtype, struct thread *td)
{
	int error = 0;

	uprintf("Opened device \"dvkm\" successfully.\n");
	return (error);
}

static int
dvkm_close(struct cdev *dev, int fflag, int devtype, struct thread *td)
{
	uprintf("Closing device \"dvkm\".\n");
	return (0);
}

static int
dvkm_read(struct cdev *dev, struct uio *uio, int ioflag)
{
	int error = 0;

    uprintf("Reading from device \"dvkm\".\n");
	return (error);
}

static int
dvkm_write(struct cdev *dev, struct uio *uio, int ioflag)
{
	int error = 0;

    uprintf("Writing to device \"dvkm\".\n");
	return (error);
}

static int
dvkm_ioctl(struct cdev *dev, u_long cmd, caddr_t arg, int flags,
    struct thread *td)
{
    struct dvkm_io *io;
    int error;

    io = (void *)arg;
    error = 0;

    switch (cmd) {
    case DVKM_IOCTL_BUFFER_OVERFLOW_STACK:
        uprintf("[+] Starting to process buffer overflow stack ioctl request\n");
        error = buffer_overflow_stack_ioctl_handler(io, 0);
        uprintf("[+] Finished processing buffer overflow stack ioctl request\n");
        break;
	case DVKM_IOCTL_BUFFER_OVERFLOW_STACK_SUBOBJECT:
        uprintf("[+] Starting to process buffer overflow stack subobject ioctl request\n");
        error = buffer_overflow_stack_ioctl_handler(io, 1);
        uprintf("[+] Finished processing buffer overflow stack subobject ioctl request\n");
        break;
    default:
        error = ENOTTY;
        break;
    }

    return (error);
}

static int
dvkm_loader(struct module *m, int type, void *arg)
{
	int error = 0;

	switch (type) {
	case MOD_LOAD:
		error = make_dev_p(MAKEDEV_CHECKNAME | MAKEDEV_WAITOK,
		    &dvkm_cdev,
		    &dvkm_cdevsw,
		    0,
		    UID_ROOT,
		    GID_WHEEL,
		    0666,
		    "dvkm");
		if (error != 0)
			break;

		uprintf("[+] DVKM VULNERABLE driver loaded. Remember to unload it.\n");
		break;
	case MOD_UNLOAD:
		destroy_dev(dvkm_cdev);

		uprintf("[+] DVKM VULNERABLE driver unloaded.\n");
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}

	return (error);
}

DEV_MODULE(dvkm, dvkm_loader, NULL);
