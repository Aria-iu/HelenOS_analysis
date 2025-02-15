// Generated file. Fix the included header if static assert fails.
// Inlined "../../helenos/uspace/lib/c/arch/amd64/include/libarch/fibril_context.h"
/*
 * Copyright (c) 2014 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LIBC_ARCH_FIBRIL_CONTEXT_H_
#define _LIBC_ARCH_FIBRIL_CONTEXT_H_

#define __CONTEXT_OFFSET_SP   0x00
#define __CONTEXT_OFFSET_PC   0x08
#define __CONTEXT_OFFSET_RBX  0x10
#define __CONTEXT_OFFSET_RBP  0x18
#define __CONTEXT_OFFSET_R12  0x20
#define __CONTEXT_OFFSET_R13  0x28
#define __CONTEXT_OFFSET_R14  0x30
#define __CONTEXT_OFFSET_R15  0x38
#define __CONTEXT_OFFSET_TLS  0x40
#define __CONTEXT_SIZE        0x48

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct __context {
	/*
	 * We include only registers that must be preserved
	 * during function call.
	 */
	uint64_t sp;
	uint64_t pc;
	uint64_t rbx;
	uint64_t rbp;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t tls;
} __context_t;

#endif
#endif
_Static_assert(__CONTEXT_OFFSET_SP == __builtin_offsetof(struct __context, sp), "");
#ifdef __CONTEXT_SIZE_SP
_Static_assert(__CONTEXT_SIZE_SP == sizeof(((struct __context){ }).sp), "");
#endif
_Static_assert(__CONTEXT_OFFSET_PC == __builtin_offsetof(struct __context, pc), "");
#ifdef __CONTEXT_SIZE_PC
_Static_assert(__CONTEXT_SIZE_PC == sizeof(((struct __context){ }).pc), "");
#endif
_Static_assert(__CONTEXT_OFFSET_RBX == __builtin_offsetof(struct __context, rbx), "");
#ifdef __CONTEXT_SIZE_RBX
_Static_assert(__CONTEXT_SIZE_RBX == sizeof(((struct __context){ }).rbx), "");
#endif
_Static_assert(__CONTEXT_OFFSET_RBP == __builtin_offsetof(struct __context, rbp), "");
#ifdef __CONTEXT_SIZE_RBP
_Static_assert(__CONTEXT_SIZE_RBP == sizeof(((struct __context){ }).rbp), "");
#endif
_Static_assert(__CONTEXT_OFFSET_R12 == __builtin_offsetof(struct __context, r12), "");
#ifdef __CONTEXT_SIZE_R12
_Static_assert(__CONTEXT_SIZE_R12 == sizeof(((struct __context){ }).r12), "");
#endif
_Static_assert(__CONTEXT_OFFSET_R13 == __builtin_offsetof(struct __context, r13), "");
#ifdef __CONTEXT_SIZE_R13
_Static_assert(__CONTEXT_SIZE_R13 == sizeof(((struct __context){ }).r13), "");
#endif
_Static_assert(__CONTEXT_OFFSET_R14 == __builtin_offsetof(struct __context, r14), "");
#ifdef __CONTEXT_SIZE_R14
_Static_assert(__CONTEXT_SIZE_R14 == sizeof(((struct __context){ }).r14), "");
#endif
_Static_assert(__CONTEXT_OFFSET_R15 == __builtin_offsetof(struct __context, r15), "");
#ifdef __CONTEXT_SIZE_R15
_Static_assert(__CONTEXT_SIZE_R15 == sizeof(((struct __context){ }).r15), "");
#endif
_Static_assert(__CONTEXT_OFFSET_TLS == __builtin_offsetof(struct __context, tls), "");
#ifdef __CONTEXT_SIZE_TLS
_Static_assert(__CONTEXT_SIZE_TLS == sizeof(((struct __context){ }).tls), "");
#endif
_Static_assert(__CONTEXT_SIZE == sizeof(struct __context), "");
