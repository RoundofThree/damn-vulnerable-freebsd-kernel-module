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

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/conf.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/ioccom.h>

#include <cheri/cheric.h>

#include <vm/vm.h>
#include <vm/uma.h>
#include <vm/vm_param.h>
#include <vm/vm_page.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_kern.h>
#include <vm/vm_extern.h>
#include <vm/uma_int.h>

#include <sys/queue.h>

// #define DEBUG

// make the compiler happy for FreeBSD
#if !__has_feature(capabilities)

#define __cheri_tocap
#define __cheri_fromcap
#define __cheri_offset
#define __cheri_addr

#define	copyoutcap	copyout
#define copyincap   copyin

#define	__capability

#endif

/* Constants */
#define KBUFSIZE 0x100
#define MAX_DVKM_ZONES 0x10
#define ZONE_NAME_MAXLEN 0x50

/* Heap operations */
#define KHEAP_MALLOC 1
#define KHEAP_FREE 2

/* Security mitigations mask bits */
#define DVKM_DISABLE_PAN 1

/* Structs */
struct dvkm_io {
    /* General I/O fields */
	void * __kerncap input_buffer;
    size_t input_buffer_size;
    void * __kerncap output_buffer;
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
    char * __kerncap zone_name;    // XXXR3: possibly I could manually walk memory structs
                        // to get a pointer to zone objects given a name
                        // For now, we just get dvkm_zones, for which we have 
                        // a mapping of uma_zone_t and names. 
                        // This is consumed by malloc, uma_zalloc and uma_zfree requests
    int secondary_zone; // XXXR3: unused for now. Create a secondary zone

    /* Heap use after free specific */
    int kheap_operation; // eg. KHEAP_MALLOC
    uint64_t kheap_addr;    // consumed by free

    /* Stack use after free specific */
    
    /* Arbitrary gadgets specific */
    uint64_t target_addr;  // (kernel) address to read, write or increment
    int increment;      // how much to increment (can be negative)

    /* Double fetch specific */

    /* Disable/enable security mitigations */
    int security_mitigation_mask;
};

struct buffer_overflow_obj {
    char bo_buf[KBUFSIZE];
    // fields that could be reachable:
    struct thread *bo_td;  // XXXR3: can customize this
};

struct uaf_obj {
    char uaf_buf[KBUFSIZE];
    // fields that could be reachable:
    struct thread *uaf_td;  // XXXR3: can customize this
};

// allocate up to 0x10 zones of different size
extern uma_zone_t dvkm_zones[MAX_DVKM_ZONES];
extern size_t dvkm_zones_count;

MALLOC_DECLARE(M_DVKM);

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
int disable_security_mitigation_handler(struct dvkm_io *io);
int read_l0(struct dvkm_io *io);
