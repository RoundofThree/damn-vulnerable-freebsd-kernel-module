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

// XXXR3: this is currently too simplistic
// consider other scenarios: 
//   - No pointer escape but value from unitialized stack, maybe to kROP
//   - Pointer escape through global
//   - Pointer escape through return params
// And victim targets:
//   - Escaped pointer spans an area of the current stack frame (write => could impact control flow, read => could leak and actually also impact control flow)
//   - Escaped pointer spans an area out of the current stack frame (write => umm, read => could leak and impact control flow)
//   - Escaped pointer points to a pointer that spans an area of the current stack frame => fakeobj? 
//   - Escaped pointer points to a pointer that spans an area out of the current stack frame => if powerful enough, trigger derive ptrs?

static void
func1(void **out)
{
    struct uaf_obj obj;

    *out = &obj;   // escape
}

int uaf_stack_ioctl_handler(struct dvkm_io *io)
{
    int error;
    void *obj, *ubuf;
    size_t ubufsize;

    ubuf = io->output_buffer;
    ubufsize = io->output_buffer_size;

    if (ubuf == NULL || ubufsize == 0) {
        return (EINVAL);
    }

    func1(&obj);

    error = copyoutcap(obj, ubuf, ubufsize);

    return (error);
}
