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

/** @addtogroup kernel_ia32
 * @{
 */
/** @file
 */

#include <smp/smp.h>
#include <arch/smp/smp.h>
#include <arch/smp/mps.h>
#include <arch/smp/ap.h>
#include <arch/boot/boot.h>
#include <assert.h>
#include <errno.h>
#include <genarch/acpi/acpi.h>
#include <genarch/acpi/madt.h>
#include <config.h>
#include <synch/waitq.h>
#include <arch/pm.h>
#include <halt.h>
#include <panic.h>
#include <arch/asm.h>
#include <mm/page.h>
#include <mm/frame.h>
#include <mm/km.h>
#include <stdlib.h>
#include <mm/as.h>
#include <log.h>
#include <memw.h>
#include <genarch/drivers/i8259/i8259.h>
#include <cpu.h>

#ifdef CONFIG_SMP

static struct smp_config_operations *ops = NULL;

// 多处理器系统（SMP）初始化函数
// 根据系统的配置和硬件支持来设置和初始化相关的多处理器设置
void smp_init(void)
{
	// acpi_madt 是一个 ACPI 表，它描述了系统中的多个中断控制器（APIC），
	// 包括本地 APIC（LAPIC）和 I/O APIC。
	if (acpi_madt) {
		// 其中会设置 config.cpu_count
		// config.cpu_count = madt_l_apic_entry_cnt;
		acpi_madt_parse();
		ops = &madt_config_operations;
	}

	// 若是多核，这里应该大于1
	if (config.cpu_count == 1) {
		mps_init();
		ops = &mps_config_operations;
	}

	// 多核处理器系统初始化（cpu_count > 1）
	// 本地 APIC（LAPIC）用于处理本地 CPU 上的中断，
	// 而 I/O APIC 用于处理 I/O 设备的中断。
	if (config.cpu_count > 1) {
		l_apic = (uint32_t *) km_map((uintptr_t) l_apic, PAGE_SIZE,
		    PAGE_SIZE, PAGE_WRITE | PAGE_NOT_CACHEABLE);
		io_apic = (uint32_t *) km_map((uintptr_t) io_apic, PAGE_SIZE,
		    PAGE_SIZE, PAGE_WRITE | PAGE_NOT_CACHEABLE);
	}
}

static void cpu_arch_id_init(void)
{
	assert(ops != NULL);
	assert(cpus != NULL);

	for (unsigned int i = 0; i < config.cpu_count; ++i) {
		cpus[i].arch.id = ops->cpu_apic_id(i);
	}
}

/*
 * Kernel thread for bringing up application processors. It becomes clear
 * that we need an arrangement like this (AP's being initialized by a kernel
 * thread), for a thread has its dedicated stack. (The stack used during the
 * BSP initialization (prior the very first call to scheduler()) will be used
 * as an initialization stack for each AP.)
 */
// 这里时AMD64中的kmp线程函数，用来拉起 AP。通过发送INIT IPI（中断处理指令）来启动应用处理器
void kmp(void *arg __attribute__((unused)))
{
	unsigned int i;

	assert(ops != NULL);

	/*
	 * SMP initialized, cpus array allocated. Assign each CPU its
	 * physical APIC ID.
	 */
	// 为每个CPU赋值它们各自的物理 APIC ID. --- cpus[i].arch.id
	cpu_arch_id_init();

	/*
	 * We need to access data in frame 0.
	 * We boldly make use of kernel address space mapping.
	 */

	/*
	 * Set the warm-reset vector to the real-mode address of 4K-aligned ap_boot()
	 */
	// ap_boot是AP启动函数的地址
	// 设置warm-reset向量，以便APs在启动时能够跳转到正确的地址
	// 0x467：warm-reset向量的地址
	*((uint16_t *) (PA2KA(0x467 + 0))) =
	    (uint16_t) (((uintptr_t) ap_boot) >> 4);  /* segment */
	*((uint16_t *) (PA2KA(0x467 + 2))) = 0;       /* offset */

	/*
	 * Save 0xa to address 0xf of the CMOS RAM.
	 * BIOS will not do the POST after the INIT signal.
	 */
	pio_write_8((ioport8_t *) 0x70, 0xf);
	pio_write_8((ioport8_t *) 0x71, 0xa);

	// 禁用所有中断，使用 apic 来投递中断的话不再使用 8259
	i8259_disable_irqs(0xffff);
	// 初始化APIC
	apic_init();

	for (i = 0; i < config.cpu_count; i++) {
		/*
		 * Skip processors marked unusable.
		 */
		// 跳过不可用的CPU
		if (!ops->cpu_enabled(i))
			continue;

		/*
		 * The bootstrap processor is already up.
		 */
		// 跳过引导处理器（BSP）
		if (ops->cpu_bootstrap(i))
			continue;

		// 跳过自身
		if (ops->cpu_apic_id(i) == bsp_l_apic) {
			log(LF_ARCH, LVL_ERROR, "kmp: bad processor entry #%u, "
			    "will not send IPI to myself", i);
			continue;
		}

		/*
		 * Prepare new GDT for CPU in question.
		 */

		/*
		 * XXX Flag FRAME_LOW_4_GiB was removed temporarily,
		 * it needs to be replaced by a generic fuctionality of
		 * the memory subsystem
		 */
		// 准备新的GDT
		descriptor_t *gdt_new =
		    (descriptor_t *) malloc(GDT_ITEMS * sizeof(descriptor_t));
		if (!gdt_new)
			panic("Cannot allocate memory for GDT.");

		//  复制GDT
		memcpy(gdt_new, gdt, GDT_ITEMS * sizeof(descriptor_t));
		memsetb(&gdt_new[TSS_DES], sizeof(descriptor_t), 0);
		protected_ap_gdtr.limit = GDT_ITEMS * sizeof(descriptor_t);
		protected_ap_gdtr.base = KA2PA((uintptr_t) gdt_new);
        // 这里将gdtr变量的base改为 gdt_new 但是没有load gdtr。
        // 这样 ap在访问 gdtr变量时候访问的就是新的gdtr而不是原来的gdtr。
		gdtr.base = (uintptr_t) gdt_new;

		// 发送INIT IPI
		if (l_apic_send_init_ipi(ops->cpu_apic_id(i))) {
			/*
			 * There may be just one AP being initialized at
			 * the time. After it comes completely up, it is
			 * supposed to wake us up.
			 */
			if (semaphore_down_timeout(&ap_completion_semaphore, 1000000) != EOK) {
				log(LF_ARCH, LVL_NOTE, "%s: waiting for cpu%u "
				    "(APIC ID = %d) timed out", __FUNCTION__,
				    i, ops->cpu_apic_id(i));
			}
		} else
			log(LF_ARCH, LVL_ERROR, "INIT IPI for l_apic%d failed",
			    ops->cpu_apic_id(i));
	}
}

int smp_irq_to_pin(unsigned int irq)
{
	assert(ops != NULL);
	return ops->irq_to_pin(irq);
}

#endif /* CONFIG_SMP */

/** @}
 */
