// Generated file. Fix the included header if static assert fails.
// Inlined "../../helenos/kernel/arch/amd64/include/arch/istate_struct.h"
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

#ifndef KERN_ARCH_ISTATE_STRUCT_H_
#define KERN_ARCH_ISTATE_STRUCT_H_

#define ISTATE_OFFSET_RAX         0x00
#define ISTATE_OFFSET_RBX         0x08
#define ISTATE_OFFSET_RCX         0x10
#define ISTATE_OFFSET_RDX         0x18
#define ISTATE_OFFSET_RSI         0x20
#define ISTATE_OFFSET_RDI         0x28
#define ISTATE_OFFSET_RBP         0x30
#define ISTATE_OFFSET_R8          0x38
#define ISTATE_OFFSET_R9          0x40
#define ISTATE_OFFSET_R10         0x48
#define ISTATE_OFFSET_R11         0x50
#define ISTATE_OFFSET_R12         0x58
#define ISTATE_OFFSET_R13         0x60
#define ISTATE_OFFSET_R14         0x68
#define ISTATE_OFFSET_R15         0x70
#define ISTATE_OFFSET_ALIGNMENT   0x78
#define ISTATE_OFFSET_RBP_FRAME   0x80
#define ISTATE_OFFSET_RIP_FRAME   0x88
#define ISTATE_OFFSET_ERROR_WORD  0x90
#define ISTATE_OFFSET_RIP         0x98
#define ISTATE_OFFSET_CS          0xa0
#define ISTATE_OFFSET_RFLAGS      0xa8
#define ISTATE_OFFSET_RSP         0xb0
#define ISTATE_OFFSET_SS          0xb8
#define ISTATE_SIZE               0xc0

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct istate {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	/* align rbp_frame on multiple of 16 */
	uint64_t alignment;
	/* imitation of frame pointer linkage */
	uint64_t rbp_frame;
	/* imitation of frame address linkage */
	uint64_t rip_frame;
	/* real or fake error word */
	uint64_t error_word;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	/* only if istate_t is from uspace */
	uint64_t rsp;
	/* only if istate_t is from uspace */
	uint64_t ss;
} istate_t;

