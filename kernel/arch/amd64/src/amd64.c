/*
 * Copyright (c) 2005 Ondrej Palkovsky
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

/** @addtogroup kernel_amd64
 * @{
 */
/** @file
 */

#include <arch.h>
#include <arch/arch.h>
#include <stdint.h>
#include <errno.h>
#include <memw.h>
#include <interrupt.h>
#include <console/console.h>
#include <syscall/syscall.h>
#include <sysinfo/sysinfo.h>
#include <arch/bios/bios.h>
#include <arch/boot/boot.h>
#include <arch/drivers/i8254.h>
#include <arch/syscall.h>
#include <genarch/acpi/acpi.h>
#include <genarch/drivers/ega/ega.h>
#include <genarch/drivers/i8042/i8042.h>
#include <genarch/drivers/i8259/i8259.h>
#include <genarch/drivers/ns16550/ns16550.h>
#include <genarch/drivers/legacy/ia32/io.h>
#include <genarch/fb/bfb.h>
#include <genarch/kbrd/kbrd.h>
#include <genarch/srln/srln.h>
#include <genarch/multiboot/multiboot.h>
#include <genarch/multiboot/multiboot2.h>
#include <arch/pm.h>
#include <arch/vreg.h>
#include <arch/kseg.h>
#include <arch/mm/pat.h>
#include <genarch/pic/pic_ops.h>

#ifdef CONFIG_SMP
#include <arch/smp/apic.h>
#endif

static void amd64_pre_mm_init(void);
static void amd64_post_mm_init(void);
static void amd64_post_cpu_init(void);
static void amd64_pre_smp_init(void);
static void amd64_post_smp_init(void);

arch_ops_t amd64_ops = {
	.pre_mm_init = amd64_pre_mm_init,
	.post_mm_init = amd64_post_mm_init,
	.post_cpu_init = amd64_post_cpu_init,
	.pre_smp_init = amd64_pre_smp_init,
	.post_smp_init = amd64_post_smp_init
};

arch_ops_t *arch_ops = &amd64_ops;

/** Perform amd64-specific initialization before main_bsp() is called.
 *
 * @param signature Multiboot signature.
 * @param info      Multiboot information structure.
 *
 * signature是多引导签名，用于标识引导加载器传递的信息的类型。
 * info指向多引导信息结构的指针，包含了引导加载器传递的各种信息。
 */
void amd64_pre_main(uint32_t signature, void *info)
{
	/* Parse multiboot information obtained from the bootloader. */
	// 在这里解析multiboot2规范传来的信息。。。
	// AMD64使用的是multiboot2规范，所以multiboot_info_parse会直接返回，multiboot2_info_parse会被调用。
	multiboot_info_parse(signature, (multiboot_info_t *) info);
	multiboot2_info_parse(signature, (multiboot2_info_t *) info);
	
#ifdef CONFIG_SMP
	size_t unmapped_size = (uintptr_t) unmapped_end - BOOT_OFFSET;
	/* Copy AP bootstrap routines below 1 MB. */
    // 使用 memcpy 函数将从 BOOT_OFFSET 开始的 unmapped_size 字节的数据复制到 AP_BOOT_OFFSET 指定的位置。
    // AP_BOOT_OFFSET 通常位于1MB以下的内存区域（例如，0x008000）。
	memcpy((void *) AP_BOOT_OFFSET, (void *) BOOT_OFFSET, unmapped_size);
#endif
}

