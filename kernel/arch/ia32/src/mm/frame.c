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

/*
* e820 �� x86 �ܹ����������������ڴ沼�ֵı�׼��
* �����ڴ滮��Ϊ����飬ÿ�����л���ַ����С�����͡�
* �����������Ǳ�����Щ�ڴ�飬�������ͽ����ǻ��ֵ���ͬ���ڴ������С�
 * */
static void init_e820_memory(pfn_t minconf, bool low)
{
	unsigned int i;

	for (i = 0; i < e820counter; i++) {
		// ��ȡ���ڴ��Ļ���ַ�ͳ��ȡ�
		uint64_t base64 = e820table[i].base_address;
		uint64_t size64 = e820table[i].size;

// AMD64�ܹ�������겻�ᱻ����
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

		// �����ں���Ҫ�������ڴ滮�ֵͶ��ڴ�͸߶��ڴ档
		// �Ͷ��ڴ汻�ں�ʹ�ã�ͨ�����ڴ洢�ں˴��롢���ݽṹ���豸�����ȡ�
		// �߶��ڴ汻�û�����ʹ�ã�ͨ�������û��ռ������ں˶�̬������ڴ档
		// ���ڴ�����Ĺ�����Լ򵥣���Ϊ��Щ��ַ����ֱ��ӳ�䵽�ں˵������ַ�ռ䡣
		// ���ڴ�����Ĺ�����Ը��ӣ���Ϊ��Щ��ַ��Ҫͨ��ҳ��ӳ����������֧�������ڴ�ϵͳ��
		if (!frame_adjust_zone_bounds(low, &base, &size))
			continue;

		// ����ÿһ���ڴ棬��������ͣ�����һ����Ӧ���ڴ�����ṹ��
		if (e820table[i].type == MEMMAP_MEMORY_AVAILABLE) {
			/* To be safe, make the available zone possibly smaller */
			uint64_t new_base = ALIGN_UP(base, FRAME_SIZE);
			uint64_t new_size = ALIGN_DOWN(size - (new_base - base),
			    FRAME_SIZE);

			// ���ڴ��Сת��Ϊ֡����
			size_t count = SIZE2FRAMES(new_size);
			// �ڴ����ʼҳ֡�š�
			pfn_t pfn = ADDR2PFN(new_base);
			pfn_t conf;

			if (low) {
				// ���ڴ棺��AMD64�ܹ�����0-2GB��
				if ((minconf < pfn) || (minconf >= pfn + count))
					// ������������������������������������������������������������������������
					// |		AP_KERNEL				  |
					// |								  |----> pfn2
					// |								  |
					// |								  |----> pfn2 + count
					// |__________________________________| --->   minconf
					// |
					// |									--->   pfn1
					// |
					// minconfָ��AP_BOOTOFFFSET + �ں˴�С��֡��������
					// ����������minconf֡��С��pfn�����Ǹ��º�Ļ���ַ����minconf
					// ����Խ�conf����Ϊpfn����
					// ��������ڴ�����minconf֮ǰ����������ڴ�ĳ��Ȳ�����minconf��
					// ���µ�conf����Ϊpfn��
					conf = pfn;
				else
					// ������������������������������������������������������������������������
					// |		AP_KERNEL				  |
					// |								  |
					// |								  |
					// |								  |----> pfn
					// |__________________________________| --->   minconf
					// |
					// |									---> pfn + count
					// |
					// ����ڴ�ĵ�ַ��ʼ��minconf֮ǰ��������minconf֮��
					// ���µ�confֱ������Ϊ��������minconf��
					conf = minconf;
				// ����һ���µ��ڴ����򣬲�����Ҫ����ַҳ֡�����ڴ��С��֡����conf��
				zone_create(pfn, count, conf,
				    ZONE_AVAILABLE | ZONE_LOWMEM);
			} else {
				// ����Ǵ���߶��ڴ棬�ʹӵͶ��ڴ��з�������ڴ�洢��������֡��
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

// AMD64������������
// ���ڴ��ʼ����
void frame_low_arch_init(void)
{
	pfn_t minconf;

	if (config.cpu_active == 1) {
		minconf = 1;

#ifdef CONFIG_SMP
		// ����Ƕ��ϵͳ��֮ǰ����ں�copy��AP_BOOT_OFFSET�ϣ�������unmapped_size
		// ������Ҫȷ�������ϵͳ��Ҫ���õ��ں�ռ���ڴ档����

		size_t unmapped_size =
		    (uintptr_t) unmapped_end - BOOT_OFFSET;
		// FRAME_WIDTH = 12
        // ҳ��С��4K
		// ADDR2PFN(addr) = ((addr) >> FRAME_WIDTH)
        // 0x14000 = AP_BOOT_OFFSET + unmapped_size
        // ADDR2PFN(AP_BOOT_OFFSET + unmapped_size) = 0x14 = 20
		minconf = max(minconf,
		    ADDR2PFN(AP_BOOT_OFFSET + unmapped_size));
#endif

		// ����Ƕ����ϵͳ��minconfָ����ǳ���AP��Ҫ���ڴ�͵�һ��ҳ֡��ڶ���ҳ֡�š�
		// ���򣬾��ǵڶ���ҳ֡�ţ�1����
		init_e820_memory(minconf, true);

		/* Reserve frame 0 (BIOS data) */
		// ��һ��ҳ֡��ΪBIOS���ġ�����
		frame_mark_unavailable(0, 1);

#ifdef CONFIG_SMP
		/* Reserve AP real mode bootstrap memory */
		// ���� AP ������ ʵģʽ ������Ҫ���ڴ档
		// Question Here�� MultiBoot2����֮����Ǳ���ģʽ32λ��APP�����������������������Ƿ�����Ҫ��ʵģʽBoot��
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
