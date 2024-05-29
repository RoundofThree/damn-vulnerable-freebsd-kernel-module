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

static int __attribute__((no_stack_protector))
buffer_overflow_stack(void *ubuf, size_t ubufsize, int preserve_cheri_caps, int is_disclosure)
{
    char kbuf[KBUFSIZE]; // uninitialized
    int error;

    if (is_disclosure) {
        if (preserve_cheri_caps) {
            error = copyoutcap(kbuf, ubuf, ubufsize);
        } else {
            error = copyout(kbuf, ubuf, ubufsize);
        }
    } else {
        if (preserve_cheri_caps) {
            error = copyincap(ubuf, kbuf, ubufsize);
        } else {
            error = copyin(ubuf, kbuf, ubufsize);
        }
    }

    return (error);
}

static int
buffer_overflow_stack_subobject(void *ubuf, size_t ubufsize, int preserve_cheri_caps, int is_disclosure)
{
    struct buffer_overflow_obj bo_obj;
    int error;

    // bo_obj.bo_buf;  // uninitialized
    bo_obj.bo_td = curthread;

    if (is_disclosure) {
        if (preserve_cheri_caps) {
            error = copyoutcap(bo_obj.bo_buf, ubuf, ubufsize);
        } else {
            error = copyout(bo_obj.bo_buf, ubuf, ubufsize);
        }
    } else {
        if (preserve_cheri_caps) {
            error = copyincap(ubuf, bo_obj.bo_buf, ubufsize);
        } else {
            error = copyin(ubuf, bo_obj.bo_buf, ubufsize);
        }
    }

    return (error);
}

int
buffer_overflow_stack_ioctl_handler(struct dvkm_io *io, int bo_subobject)
{
    size_t ubufsize;
    void *ubuf = NULL;
    int preserve_cheri_caps, is_disclosure;
    int error;

    ubuf = io->input_buffer;
    ubufsize = io->input_buffer_size;
    preserve_cheri_caps = io->preserve_cheri_caps;
    is_disclosure = io->is_disclosure;

    if (ubuf == NULL) {
        return (EINVAL);
    }

    if (bo_subobject) {
        error = buffer_overflow_stack_subobject(ubuf, ubufsize, preserve_cheri_caps, is_disclosure);
    } else {
        error = buffer_overflow_stack(ubuf, ubufsize, preserve_cheri_caps, is_disclosure);
    }

    return (error);
}

/// Kernel malloc(9)

static int
buffer_overflow_heap(void *ubuf, size_t ubufsize, void *hbuf, int preserve_cheri_caps, int is_disclosure)
{
    int error;

    if (is_disclosure) {
        if (preserve_cheri_caps) {
            error = copyoutcap(hbuf, ubuf, ubufsize);
        } else {
            error = copyout(hbuf, ubuf, ubufsize);
        }
    } else {
        if (preserve_cheri_caps) {
            error = copyincap(ubuf, hbuf, ubufsize);
        } else {
            error = copyin(ubuf, hbuf, ubufsize);
        }
    }

    return (error);
}

// hardcoded heap buffer size
static int
buffer_overflow_heap_subobject(void *ubuf, size_t ubufsize, int preserve_cheri_caps, int is_disclosure)
{
    int error;
    struct buffer_overflow_obj *bo_obj;

    bo_obj = (struct buffer_overflow_obj *)malloc(sizeof(struct buffer_overflow_obj), M_DVKM, M_WAITOK);

    if (is_disclosure) {
        if (preserve_cheri_caps) {
            error = copyoutcap(bo_obj->bo_buf, ubuf, ubufsize);
        } else {
            error = copyout(bo_obj->bo_buf, ubuf, ubufsize);
        }
    } else {
        if (preserve_cheri_caps) {
            error = copyincap(ubuf, bo_obj->bo_buf, ubufsize);
        } else {
            error = copyin(ubuf, bo_obj->bo_buf, ubufsize);
        }
    }

    return (error);
}

