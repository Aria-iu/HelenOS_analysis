/*
 * Copyright (c) 2008 Jakub Jermar
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

/** @addtogroup kernel_ia32_mm
 * @{
 */
/** @file
 * @ingroup kernel_ia32_mm, kernel_amd64_mm
 */

#include <mm/frame.h>
#include <arch/mm/frame.h>
#include <mm/as.h>
#include <config.h>
#include <arch/boot/boot.h>
#include <arch/boot/memmap.h>
#include <panic.h>
#include <debug.h>
#include <align.h>
#include <macros.h>
#include <stdio.h>

#define PHYSMEM_LIMIT32  UINT64_C(0x100000000)

static void init_e820_memory(pfn_t minconf, bool low)
{
	unsigned int i;

	for (i = 0; i < e820counter; i++) {
		// 获取到内存块的基地址和长度。
		uint64_t base64 = e820table[i].base_address;
		uint64_t size64 = e820table[i].size;

#ifdef KARCH_ia32
		/*
		 * Restrict the e820 table entries to 32-bits.
		 */
		if (base64 >= PHYSMEM_LIMIT32)
			continue;

		if (base64 + size64 > PHYSMEM_LIMIT32)
			size64 = PHYSMEM_LIMIT32 - base64;
#endif

		uintptr_t base = (uintptr_t) base64;
		size_t size = (size_t) size64;

		// 根据内核需要的物理内存划分低端内存和高端内存。
		// 低端内存被内核使用，通常用于存储内核代码、数据结构、设备驱动等。
		// 高端内存被用户程序使用，通常用于用户空间程序和内核动态分配的内存。
		// 低内存区域的管理相对简单，因为这些地址可以直接映射到内核的虚拟地址空间。
		// 高内存区域的管理相对复杂，因为这些地址需要通过页表映射来管理，以支持虚拟内存系统。
		if (!frame_adjust_zone_bounds(low, &base, &size))
			continue;

		// 对于每一块内存，检查其类型，创建一个对应的内存区域结构。
		if (e820table[i].type == MEMMAP_MEMORY_AVAILABLE) {
			/* To be safe, make the available zone possibly smaller */
			uint64_t new_base = ALIGN_UP(base, FRAME_SIZE);
			uint64_t new_size = ALIGN_DOWN(size - (new_base - base),
			    FRAME_SIZE);

			// 将内存大小转换为帧数。
			size_t count = SIZE2FRAMES(new_size);
			// 内存的起始页帧号。
			pfn_t pfn = ADDR2PFN(new_base);
			pfn_t conf;

			if (low) {
				// 低内存：对AMD64架构，是0-2GB。
				if ((minconf < pfn) || (minconf >= pfn + count))
					// ——————————————————
					// |		AP_KERNEL				  |
					// |								  |----> pfn2
					// |								  |
					// |								  |----> pfn2 + count
					// |__________________________________| --->   minconf
					// |
					// |									--->   pfn1
					// |
					// minconf指向AP_BOOTOFFFSET + 内核大小的帧数。。。
					// 若传进来的minconf帧数小于pfn，就是更新后的基地址大于minconf
					// 则可以将conf设置为pfn。。
					// 或者这块内存是在minconf之前，但是这块内存的长度不超过minconf。
					// 将新的conf设置为pfn。
					conf = pfn;
				else
					// ——————————————————
					// |		AP_KERNEL				  |
					// |								  |
					// |								  |
					// |								  |----> pfn
					// |__________________________________| --->   minconf
					// |
					// |									---> pfn + count
					// |
					// 这里，内存的地址起始在minconf之前，结束在minconf之后。
					// 将新的conf直接设置为传进来的minconf。
					conf = minconf;
				// 创建一个新的内存区域，参数需要基地址页帧数，内存大小的帧数和conf。
				zone_create(pfn, count, conf,
				    ZONE_AVAILABLE | ZONE_LOWMEM);
			} else {
				// 如果是处理高端内存，就从低端内存中分配出来内存存储数据配置帧。
				conf = zone_external_conf_alloc(count);
				if (conf != 0)
					zone_create(pfn, count, conf,
					    ZONE_AVAILABLE | ZONE_HIGHMEM);
			}
		} 
		else if ((e820table[i].type == MEMMAP_MEMORY_ACPI) || (e820table[i].type == MEMMAP_MEMORY_NVS)) {
			/* To be safe, make the firmware zone possibly larger */
			uint64_t new_base = ALIGN_DOWN(base, FRAME_SIZE);
			uint64_t new_size = ALIGN_UP(size + (base - new_base),
			    FRAME_SIZE);

			zone_create(ADDR2PFN(new_base), SIZE2FRAMES(new_size), 0,
			    ZONE_FIRMWARE);
		} 
		else {
			/* To be safe, make the reserved zone possibly larger */
			uint64_t new_base = ALIGN_DOWN(base, FRAME_SIZE);
			uint64_t new_size = ALIGN_UP(size + (base - new_base),
			    FRAME_SIZE);

			zone_create(ADDR2PFN(new_base), SIZE2FRAMES(new_size), 0,
			    ZONE_RESERVED);
		}
	}
}

static const char *e820names[] = {
	"invalid",
	"available",
	"reserved",
	"acpi",
	"nvs",
	"unusable"
};

void physmem_print(void)
{
	unsigned int i;
	printf("[base            ] [size            ] [name   ]\n");

	for (i = 0; i < e820counter; i++) {
		const char *name;

		if (e820table[i].type <= MEMMAP_MEMORY_UNUSABLE)
			name = e820names[e820table[i].type];
		else
			name = "invalid";

		printf("%#018" PRIx64 " %#018" PRIx64 " %s\n", e820table[i].base_address,
		    e820table[i].size, name);
	}
}

// AMD64会调用这个函数
// 低内存初始化。
void frame_low_arch_init(void)
{
	pfn_t minconf;

	if (config.cpu_active == 1) {
		minconf = 1;

#ifdef CONFIG_SMP
		// 如果是多核系统，之前会把内核copy到AP_BOOT_OFFSET上，长度是unmapped_size
		// 所以需要确定多核心系统需要设置的内核占用内存。。。
		size_t unmapped_size =
		    (uintptr_t) unmapped_end - BOOT_OFFSET;

		// FRAME_WIDTH = 12
		// ADDR2PFN(addr) = ((addr) >> FRAME_WIDTH)
		minconf = max(minconf,
		    ADDR2PFN(AP_BOOT_OFFSET + unmapped_size));
#endif

		// 如果是多核心系统，minconf指向的是除开AP需要的内存和第一个页帧外第二个页帧号。
		// 否则，就是第二个页帧号（1）。
		init_e820_memory(minconf, true);

		/* Reserve frame 0 (BIOS data) */
		// 第一个页帧是为BIOS留的。。。
		frame_mark_unavailable(0, 1);

#ifdef CONFIG_SMP
		/* Reserve AP real mode bootstrap memory */
		// 保留 AP 处理器 实模式 启动需要的内存。
		// Question Here： MultiBoot2启动之后就是保护模式32位？APP处理器由主处理器启动，是否还是需要从实模式Boot？
		frame_mark_unavailable(AP_BOOT_OFFSET >> FRAME_WIDTH,
		    unmapped_size >> FRAME_WIDTH);
#endif
	}
}

void frame_high_arch_init(void)
{
	if (config.cpu_active == 1)
		init_e820_memory(0, false);
}

/** @}
 */
