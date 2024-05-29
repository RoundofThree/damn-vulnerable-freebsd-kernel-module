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

/// Kernel malloc(9)

int uaf_heap_ioctl_handler(struct dvkm_io *io)
{
    int error;
    int kheap_operation;
    void *ubuf, kheap_addr, addr;
    size_t alloc_size, ubufsize;

    kheap_operation = io->kheap_operation;
    kheap_addr = io->kheap_addr;
    ubuf = io->output_buffer;
    ubufsize = io->output_buffer_size;
    alloc_size = io->alloc_size;

    switch (kheap_operation) {
        case KHEAP_MALLOC:
            if (ubufsize < sizeof(uintptr_t)) {
                uprintf("[!] output_buffer_size shoud be at least sizeof(uintptr_t)!\n");
                return (EINVAL);
            }
            addr = malloc(alloc_size, M_DVKM, M_WAITOK);
            error = copyoutcap(&addr, ubuf, sizeof(uintptr_t));
            break;
        case KHEAP_FREE:
            free(kheap_addr, M_DVKM);
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }

    return (error);
}

/// Specific UMA zone

int uaf_uma_ioctl_handler(struct dvkm_io *io)
{
    int error;
    int kheap_operation;
    void *ubuf, kheap_addr, addr;
    uma_zone_t dvkm_zone = NULL;
    size_t dvkm_zone_idx, ubufsize, alloc_size;
    char zone_name[ZONE_NAME_MAXLEN + 1];

    kheap_operation = io->kheap_operation;
    kheap_addr = io->kheap_addr;
    ubuf = io->output_buffer;
    ubufsize = io->output_buffer_size;
    alloc_size = io->alloc_size;

    error = copyinstr(io->zone_name, zone_name, ZONE_NAME_MAXLEN + 1, NULL);
    if (error) {
        return (error);
    }

    // search for the zone with the corresponding name
    for (dvkm_zone_idx = 0; dvkm_zone_idx < dvkm_zones_count; ++dvkm_zone_idx) {
        if (strcmp(dvkm_zones[dvkm_zone_idx]->uz_name, zone_name) == 0) {
            break;
        }
    }
    if (dvkm_zone_idx < dvkm_zones_count) {  // found
        dvkm_zone = dvkm_zones[dvkm_zone_idx];
    }

    switch (kheap_operation) {
        case KHEAP_MALLOC:
            if (ubufsize < sizeof(uintptr_t)) {
                uprintf("[!] output_buffer_size shoud be at least sizeof(uintptr_t)!\n");
                return (EINVAL);
            }
            if (dvkm_zone == NULL && dvkm_zones_count < MAX_DVKM_ZONES) {
                dvkm_zone = uma_zcreate(zone_name, alloc_size, NULL, NULL, NULL, NULL, UMA_ALIGN_PTR, 0);
                dvkm_zones[dvkm_zones_count++] = dvkm_zone;
            } else if (dvkm_zones_count >= MAX_DVKM_ZONES) {
                return (EBUSY);
            }
            addr = uma_zalloc(dvkm_zone, M_WAITOK);
            error = copyoutcap(&addr, ubuf, sizeof(uintptr_t));
            break;
        case KHEAP_FREE:
            if (dvkm_zone == NULL) {
                return (EINVAL);
            }
            uma_zfree(dvkm_zone, kheap_addr);
            break;
        default:
            error = EOPNOTSUPP;
            break;
    }

    return (error);
}
