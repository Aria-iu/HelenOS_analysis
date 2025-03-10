// Generated file. Fix the included header if static assert fails.
// Inlined "../../helenos/kernel/arch/amd64/include/arch/boot/memmap_struct.h"
/*
 * Copyright (c) 2016 Jakub Jermar
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

#ifndef KERN_ARCH_BOOT_MEMMAP_STRUCT_H_
#define KERN_ARCH_BOOT_MEMMAP_STRUCT_H_

#define E820MEMMAP_OFFSET_BASE_ADDRESS  0x00
#define E820MEMMAP_OFFSET_SIZE          0x08
#define E820MEMMAP_OFFSET_TYPE          0x10
#define E820MEMMAP_SIZE                 0x14

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct e820memmap {
	uint64_t base_address;
	uint64_t size;
	uint32_t type;
} __attribute__((packed)) e820memmap_t;

#endif
#endif
_Static_assert(E820MEMMAP_OFFSET_BASE_ADDRESS == __builtin_offsetof(struct e820memmap, base_address), "");
#ifdef E820MEMMAP_SIZE_BASE_ADDRESS
_Static_assert(E820MEMMAP_SIZE_BASE_ADDRESS == sizeof(((struct e820memmap){ }).base_address), "");
#endif
_Static_assert(E820MEMMAP_OFFSET_SIZE == __builtin_offsetof(struct e820memmap, size), "");
#ifdef E820MEMMAP_SIZE_SIZE
_Static_assert(E820MEMMAP_SIZE_SIZE == sizeof(((struct e820memmap){ }).size), "");
#endif
_Static_assert(E820MEMMAP_OFFSET_TYPE == __builtin_offsetof(struct e820memmap, type), "");
#ifdef E820MEMMAP_SIZE_TYPE
_Static_assert(E820MEMMAP_SIZE_TYPE == sizeof(((struct e820memmap){ }).type), "");
#endif
_Static_assert(E820MEMMAP_SIZE == sizeof(struct e820memmap), "");
