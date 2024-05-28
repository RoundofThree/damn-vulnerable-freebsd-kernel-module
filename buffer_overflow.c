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

struct buffer_overflow_obj {
    char bo_buf[KBUFSIZE];
    // fields that could be reachable:
    struct thread *bo_td;
};

static int __attribute__((no_stack_protector))
buffer_overflow_stack(void *ubuf, size_t ubufsize, int preserve_cheri_caps)
{
    char kbuf[KBUFSIZE];
    int error;

    if (preserve_cheri_caps) {
        error = copyincap(ubuf, kbuf, ubufsize);
    } else {
        error = copyin(ubuf, kbuf, ubufsize);
    }

    return (error);
}

static int
buffer_overflow_stack_subobject(void *ubuf, size_t ubufsize, int preserve_cheri_caps)
{
    struct buffer_overflow_obj bo_obj;
    int error;

    bo_obj.bo_td = curthread;

    if (preserve_cheri_caps) {
        error = copyincap(ubuf, bo_obj.bo_buf, ubufsize);
    } else {
        error = copyin(ubuf, bo_obj.bo_buf, ubufsize);
    }

    return (error);
}

int
buffer_overflow_stack_ioctl_handler(struct dvkm_io *io, int bo_subobject)
{
    size_t ubufsize;
    void *ubuf = NULL;
    int preserve_cheri_caps;
    int error;

    ubuf = io->input_buffer;
    ubufsize = io->input_buffer_size;
    preserve_cheri_caps = io->preserve_cheri_caps;

    if (ubuf == NULL) {
        return (EINVAL);
    }

    if (bo_subobject) {
        error = buffer_overflow_stack_subobject(ubuf, ubufsize, preserve_cheri_caps);
    } else {
        error = buffer_overflow_stack(ubuf, ubufsize, preserve_cheri_caps);
    }

    return error;
}
