/*
 * Copyright (c) 2011 Jakub Jermar
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

/** @addtogroup kernel_amd64_mm
 * @{
 */

#include <arch/mm/km.h>
#include <mm/km.h>
#include <config.h>
#include <macros.h>
#include <stdbool.h>

void km_identity_arch_init(void)
{
	// KM_AMD64_IDENTITY_START      UINT64_C(0xffffffff80000000)
	config.identity_base = KM_AMD64_IDENTITY_START;
	// KM_AMD64_IDENTITY_SIZE       UINT64_C(0x0000000080000000)
	config.identity_size = KM_AMD64_IDENTITY_SIZE;
}

// AMD64架构kernel memory 非直接映射初始化
void km_non_identity_arch_init(void)
{
	// KM_AMD64_NON_IDENTITY_START = 0xffff800000000000
	// KM_AMD64_NON_IDENTITY_SIZE  = 0x00007fff80000000
	// 这部分是将内核部分虚拟地址 0xffff800000000000 - 0xffffffff80000000这段地址 非 直接映射到物理内存。
	km_non_identity_span_add(KM_AMD64_NON_IDENTITY_START,
	    KM_AMD64_NON_IDENTITY_SIZE);
}

bool km_is_non_identity_arch(uintptr_t addr)
{
	return iswithin(KM_AMD64_NON_IDENTITY_START,
	    KM_AMD64_NON_IDENTITY_SIZE, addr, 1);
}

/** @}
 */