#endif
#endif
_Static_assert(ISTATE_OFFSET_RAX == __builtin_offsetof(struct istate, rax), "");
#ifdef ISTATE_SIZE_RAX
_Static_assert(ISTATE_SIZE_RAX == sizeof(((struct istate){ }).rax), "");
#endif
_Static_assert(ISTATE_OFFSET_RBX == __builtin_offsetof(struct istate, rbx), "");
#ifdef ISTATE_SIZE_RBX
_Static_assert(ISTATE_SIZE_RBX == sizeof(((struct istate){ }).rbx), "");
#endif
_Static_assert(ISTATE_OFFSET_RCX == __builtin_offsetof(struct istate, rcx), "");
#ifdef ISTATE_SIZE_RCX
_Static_assert(ISTATE_SIZE_RCX == sizeof(((struct istate){ }).rcx), "");
#endif
_Static_assert(ISTATE_OFFSET_RDX == __builtin_offsetof(struct istate, rdx), "");
#ifdef ISTATE_SIZE_RDX
_Static_assert(ISTATE_SIZE_RDX == sizeof(((struct istate){ }).rdx), "");
#endif
_Static_assert(ISTATE_OFFSET_RSI == __builtin_offsetof(struct istate, rsi), "");
#ifdef ISTATE_SIZE_RSI
_Static_assert(ISTATE_SIZE_RSI == sizeof(((struct istate){ }).rsi), "");
#endif
_Static_assert(ISTATE_OFFSET_RDI == __builtin_offsetof(struct istate, rdi), "");
#ifdef ISTATE_SIZE_RDI
_Static_assert(ISTATE_SIZE_RDI == sizeof(((struct istate){ }).rdi), "");
#endif
_Static_assert(ISTATE_OFFSET_RBP == __builtin_offsetof(struct istate, rbp), "");
#ifdef ISTATE_SIZE_RBP
_Static_assert(ISTATE_SIZE_RBP == sizeof(((struct istate){ }).rbp), "");
#endif
_Static_assert(ISTATE_OFFSET_R8 == __builtin_offsetof(struct istate, r8), "");
#ifdef ISTATE_SIZE_R8
_Static_assert(ISTATE_SIZE_R8 == sizeof(((struct istate){ }).r8), "");
#endif
_Static_assert(ISTATE_OFFSET_R9 == __builtin_offsetof(struct istate, r9), "");
#ifdef ISTATE_SIZE_R9
_Static_assert(ISTATE_SIZE_R9 == sizeof(((struct istate){ }).r9), "");
#endif
_Static_assert(ISTATE_OFFSET_R10 == __builtin_offsetof(struct istate, r10), "");
#ifdef ISTATE_SIZE_R10
_Static_assert(ISTATE_SIZE_R10 == sizeof(((struct istate){ }).r10), "");
#endif
_Static_assert(ISTATE_OFFSET_R11 == __builtin_offsetof(struct istate, r11), "");
#ifdef ISTATE_SIZE_R11
_Static_assert(ISTATE_SIZE_R11 == sizeof(((struct istate){ }).r11), "");
#endif
_Static_assert(ISTATE_OFFSET_R12 == __builtin_offsetof(struct istate, r12), "");
#ifdef ISTATE_SIZE_R12
_Static_assert(ISTATE_SIZE_R12 == sizeof(((struct istate){ }).r12), "");
#endif
_Static_assert(ISTATE_OFFSET_R13 == __builtin_offsetof(struct istate, r13), "");
#ifdef ISTATE_SIZE_R13
_Static_assert(ISTATE_SIZE_R13 == sizeof(((struct istate){ }).r13), "");
#endif
_Static_assert(ISTATE_OFFSET_R14 == __builtin_offsetof(struct istate, r14), "");
#ifdef ISTATE_SIZE_R14
_Static_assert(ISTATE_SIZE_R14 == sizeof(((struct istate){ }).r14), "");
#endif
_Static_assert(ISTATE_OFFSET_R15 == __builtin_offsetof(struct istate, r15), "");
#ifdef ISTATE_SIZE_R15
_Static_assert(ISTATE_SIZE_R15 == sizeof(((struct istate){ }).r15), "");
#endif
_Static_assert(ISTATE_OFFSET_ALIGNMENT == __builtin_offsetof(struct istate, alignment), "");
#ifdef ISTATE_SIZE_ALIGNMENT
_Static_assert(ISTATE_SIZE_ALIGNMENT == sizeof(((struct istate){ }).alignment), "");
#endif
_Static_assert(ISTATE_OFFSET_RBP_FRAME == __builtin_offsetof(struct istate, rbp_frame), "");
#ifdef ISTATE_SIZE_RBP_FRAME
_Static_assert(ISTATE_SIZE_RBP_FRAME == sizeof(((struct istate){ }).rbp_frame), "");
#endif
_Static_assert(ISTATE_OFFSET_RIP_FRAME == __builtin_offsetof(struct istate, rip_frame), "");
#ifdef ISTATE_SIZE_RIP_FRAME
_Static_assert(ISTATE_SIZE_RIP_FRAME == sizeof(((struct istate){ }).rip_frame), "");
#endif
_Static_assert(ISTATE_OFFSET_ERROR_WORD == __builtin_offsetof(struct istate, error_word), "");
#ifdef ISTATE_SIZE_ERROR_WORD
_Static_assert(ISTATE_SIZE_ERROR_WORD == sizeof(((struct istate){ }).error_word), "");
#endif
_Static_assert(ISTATE_OFFSET_RIP == __builtin_offsetof(struct istate, rip), "");
#ifdef ISTATE_SIZE_RIP
_Static_assert(ISTATE_SIZE_RIP == sizeof(((struct istate){ }).rip), "");
#endif
_Static_assert(ISTATE_OFFSET_CS == __builtin_offsetof(struct istate, cs), "");
#ifdef ISTATE_SIZE_CS
_Static_assert(ISTATE_SIZE_CS == sizeof(((struct istate){ }).cs), "");
#endif
_Static_assert(ISTATE_OFFSET_RFLAGS == __builtin_offsetof(struct istate, rflags), "");
#ifdef ISTATE_SIZE_RFLAGS
_Static_assert(ISTATE_SIZE_RFLAGS == sizeof(((struct istate){ }).rflags), "");
#endif
_Static_assert(ISTATE_OFFSET_RSP == __builtin_offsetof(struct istate, rsp), "");
#ifdef ISTATE_SIZE_RSP
_Static_assert(ISTATE_SIZE_RSP == sizeof(((struct istate){ }).rsp), "");
#endif
_Static_assert(ISTATE_OFFSET_SS == __builtin_offsetof(struct istate, ss), "");
#ifdef ISTATE_SIZE_SS
_Static_assert(ISTATE_SIZE_SS == sizeof(((struct istate){ }).ss), "");
#endif
_Static_assert(ISTATE_SIZE == sizeof(struct istate), "");
