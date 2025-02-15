// Generated file. Fix the included header if static assert fails.
// Inlined "../../helenos/kernel/arch/amd64/include/arch/context_struct.h"
/*
 * Copyright (c) 2014 Jakub Jermar
 * All rights preserved.
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

#ifndef KERN_ARCH_CONTEXT_STRUCT_H_
#define KERN_ARCH_CONTEXT_STRUCT_H_

#define CONTEXT_OFFSET_SP   0x00
#define CONTEXT_OFFSET_PC   0x08
#define CONTEXT_OFFSET_RBX  0x10
#define CONTEXT_OFFSET_RBP  0x18
#define CONTEXT_OFFSET_R12  0x20
#define CONTEXT_OFFSET_R13  0x28
#define CONTEXT_OFFSET_R14  0x30
#define CONTEXT_OFFSET_R15  0x38
#define CONTEXT_OFFSET_TP   0x40
#define CONTEXT_SIZE        0x48

#ifndef __ASSEMBLER__

#include <typedefs.h>

typedef struct context {
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
	uint64_t tp;
} context_t;

#endif
#endif
_Static_assert(CONTEXT_OFFSET_SP == __builtin_offsetof(struct context, sp), "");
#ifdef CONTEXT_SIZE_SP
_Static_assert(CONTEXT_SIZE_SP == sizeof(((struct context){ }).sp), "");
#endif
_Static_assert(CONTEXT_OFFSET_PC == __builtin_offsetof(struct context, pc), "");
#ifdef CONTEXT_SIZE_PC
_Static_assert(CONTEXT_SIZE_PC == sizeof(((struct context){ }).pc), "");
#endif
_Static_assert(CONTEXT_OFFSET_RBX == __builtin_offsetof(struct context, rbx), "");
#ifdef CONTEXT_SIZE_RBX
_Static_assert(CONTEXT_SIZE_RBX == sizeof(((struct context){ }).rbx), "");
#endif
_Static_assert(CONTEXT_OFFSET_RBP == __builtin_offsetof(struct context, rbp), "");
#ifdef CONTEXT_SIZE_RBP
_Static_assert(CONTEXT_SIZE_RBP == sizeof(((struct context){ }).rbp), "");
#endif
_Static_assert(CONTEXT_OFFSET_R12 == __builtin_offsetof(struct context, r12), "");
#ifdef CONTEXT_SIZE_R12
_Static_assert(CONTEXT_SIZE_R12 == sizeof(((struct context){ }).r12), "");
#endif
_Static_assert(CONTEXT_OFFSET_R13 == __builtin_offsetof(struct context, r13), "");
#ifdef CONTEXT_SIZE_R13
_Static_assert(CONTEXT_SIZE_R13 == sizeof(((struct context){ }).r13), "");
#endif
_Static_assert(CONTEXT_OFFSET_R14 == __builtin_offsetof(struct context, r14), "");
#ifdef CONTEXT_SIZE_R14
_Static_assert(CONTEXT_SIZE_R14 == sizeof(((struct context){ }).r14), "");
#endif
_Static_assert(CONTEXT_OFFSET_R15 == __builtin_offsetof(struct context, r15), "");
#ifdef CONTEXT_SIZE_R15
_Static_assert(CONTEXT_SIZE_R15 == sizeof(((struct context){ }).r15), "");
#endif
_Static_assert(CONTEXT_OFFSET_TP == __builtin_offsetof(struct context, tp), "");
#ifdef CONTEXT_SIZE_TP
_Static_assert(CONTEXT_SIZE_TP == sizeof(((struct context){ }).tp), "");
#endif
_Static_assert(CONTEXT_SIZE == sizeof(struct context), "");
