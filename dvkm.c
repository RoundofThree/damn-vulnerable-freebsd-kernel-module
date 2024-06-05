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

MALLOC_DEFINE(M_DVKM, "dvkmbuf", "DVKM general heap buffer");

uma_zone_t dvkm_zones[MAX_DVKM_ZONES];
size_t dvkm_zones_count;

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

    uprintf("Reading from device \"dvkm\" does nothing.\n");
    return (error);
}

static int
dvkm_write(struct cdev *dev, struct uio *uio, int ioflag)
{
    int error = 0;

    uprintf("Writing to device \"dvkm\" does nothing.\n");
    return (error);
}

static int
dvkm_ioctl(struct cdev *dev, u_long cmd, caddr_t arg, int flags,
    struct thread *td)
{
    struct dvkm_io *io;
    int error;

    io = (struct dvkm_io *)arg;

#ifdef DEBUG
    uprintf("[DEBUG] IOCTL code = %lx\n", cmd);
#endif

    switch (cmd) {
        case DVKM_IOCTL_BUFFER_OVERFLOW_STACK:
        {
            uprintf("[+] Starting to process buffer overflow stack ioctl request\n");
            error = buffer_overflow_stack_ioctl_handler(io, 0);
            uprintf("[+] Finished processing buffer overflow stack ioctl request\n");
            break;
        }
        case DVKM_IOCTL_BUFFER_OVERFLOW_STACK_SUBOBJECT:
        {
            uprintf("[+] Starting to process buffer overflow stack subobject ioctl request\n");
            error = buffer_overflow_stack_ioctl_handler(io, 1);
            uprintf("[+] Finished processing buffer overflow stack subobject ioctl request\n");
            break;
        }
        case DVKM_IOCTL_BUFFER_OVERFLOW_HEAP:
        {
            uprintf("[+] Starting to process buffer overflow heap ioctl request\n");
            error = buffer_overflow_heap_ioctl_handler(io, 0);
            uprintf("[+] Finished processing buffer overflow heap ioctl request\n");
            break;
        }
        case DVKM_IOCTL_BUFFER_OVERFLOW_HEAP_SUBOBJECT:
        {
            uprintf("[+] Starting to process buffer overflow heap subobject ioctl request\n");
            error = buffer_overflow_heap_ioctl_handler(io, 1);
            uprintf("[+] Finished processing buffer overflow heap subobject ioctl request\n");
            break;
        }
        case DVKM_IOCTL_BUFFER_OVERFLOW_HEAP_UMA:
        {
            uprintf("[+] Starting to process buffer overflow in UMA zone ioctl request\n");
            error = buffer_overflow_uma_ioctl_handler(io, 0);
            uprintf("[+] Finished processing buffer overflow in UMA zone ioctl request\n");
            break;
        }
        case DVKM_IOCTL_BUFFER_OVERFLOW_HEAP_UMA_SUBOBJECT:
        {
            uprintf("[+] Starting to process buffer overflow in UMA zone subobject ioctl request\n");
            error = buffer_overflow_uma_ioctl_handler(io, 1);
            uprintf("[+] Finished processing buffer overflow in UMA zone subobject ioctl request\n");
            break;
        }
        case DVKM_IOCTL_UAF_HEAP:
        {
            uprintf("[+] Starting to process heap UAF ioctl request\n");
            error = uaf_heap_ioctl_handler(io);
            uprintf("[+] Finished processing heap UAF ioctl request\n");
            break;
        }
        case DVKM_IOCTL_UAF_HEAP_UMA:
        {
            uprintf("[+] Starting to process UMA zone UAF ioctl request\n");
            error = uaf_uma_ioctl_handler(io);
            uprintf("[+] Finished processing UMA zone UAF ioctl request\n");
            break;
        }
        case DVKM_IOCTL_UAF_STACK:
        {
            uprintf("[+] Starting to process stack UAF ioctl request\n");
            error = uaf_stack_ioctl_handler(io);
            uprintf("[+] Finished processing stack UAF ioctl request\n");
            break;
        }
        case DVKM_IOCTL_ARBITRARY_READ:
        {
            uprintf("[+] Starting to process arbitrary read ioctl request\n");
            error = arbitrary_read_ioctl_handler(io);
            uprintf("[+] Finished processing arbitrary read ioctl request\n");
            break;
        }
        case DVKM_IOCTL_ARBITRARY_WRITE:
        {
            uprintf("[+] Starting to process arbitrary write ioctl request\n");
            error = arbitrary_write_ioctl_handler(io);
            uprintf("[+] Finished processing arbitrary write ioctl request\n");
            break;
        }
        case DVKM_IOCTL_ARBITRARY_INCREMENT:
        {
            uprintf("[+] Starting to process arbitrary increment ioctl request\n");
            error = arbitrary_increment_ioctl_handler(io);
            uprintf("[+] Finished processing arbitrary increment ioctl request\n");
            break;
        }
        case DVKM_IOCTL_DOUBLE_FETCH:
        {
            uprintf("[+] Starting to process double fetch ioctl request\n");
            error = double_fetch_ioctl_handler(io);
            uprintf("[+] Finished processing double fetch ioctl request\n");
            break;
        }
        case DVKM_IOCTL_DISABLE_SECURITY:
        {
            uprintf("[+] Starting to process disable security mitigation request\n");
            error = disable_security_mitigation_handler(io);
            uprintf("[+] Finished processing disable security mitigation request\n");
            break;
        }
        case DVKM_IOCTL_READ_PMAP_TOP:
        {
            uprintf("[+] Starting to process read PMAP L0 request\n");
            error = read_l0(io);
            uprintf("[+] Finished processing read PMAP L0 request\n");
            break;
        }
        default:
        {
            error = EINVAL;
            break;
        }
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

        memset(dvkm_zones, 0, sizeof(dvkm_zones));
        dvkm_zones_count = 0;
        dvkm_zones[dvkm_zones_count++] = uma_zcreate("dvkm default zone", sizeof(struct buffer_overflow_obj),
            NULL, NULL, NULL, NULL, UMA_ALIGN_PTR, 0);

        uprintf("[+] DVKM VULNERABLE driver loaded. Remember to unload it.\n");
        break;
    case MOD_UNLOAD:
        while (dvkm_zones_count > 0) {
            uma_zdestroy(dvkm_zones[--dvkm_zones_count]);
        }

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
