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

#include "utils.h"

/* Constants */

#ifndef DVKM_IOCTL

#define DKM_IOCTL ('I')
#define DVKM_IOCTL_BUFFER_OVERFLOW_STACK _IOWR(DKM_IOCTL, 1, struct dvkm_io)
#define DVKM_IOCTL_BUFFER_OVERFLOW_STACK_SUBOBJECT _IOWR(DKM_IOCTL, 2, struct dvkm_io)
#define DVKM_IOCTL_BUFFER_OVERFLOW_HEAP _IOWR(DKM_IOCTL, 3, struct dvkm_io)
#define DVKM_IOCTL_BUFFER_OVERFLOW_HEAP_SUBOBJECT _IOWR(DKM_IOCTL, 4, struct dvkm_io)
#define DVKM_IOCTL_BUFFER_OVERFLOW_HEAP_UMA _IOWR(DKM_IOCTL, 5, struct dvkm_io)
#define DVKM_IOCTL_BUFFER_OVERFLOW_HEAP_UMA_SUBOBJECT _IOWR(DKM_IOCTL, 6, struct dvkm_io)
#define DVKM_IOCTL_UAF_HEAP _IOWR(DKM_IOCTL, 7, struct dvkm_io)
#define DVKM_IOCTL_UAF_HEAP_UMA _IOWR(DKM_IOCTL, 8, struct dvkm_io)
#define DVKM_IOCTL_UAF_STACK _IOWR(DKM_IOCTL, 9, struct dvkm_io)
#define DVKM_IOCTL_ARBITRARY_READ _IOWR(DKM_IOCTL, 10, struct dvkm_io)
#define DVKM_IOCTL_ARBITRARY_WRITE _IOWR(DKM_IOCTL, 11, struct dvkm_io)
#define DVKM_IOCTL_ARBITRARY_INCREMENT _IOWR(DKM_IOCTL, 12, struct dvkm_io)
#define DVKM_IOCTL_DOUBLE_FETCH _IOWR(DKM_IOCTL, 13, struct dvkm_io)
#define DVKM_IOCTL_DISABLE_SECURITY _IOWR(DKM_IOCTL, 14, struct dvkm_io)
#define DVKM_IOCTL_READ_PMAP_L0 _IOWR(DKM_IOCTL, 15, struct dvkm_io)

// TODO: pointer hijack

#endif /* DVKM_IOCTL */

/* Function prototypes */
static d_open_t      dvkm_open;
static d_close_t     dvkm_close;
static d_read_t      dvkm_read;
static d_write_t     dvkm_write;
static d_ioctl_t     dvkm_ioctl;
