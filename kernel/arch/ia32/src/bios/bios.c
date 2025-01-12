/*
 * Copyright (c) 2005 Jakub Jermar
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

/** @addtogroup kernel_ia32
 * @{
 */
/** @file
 */

#include <arch/bios/bios.h>
#include <typedefs.h>

// 定义了一个宏，指向 BDA 中存储 EBDA 段地址的位置 0x40E
#define BIOS_EBDA_PTR  0x40eU

// 存储 EBDA 的物理地址
uintptr_t ebda = 0;

// 即使是AMD64架构，bios初始化依然使用这个函数。
// 从 BIOS 数据区（BIOS Data Area, BDA）中读取
// 扩展 BIOS 数据区（Extended BIOS Data Area, EBDA）的地址，
// 并将其存储在全局变量 ebda 中。
void bios_init(void)
{
	/* Copy the EBDA address out from BIOS Data Area */
	// x86 架构中，内存地址被分为段（Segment）和偏移（Offset）。
	// 段地址和偏移地址结合在一起形成一个物理地址。
	// 具体来说，段地址需要乘以 16（即左移 4 位）
	// 取出该地址的值，乘以16得到EBDA 的物理地址。
	ebda = *((uint16_t *) BIOS_EBDA_PTR) * 0x10U;
}

/** @}
 */
