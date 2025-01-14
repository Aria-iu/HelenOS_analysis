/*
 * Copyright (c) 2006 Ondrej Palkovsky
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

#include <syscall/syscall.h>
#include <arch/syscall.h>
#include <panic.h>
#include <arch/cpu.h>
#include <arch/pm.h>
#include <arch/asm.h>

extern void syscall_entry(void);

/** Enable & setup support for SYSCALL/SYSRET */
void syscall_setup_cpu(void)
{
	/* Enable SYSCALL/SYSRET */
	// 通过修改 EFER（Extended Feature Enable Register）来启用 SYSCALL 和 SYSRET 指令的支持。
	// AMD_SCE 是一个位掩码，用来启用 SYSCALL 和 SYSRET 功能。
	write_msr(AMD_MSR_EFER, read_msr(AMD_MSR_EFER) | AMD_SCE);

	/* Setup syscall entry address */

	/*
	 * This is _mess_ - the 64-bit CS is argument + 16,
	 * the SS is argument + 8. The order is:
	 * +0(KDATA_DES), +8(UDATA_DES), +16(UTEXT_DES)
	 */
	// 设置系统调用（SYSCALL）
	// 详细设计见 /ZYC-Debug/Images/SYSCALL_SYSRET*.png
	write_msr(AMD_MSR_STAR,
	    ((uint64_t) (GDT_SELECTOR(KDATA_DES) | PL_USER) << 48) |
	    ((uint64_t) (GDT_SELECTOR(KTEXT_DES) | PL_KERNEL) << 32));
	// 设置系统调用的入口地址
	write_msr(AMD_MSR_LSTAR, (uint64_t)syscall_entry);
	/*
	 * Mask RFLAGS on syscall
	 * - disable interrupts, until we exchange the stack register
	 *   (mask the IF bit)
	 * - clear DF so that the string instructions operate in
	 *   the right direction
	 * - clear NT to prevent a #GP should the flag proliferate to an IRET
	 * - clear TF to prevent an immediate #DB if TF is set
	 */
	// SFMASK 寄存器用来配置系统调用时的标志位掩码。
	// 通过设置以下标志，确保在进行系统调用时，CPU 会禁用某些影响指令执行的标志位
	write_msr(AMD_MSR_SFMASK,
	    RFLAGS_IF | RFLAGS_DF | RFLAGS_NT | RFLAGS_TF);
}

/** @}
 */
