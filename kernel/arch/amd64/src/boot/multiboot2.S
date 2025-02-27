/*
 * Copyright (c) 2011 Martin Decky
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

#include <abi/asmtool.h>
#include <arch/boot/boot.h>
#include <arch/mm/page.h>
#include <arch/pm.h>
#include <arch/cpuid.h>
#include <arch/cpu.h>
#include <genarch/multiboot/multiboot2.h>

#define START_STACK  (BOOT_OFFSET - BOOT_STACK_SIZE)
/*
	若系统使用UEFI启动，GRUB就会作为64位程序加载运行，再将内核的映像文件加载到内存中。
*/
.section K_TEXT_START, "ax"

.code32

.align 8
multiboot2_header_start:
	.long MULTIBOOT2_HEADER_MAGIC
	.long MULTIBOOT2_HEADER_ARCH_I386
	.long multiboot2_header_end - multiboot2_header_start
	.long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCH_I386 + (multiboot2_header_end - multiboot2_header_start))

	/* Information request tag */
	.align 8
	tag_info_req_start:
		.word MULTIBOOT2_TAG_INFO_REQ
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_info_req_end - tag_info_req_start
		.long MULTIBOOT2_TAG_CMDLINE
		.long MULTIBOOT2_TAG_MODULE
		.long MULTIBOOT2_TAG_MEMMAP
#ifdef CONFIG_FB
		.long MULTIBOOT2_TAG_FBINFO
#endif
	tag_info_req_end:

	/* Address tag */
	.align 8
	tag_address_start:
		.word MULTIBOOT2_TAG_ADDRESS
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_address_end - tag_address_start
		.long multiboot2_header_start
		.long unmapped_start
		.long 0
		.long 0
	tag_address_end:

	/* Entry address tag */
	.align 8
	tag_entry_address_start:
		.word MULTIBOOT2_TAG_ENTRY_ADDRESS
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_entry_address_end - tag_entry_address_start
		.long multiboot2_image_start
	tag_entry_address_end:

	/* Flags tag */
	.align 8
	tag_flags_start:
		.word MULTIBOOT2_TAG_FLAGS
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_flags_end - tag_flags_start
		.long MULTIBOOT2_FLAGS_CONSOLE
	tag_flags_end:

#ifdef CONFIG_FB
	/* Framebuffer tag */
	.align 8
	tag_framebuffer_start:
		.word MULTIBOOT2_TAG_FRAMEBUFFER
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_framebuffer_end - tag_framebuffer_start
		.long CONFIG_BFB_WIDTH
		.long CONFIG_BFB_HEIGHT
		.long CONFIG_BFB_BPP
	tag_framebuffer_end:
#endif

	/* Module alignment tag */
	.align 8
	tag_module_align_start:
		.word MULTIBOOT2_TAG_MODULE_ALIGN
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_module_align_end - tag_module_align_start
		.long 0
	tag_module_align_end:

	/* Tag terminator */
	.align 8
	tag_terminator_start:
		.word MULTIBOOT2_TAG_TERMINATOR
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_terminator_end - tag_terminator_start
	tag_terminator_end:
multiboot2_header_end:

//  START_STACK = BOOT_OFFSET - BOOT_STACK_SIZE 
//	BOOT_OFFSET     = 0x108000
//  AP_BOOT_OFFSET  = 0x008000
//  BOOT_STACK_SIZE = 0x000400
//  START_STACK = 0x108000 - 0x000400
SYMBOL(multiboot2_image_start)
	cli					// 清除中断标志（Clear Interrupt Flag）。这条指令会禁用中断，防止在初始化过程中被中断打断。
	cld					// 清除方向标志（Clear Direction Flag）。这条指令通常用于字符串操作指令，确保字符串操作是从低地址向高地址进行的。

	/* Initialize stack pointer */
	// 将START_STACK的值移动到esp寄存器中。esp是x86体系结构中的堆栈指针寄存器，用于指向当前栈顶的地址。
	movl $START_STACK, %esp

	/*
	 * Initialize 
	 * Global Descriptor Table and
	 * Interrupt Descriptor Table 
	 * registers
	 */
	/*
	 *	这里加载的是和multiboot的同一个bootstrap_gdtr和bootstrap_idtr
	 */
	lgdtl bootstrap_gdtr
	lidtl bootstrap_idtr

	/* Kernel data + stack */
	// $GDT_SELECTOR(KDATA_DES) 是一个宏，计算内核数据段的选择器。
	// 设置内核数据段和栈段的寄存器。通过加载GDT和IDT指针，
	// CPU可以访问相应的描述符表，从而正确地管理内存段和中断处理。
	movw $GDT_SELECTOR(KDATA_DES), %cx
	movw %cx, %es
	movw %cx, %ds
	movw %cx, %ss

	/*
	 * Simics seems to remove hidden part of GS on entering user mode
	 * when _visible_ part of GS does not point to user-mode segment.
	 * Simics（一种模拟器）在从内核模式进入用户模式时，可能会清除%gs寄存器的隐藏部分（即段描述符的高16位）。
	 * 为了避免这种情况，需要在进入用户模式之前，将用户数据段的选择器加载到%fs和%gs寄存器中。
	 */
	// 计算用户数据段的选择器
	// 将 %cx 寄存器中的值（用户数据段的选择器）移动到 %fs 寄存器中，设置 fs 段为用户数据段。
	// 将 %cx 寄存器中的值（用户数据段的选择器）移动到 %gs 寄存器中，设置 gs 段为用户数据段。
	movw $GDT_SELECTOR(UDATA_DES), %cx
	movw %cx, %fs
	movw %cx, %gs

	// jmpl 是一个长跳转指令，用于切换到一个新的代码段并跳转到指定的地址。
	// 通过 jmpl 指令，CPU切换到由 GDT_SELECTOR(KTEXT32_DES) 指定的代码段。这通常用于从当前的代码段切换到内核的32位代码段。
	// 跳转到 multiboot2_meeting_point 标签所指示的地址，继续执行后续的代码。
	// 在操作系统启动过程中，可能会使用长跳转来切换到不同的代码段，例如从启动代码段切换到内核代码段。
	// 切换到保护模式。
	jmpl $GDT_SELECTOR(KTEXT32_DES), $multiboot2_meeting_point
	multiboot2_meeting_point:

	/*
	 * Protected 32-bit. We want to reuse the code-seg descriptor,
	 * the Default operand size must not be 1 when entering long mode.
	 */
	
	/* Save multiboot arguments */
	// multiboot_eax 定义在 multiboot.S 中，为 .long 0
	// multiboot_ebx 定义在 multiboot.S 中，为 .long 0
	// 这两条指令将多引导（Multiboot）参数保存到内存中的变量 multiboot_eax 和 multiboot_ebx 中。
	// 这些参数是由引导加载器传递给内核的，包含了启动时的配置信息。
	movl %eax, multiboot_eax
	movl %ebx, multiboot_ebx

	// 检测CPU是否支持扩展的CPUID指令集
	// 将 INTEL_CPUID_EXTENDED 的值（即 0x80000000）加载到 eax 寄存器中。
	// 这是为了调用CPUID指令来检测CPU是否支持扩展的CPUID指令集。
	movl $(INTEL_CPUID_EXTENDED), %eax
	// CPUID指令会根据 eax 寄存器中的值返回CPU的相关信息。
	// 当 eax 为 0x80000000 时，CPUID指令会返回CPU支持的最大扩展CPUID输入值。
	cpuid
	// 比较 eax 寄存器的值与 INTEL_CPUID_EXTENDED 的值。
	// 如果 eax 的值大于 INTEL_CPUID_EXTENDED，则说明CPU支持扩展的CPUID指令集。
	cmp $(INTEL_CPUID_EXTENDED), %eax
	ja extended_cpuid_supported

		jmp pm_error_halt

	extended_cpuid_supported:
	// 将 AMD_CPUID_EXTENDED 的值（即 0x80000001）加载到 eax 寄存器中。
	// 这是为了调用CPUID指令来检测CPU是否支持长模式和非执行内存保护。
	movl $(AMD_CPUID_EXTENDED), %eax
	cpuid
	// 检查 edx 寄存器的第29位（即 AMD_EXT_LONG_MODE）是否被设置。
	// 如果该位被设置，则表示CPU支持长模式。
	bt $(AMD_EXT_LONG_MODE), %edx
	jc long_mode_supported

		jmp pm_error_halt

	long_mode_supported:
	// 检查 edx 寄存器的第20位（即 AMD_EXT_NOEXECUTE）是否被设置。
	// 如果该位被设置，则表示CPU支持非执行内存保护特性。
	bt $(AMD_EXT_NOEXECUTE), %edx
	jc noexecute_supported

		jmp pm_error_halt

	noexecute_supported:
	// 将 INTEL_CPUID_STANDARD 的值（即 0x00000001）加载到 eax 寄存器中。
	// 这是为了调用CPUID指令来检测CPU是否支持FXSAVE指令。
	movl $(INTEL_CPUID_STANDARD), %eax
	cpuid
	// 检查 edx 寄存器的第24位（即 INTEL_FXSAVE）是否被设置。
	// 如果该位被设置，则表示CPU支持FXSAVE指令。
	bt $(INTEL_FXSAVE), %edx
	jc fx_supported

		jmp pm_error_halt

	fx_supported:
	// 检查 edx 寄存器的第26位（即 INTEL_SSE2）是否被设置。
	// 如果该位被设置，则表示CPU支持SSE2指令集。
	bt $(INTEL_SSE2), %edx
	jc sse2_supported

		jmp pm_error_halt

	sse2_supported:

	/*
	 * Enable 64-bit page translation entries - CR4.PAE = 1.
	 * Paging is not enabled until after long mode is enabled.
	 */
	/*
	 * 这段代码是启用64位长模式的汇编代码片段。
	 */
	// 将控制寄存器CR4的值加载到EAX寄存器中。
	// CR4寄存器用于控制处理器的某些功能，如物理地址扩展（PAE）等。
	movl %cr4, %eax
	// 将EAX寄存器的值与CR4_PAE的值进行逻辑或操作。CR4_PAE是物理地址扩展的位掩码，设置此位可以启用物理地址扩展，允许处理器使用更多的物理地址空间。
	orl $CR4_PAE, %eax
	// 将EAX寄存器的值（即已设置物理地址扩展的CR4值）写回CR4寄存器，从而启用物理地址扩展。
	movl %eax, %cr4

	/* Set up paging tables */
	// 将页表基地址（ptl_0）加载到eax中。
	leal ptl_0, %eax
	// 将eax写入CR3中，从而设置页表基址。
	movl %eax, %cr3

	/* Enable long mode */
	// 将AMD_MSR_EFER的值加载到ECX寄存器中。
	// 将EFER寄存器的地址（即 0xc0000080）加载到ECX寄存器中。
	// ECX寄存器用于指定要访问的MSR寄存器地址。
	movl $AMD_MSR_EFER, %ecx
	// 读取MSR寄存器的值。读取的值被存储在EAX和EDX寄存器中。
	// 对于EFER寄存器，其值存储在EAX寄存器中。
	rdmsr                     /* read EFER */
	// 将EAX寄存器的值与AMD_LME的值进行逻辑或操作。
	// AMD_LME是长模式启用的位掩码，
	// 设置此位可以启用长模式。通过逻辑或操作，将EAX寄存器中的LME位设置为1。
	orl $AMD_LME, %eax        /* set LME = 1 */
	// 将EAX和EDX寄存器中的值写回MSR寄存器。由于EFER寄存器的值存储在EAX寄存器中，
	// 因此写回操作会将修改后的值（即已启用长模式的EFER值）写回EFER寄存器，从而启用长模式。
	wrmsr

	/* Enable paging to activate long mode (set CR0.PG = 1) */
	// 将控制寄存器CR0的值加载到EAX寄存器中。CR0寄存器用于控制处理器的某些功能，如分页等。
	movl %cr0, %eax
	// 将EAX寄存器的值与CR0_PG的值进行逻辑或操作。
	// CR0_PG是分页的位掩码，设置此位可以启用分页。
	orl $CR0_PG, %eax
	// 将EAX寄存器的值（即已设置分页的CR0值）写回CR0寄存器，
	// 从而启用分页并激活长模式。
	movl %eax, %cr0

	/* At this point we are in compatibility mode */
	// 这条指令是一个长跳转（far jump），用于切换到64位代码段。
	// GDT_SELECTOR(KTEXT_DES)是全局描述符表（GDT）中的一个选择符，
	// 指向64位代码段描述符。start64是64位代码的入口点。
	// 执行这条指令后，处理器将切换到64位长模式，并开始执行64位代码。
	jmpl $GDT_SELECTOR(KTEXT_DES), $start64