// 在main.c中的 main_bsp_separated_stack(void) 中被调用。
void amd64_pre_mm_init(void)
{
	/* Enable no-execute pages */
	// 将 MSR_EFER的 AMD_NXE 置位。
	// 设置这个位可以禁止执行某些内存区域的代码，增强系统的安全性。
	// 设置NXE位后，可以将某些内存区域标记为“不可执行”
	write_msr(AMD_MSR_EFER, read_msr(AMD_MSR_EFER) | AMD_NXE);
	/* Enable FPU */
	// 设置CPU以启用浮点运算单元（FPU）的。
	// 通过修改控制寄存器CR0和CR4来启用FPU的相关功能。
	cpu_setup_fpu();

	/* Initialize segmentation */
	pm_init();

	/* Disable I/O on nonprivileged levels, clear the nested-thread flag */
	// 通过清除 RFLAGS 寄存器中的 IOPL 位，确保只有特权级（如内核级）的代码可以执行 I/O 操作。
	// 通过清除 RFLAGS 寄存器中的 NT 位，确保系统不会使用嵌套任务切换机制。
	write_rflags(read_rflags() & ~(RFLAGS_IOPL | RFLAGS_NT));
	/* Disable alignment check */
	// 通过清除 CR0 寄存器中的 AM 位，禁用对齐检查。
	// 对齐检查用于确保数据访问操作的地址是按字节、字或双字对齐的。
	// 在某些情况下，禁用对齐检查可以提高性能，但可能会导致未对齐访问的性能问题。
	write_cr0(read_cr0() & ~CR0_AM);

	/* Use PCD+PWT bit combination in PTE to mean write-combining mode. */
	// 通过 pat_supported 函数检查 CPU 是否支持物理地址扩展（PAT）
	if (pat_supported())
		// 如果支持 PAT，通过 pat_set_mapping 函数设置页面表条目（PTE）的 
		// PCD（Page Cache Disable）和 PWT（Page Write Through）位，以启用写合并模式（Write-Combining）。
		pat_set_mapping(false, true, true, PAT_TYPE_WRITE_COMBINING);

	if (config.cpu_active == 1) {
		// 初始化中断处理机制
		interrupt_init();
		// 初始化 BIOS 相关的功能
		// 这里的bios实际上是通过 kernel/arch/amd64/src/bios文件找到ia32架构下的bios.c
		// Meson 是个很神奇的工具。
		bios_init();

		/* PIC */
		// 通过 i8259_init 函数初始化 PIC，
		// 设置主从 PIC 的基地址和中断向量基地址。这确保了系统能够正确处理外部中断请求。
		// IVT_IRQBASE =  32			中断向量基地址，通常为 32，表示 IRQ0 对应的中断向量号为 32。
		// I8259_PIC0_BASE = 0x20U		主 PIC 的基地址
		// I8259_PIC1_BASE = 0xA0U		从 PIC 的基地址
		i8259_init((i8259_t *) I8259_PIC0_BASE,
		    (i8259_t *) I8259_PIC1_BASE, IVT_IRQBASE);

		/* Set PIC operations. */
		// 通过 pic_ops = &i8259_pic_ops 设置 PIC 的操作接口，以便后续可以调用 PIC 的相关操作函数。
		pic_ops = &i8259_pic_ops;
	}
}

// ddi初始化之后调用。
void amd64_post_mm_init(void)
{
	// 每CPU虚拟寄存器初始化。
	vreg_init();
	// 每CPU kseg初始化，由GS寄存器访问。
	kseg_init();

	if (config.cpu_active == 1) {
		/* Initialize IRQ routing */
		// IRQ_COUNT = 16
		// 中断机制初始化，但是还没有开始注册外设中断或者定时器之类的中断。
		irq_init(IRQ_COUNT, IRQ_COUNT);

		/* hard clock */
		// 硬件计时器初始化。
		i8254_init();

#if (defined(CONFIG_FB) || defined(CONFIG_EGA))
		// 标记是否初始化了帧缓冲设备。
		// AMD64架构中设置了CONFIG_FB 和 CONFIG_EGA
		bool bfb = false;
#endif

#ifdef CONFIG_FB
		// 初始化帧缓冲设备。
		bfb = bfb_init();
#endif

#ifdef CONFIG_EGA
		// 如果没有设置帧缓冲设备的参数或者帧缓冲设备初始化失败，初始化ega设备。
		if (!bfb) {
			outdev_t *egadev = ega_init(EGA_BASE, EGA_VIDEORAM);
			if (egadev)
				stdout_wire(egadev);
		}
#endif

		/* Merge all memory zones to 1 big zone */
		// 合并所有可合并的内存区域（zones）为一个大的内存区域
		zone_merge_all();
	}

	/* Setup fast SYSCALL/SYSRET */
	// 设置和启用系统调用（SYSCALL/SYSRET）支持。通过配置CPU的MSR（Model-Specific Register）来实现。
	syscall_setup_cpu();
}

