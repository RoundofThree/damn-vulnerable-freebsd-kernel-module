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

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/ioccom.h>

/* Constants */
#define KBUFSIZE 0x100
#define MAX_DVKM_ZONES 0x10
#define ZONE_NAME_MAXLEN 0x50

/* Structs */
struct dvkm_io {
    /* General I/O fields */
	void *input_buffer;
    size_t input_buffer_size;
    void *output_buffer;
    size_t output_buffer_size;

    /* CheriBSD specific */
    int preserve_cheri_caps;

    /* Buffer overflow specific */
    // XXXR3: in the future, we can select different subobject scenarios:
    // eg. what data or struct to corrupt
    int is_disclosure;  // 1 if it's a read else write

    /* Heap specific */
    size_t alloc_size;  // malloc(9) or UMA zone size

    /* UMA specific */
    char *zone_name;    // XXXR3: possibly I could manually walk memory structs
                        // to get a pointer to zone objects given a name
    int secondary_zone; // XXXR3: unused for now. Create a secondary zone

    /* Heap use after free specific */
    int kmem_operation; // malloc, free, read, write

    /* Stack use after free specific */
    
    /* Arbitrary gadgets specific */
    void *target_addr;  // (kernel) address to read, write or increment

    /* Double fetch specific */
};

struct buffer_overflow_obj {
    char bo_buf[KBUFSIZE];
    // fields that could be reachable:
    struct thread *bo_td;  // XXXR3: can customize this
};

// allocate up to 0x10 zones of different size
uma_zone_t dvkm_zones[MAX_DVKM_ZONES];
size_t dvkm_zones_count;

/* Function prototypes */
int buffer_overflow_stack_ioctl_handler(struct dvkm_io *io, int bo_subobject);
int buffer_overflow_heap_ioctl_handler(struct dvkm_io *io, int bo_subobject);
int buffer_overflow_uma_ioctl_handler(struct dvkm_io *io, int bo_subobject);
int uaf_heap_ioctl_handler(struct dvkm_io *io);
int uaf_uma_ioctl_handler(struct dvkm_io *io);
int uaf_stack_ioctl_handler(struct dvkm_io *io);
int arbitrary_read_ioctl_handler(struct dvkm_io *io);
int arbitrary_write_ioctl_handler(struct dvkm_io *io);
int arbitrary_increment_ioctl_handler(struct dvkm_io *io);
int double_fetch_ioctl_handler(struct dvkm_io *io);