pm_error_halt:
	cli
	hlt1:
		hlt
		jmp hlt1

.code64
/*
 *  现在我们到了这里，64位模式下段寄存器只有 fs和 gs起作用。
 *  fs = $GDT_SELECTOR(UDATA_DES)
 *  gs = $GDT_SELECTOR(UDATA_DES)
 */
start64:

	/*
	 * Long mode.
	 */

	movq $(PA2KA(START_STACK)), %rsp

	/* Create the first stack frame */
	// 这里创建了第一个栈帧，为了方便开始调用函数。
	/*
	 *	--------------------
	 *  |					|
	 *  |					|
	 *  |		0			|	--------->  0xffffffff80107bf8 (rsp , rbp)
	 *  |					|	--------->  0xffffffff80107c00
	 *  |-------------------|	--------->  0xffffffff80108000
	 */
	pushq $0
	movq %rsp, %rbp

	/* Call amd64_pre_main(multiboot_eax, multiboot_ebx) */
	// 将 multiboot_eax 的值加载到EDI寄存器中。multiboot_eax 是一个变量，
	// 通常用于存储从引导加载器传递给内核的EAX寄存器的值。
	// EDI寄存器是第一个参数的寄存器。
	movl multiboot_eax, %edi
	// 将 multiboot_ebx 的值加载到ESI寄存器中。multiboot_ebx 是一个变量，
	// 通常用于存储从引导加载器传递给内核的EBX寄存器的值。
	// ESI寄存器是第二个参数的寄存器。
	movl multiboot_ebx, %esi

// 执行特定于 AMD64 架构的初始化操作
// 其中有两步：
//      1. 调用 multiboot_info_parse和multiboot2_info_parse来处理传入的关于硬件和启动参数的信息
//      2. 调用 memcpy将启动代码 从 0x108000 cp 到 0x8000 地址。为 AP 启动做准备。
#ifdef MEMORY_MODEL_large
	// 将 amd64_pre_main 函数的地址加载到RAX寄存器中。
	// movabsq 指令用于加载64位的绝对地址。
	movabsq $amd64_pre_main, %rax
	callq *%rax
#else
	callq amd64_pre_main
#endif

	/* Call main_bsp() */
#ifdef MEMORY_MODEL_large
	movabsq $main_bsp, %rax
	callq *%rax
#else
	callq main_bsp
#endif

	/* Not reached */
	cli
	hlt0:
		hlt
		jmp hlt0