void amd64_post_cpu_init(void)
{
#ifdef CONFIG_SMP
	// 如果是多核系统
	if (config.cpu_active > 1) {
		// 初始化LOCAL APIC。
		l_apic_init();
		l_apic_debug();
	}
#endif
}

void amd64_pre_smp_init(void)
{
	if (config.cpu_active == 1) {
#ifdef CONFIG_SMP
		// 如果当前CPU时Boot CPU而且配置是多核。
		// 用于初始化 ACPI（高级配置和电源接口） 的相关信息
		acpi_init();
#endif /* CONFIG_SMP */
	}
}

void amd64_post_smp_init(void)
{
	/* Currently the only supported platform for amd64 is 'pc'. */
	static const char *platform = "pc";

	sysinfo_set_item_data("platform", NULL, (void *) platform,
	    str_size(platform));

#ifdef CONFIG_PC_KBD
	/*
	 * Initialize the i8042 controller. Then initialize the keyboard
	 * module and connect it to i8042. Enable keyboard interrupts.
	 */
	i8042_instance_t *i8042_instance = i8042_init((i8042_t *) I8042_BASE, IRQ_KBD);
	if (i8042_instance) {
		kbrd_instance_t *kbrd_instance = kbrd_init();
		if (kbrd_instance) {
			indev_t *sink = stdin_wire();
			indev_t *kbrd = kbrd_wire(kbrd_instance, sink);
			i8042_wire(i8042_instance, kbrd);
			pic_ops->enable_irqs(1 << IRQ_KBD);
			pic_ops->enable_irqs(1 << IRQ_MOUSE);
		}
	}
#endif

#if (defined(CONFIG_NS16550) || defined(CONFIG_NS16550_OUT))
	/*
	 * Initialize the ns16550 controller.
	 */
#ifdef CONFIG_NS16550_OUT
	outdev_t *ns16550_out;
	outdev_t **ns16550_out_ptr = &ns16550_out;
#else
	outdev_t **ns16550_out_ptr = NULL;
#endif
	ns16550_instance_t *ns16550_instance =
	    ns16550_init(NS16550_BASE, 0, IRQ_NS16550, NULL, NULL,
	    ns16550_out_ptr);
	if (ns16550_instance) {
		ns16550_format_set(ns16550_instance, 38400,
		    LCR_PARITY_NONE | LCR_STOP_BIT_TWO | LCR_WORD_LEN_8);
#ifdef CONFIG_NS16550
		srln_instance_t *srln_instance = srln_init();
		if (srln_instance) {
			indev_t *sink = stdin_wire();
			indev_t *srln = srln_wire(srln_instance, sink);
			ns16550_wire(ns16550_instance, srln);
			pic_ops->enable_irqs(1 << IRQ_NS16550);
		}
#endif
#ifdef CONFIG_NS16550_OUT
		if (ns16550_out) {
			stdout_wire(ns16550_out);
		}
#endif
	}
#endif

	sysinfo_set_item_val(pic_ops->get_name(), NULL, true);
}

void calibrate_delay_loop(void)
{
	// 用来校准延迟循环的精确度。
	// 设置了当前CPU的delay_loop_const和 frequency_mhz字段。
	i8254_calibrate_delay_loop();
	if (config.cpu_active == 1) {
		/*
		 * This has to be done only on UP.
		 * On SMP, i8254 is not used for time keeping and its interrupt pin remains masked.
		 */
		// 启用i8254时钟中断。
		i8254_normal_operation();
	}
}

/** Construct function pointer
 *
 * @param fptr   function pointer structure
 * @param addr   function address
 * @param caller calling function address
 *
 * @return address of the function pointer
 *
 */
void *arch_construct_function(fncptr_t *fptr, void *addr, void *caller)
{
	return addr;
}

void arch_reboot(void)
{
#ifdef CONFIG_PC_KBD
	i8042_cpu_reset((i8042_t *) I8042_BASE);
#endif
}

void irq_initialize_arch(irq_t *irq)
{
	(void) irq;
}

/** @}
 */
