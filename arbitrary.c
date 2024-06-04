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

// subject to PAN/SMAP
int arbitrary_read_ioctl_handler(struct dvkm_io *io)
{
    int error, preserve_cheri_caps;
    void * __capability ubuf;
    uint64_t target_addr;
    void * target_ptr;
    size_t ubufsize;

    target_addr = io->target_addr;
    ubuf = io->output_buffer;
    ubufsize = io->output_buffer_size;
    preserve_cheri_caps = io->preserve_cheri_caps;

    // forge
#ifdef __CHERI_PURE_CAPABILITY__
    target_ptr = cheri_setaddress(kernel_root_cap, target_addr);
#else
    target_ptr = (void *)target_addr;
#endif

    if (preserve_cheri_caps) {
        error = copyoutcap(target_ptr, ubuf, ubufsize);
    } else {
        error = copyout(target_ptr, ubuf, ubufsize);
    }

    return (error);
}

int arbitrary_write_ioctl_handler(struct dvkm_io *io)
{
    int error, preserve_cheri_caps;
    void * __capability ubuf;
    uint64_t target_addr;
    void * target_ptr;
    size_t ubufsize;

    target_addr = io->target_addr;
    ubuf = io->input_buffer;
    ubufsize = io->input_buffer_size;
    preserve_cheri_caps = io->preserve_cheri_caps;

    // forge
#ifdef __CHERI_PURE_CAPABILITY__
    target_ptr = cheri_setaddress(kernel_root_cap, target_addr);
#else
    target_ptr = (void *)target_addr;
#endif

    if (preserve_cheri_caps) {
        error = copyincap(ubuf, target_ptr, ubufsize);
    } else {
        error = copyin(ubuf, target_ptr, ubufsize);
    }

    return (error);
}

int arbitrary_increment_ioctl_handler(struct dvkm_io *io)
{
    int error = 0, preserve_cheri_caps;
    uint64_t target_addr;
    void * target_ptr;
    int increment;

    target_addr = io->target_addr;
    preserve_cheri_caps = io->preserve_cheri_caps;
    increment = io->increment;

    // forge
#ifdef __CHERI_PURE_CAPABILITY__
    target_ptr = cheri_setaddress(kernel_root_cap, target_addr);
#else
    target_ptr = (void *)target_addr;
#endif

    if (preserve_cheri_caps) {
        *(uintptr_t *)target_ptr += increment;
    } else {
        *(char *)target_ptr += increment;
    }

    return (error);
}

int read_l0(struct dvkm_io *io)
{
    int error = 0;
    void * __capability ubuf;
    size_t ubufsize;
    void *l0;

    ubuf = io->output_buffer;
    ubufsize = io->output_buffer_size;

    l0 = curproc->p_vmspace->vm_pmap.pm_l0;
#ifdef DEBUG
    uprintf("[DEBUG] l0 = %p\n", l0);
    uprintf("[DEBUG] ubuf = %p\n", (__cheri_fromcap void *)ubuf);
    uprintf("[DEBUG] ubuf = 0x%lx\n", ubufsize);
#endif
    error = copyout(&l0, ubuf, ubufsize);
    
    return (error);
}