int
buffer_overflow_heap_ioctl_handler(struct dvkm_io *io, int bo_subobject)
{
    size_t ubufsize, alloc_size;
    void *ubuf = NULL;
    int preserve_cheri_caps, is_disclosure;
    void *hbuf;
    int error;

    ubuf = io->input_buffer;
    ubufsize = io->input_buffer_size;
    alloc_size = io->alloc_size;
    preserve_cheri_caps = io->preserve_cheri_caps;
    is_disclosure = io->is_disclosure;

    if (ubuf == NULL) {
        return (EINVAL);
    }

    if (bo_subobject) {
        error = buffer_overflow_heap_subobject(ubuf, ubufsize, preserve_cheri_caps, is_disclosure);
    } else {
        hbuf = malloc(alloc_size, M_DVKM, M_WAITOK);  // no M_ZERO
        error = buffer_overflow_heap(ubuf, ubufsize, hbuf, preserve_cheri_caps, is_disclosure);
    }

    return (error);
}

/// Specific UMA zone

static int
buffer_overflow_uma(void *ubuf, size_t ubufsize, uma_zone_t dvkm_zone, int preserve_cheri_caps, int is_disclosure)
{
    int error;
    void *hbuf;

    hbuf = uma_zalloc(dvkm_zone, M_WAITOK);

    if (is_disclosure) {
        if (preserve_cheri_caps) {
            error = copyoutcap(hbuf, ubuf, ubufsize);
        } else {
            error = copyout(hbuf, ubuf, ubufsize);
        }
    } else {
        if (preserve_cheri_caps) {
            error = copyincap(ubuf, hbuf, ubufsize);
        } else {
            error = copyin(ubuf, hbuf, ubufsize);
        }
    }

    return (error);
}

static int
buffer_overflow_uma_subobject(void *ubuf, size_t ubufsize, uma_zone_t dvkm_zone, int preserve_cheri_caps, int is_disclosure)
{
    int error;
    struct buffer_overflow_obj *bo_obj;

    bo_obj = (struct buffer_overflow_obj *)uma_zalloc(dvkm_zone, M_WAITOK);

    if (is_disclosure) {
        if (preserve_cheri_caps) {
            error = copyoutcap(bo_obj->bo_buf, ubuf, ubufsize);
        } else {
            error = copyout(bo_obj->bo_buf, ubuf, ubufsize);
        }
    } else {
        if (preserve_cheri_caps) {
            error = copyincap(ubuf, bo_obj->bo_buf, ubufsize);
        } else {
            error = copyin(ubuf, bo_obj->bo_buf, ubufsize);
        }
    }

    return (error);
}

int
buffer_overflow_uma_ioctl_handler(struct dvkm_io *io, int bo_subobject)
{
    size_t ubufsize, alloc_size, dvkm_zone_idx;
    void *ubuf = NULL;
    int preserve_cheri_caps, is_disclosure;
    char zone_name[ZONE_NAME_MAXLEN + 1];
    uma_zone_t dvkm_zone;
    int error;

    ubuf = io->input_buffer;
    ubufsize = io->input_buffer_size;
    preserve_cheri_caps = io->preserve_cheri_caps;
    is_disclosure = io->is_disclosure;
    alloc_size = io->alloc_size;

    if (ubuf == NULL) {
        return (EINVAL);
    }

    if (bo_subobject) {
        // hardcoded dvkm_zone
        dvkm_zone = dvkm_zones[0];
        error = buffer_overflow_uma_subobject(ubuf, ubufsize, dvkm_zone, preserve_cheri_caps, is_disclosure);
    } else {
        error = copyinstr(io->zone_name, zone_name, ZONE_NAME_MAXLEN + 1, NULL);
        if (error) {
            return (error);
        }
        // search for the requested zone in the existing dvkm zones
        for (dvkm_zone_idx = 0; dvkm_zone_idx < dvkm_zones_count; ++dvkm_zone_idx) {
            if (strcmp(dvkm_zones[dvkm_zone_idx]->uz_name, zone_name) == 0) {
                break;
            }
        }
        if (dvkm_zone_idx == dvkm_zones_count && dvkm_zones_count < MAX_DVKM_ZONES) {
            dvkm_zone = uma_zcreate(zone_name, alloc_size, NULL, NULL, NULL, NULL, UMA_ALIGN_PTR, 0);
            dvkm_zones[dvkm_zones_count++] = dvkm_zone;
        } else if (dvkm_zone_idx < dvkm_zones_count) {  // found
            dvkm_zone = dvkm_zones[dvkm_zone_idx];
        } else {
            return (EBUSY);
        }
        error = buffer_overflow_uma(ubuf, ubufsize, dvkm_zone, preserve_cheri_caps, is_disclosure);
    }

    return (error);
}
