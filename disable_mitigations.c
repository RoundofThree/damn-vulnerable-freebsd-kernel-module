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

extern int has_pan;

// PAN = SMAP
static void
disable_pan(void)
{
    uintptr_t has_pan_addr;

    // get has_pan address
    asm volatile (
        "ldr %0, =has_pan\n"
        : "=r" (has_pan_addr)
    );

    // set has_pan to 0
    asm volatile (
        "str xzr, [%0]\n"
        : 
        : "r" (has_pan_addr)
    );

    // msr pan, #0
    asm volatile (".inst 0xd500409f | (0 << 8)\n");
}

int
disable_security_mitigation_handler(struct dvkm_io *io)
{
    int error = 0;
    int security_mitigation_mask = io->security_mitigation_mask;

    // this is not correct but we only have one option now
    if ((security_mitigation_mask & DVKM_DISABLE_PAN) != 0) {
        disable_pan();
    }
    
    return (error);
}
