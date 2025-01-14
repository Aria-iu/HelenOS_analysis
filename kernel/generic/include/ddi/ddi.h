/*
 * Copyright (c) 2006 Jakub Jermar
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

/** @addtogroup kernel_generic_ddi
 * @{
 */
/** @file
 */

#ifndef KERN_DDI_H_
#define KERN_DDI_H_

#include <typedefs.h>
#include <abi/ddi/arg.h>
#include <proc/task.h>
#include <adt/odict.h>

/** Structure representing contiguous physical memory area. */
// 表示一个连续的物理内存区域。它用于管理一块物理内存区域，并且提供了额外的信息，
// 如该区域是否映射、是否允许普通任务访问（unpriv），以及内存区域映射状态改变时的回调函数。
typedef struct parea {
	/** Link to @c pareas ordered dictionary */
	odlink_t lpareas;

	/** Physical base of the area. */
	uintptr_t pbase;
	/** Number of frames in the area. */
	pfn_t frames;
	/** Allow mapping by unprivileged tasks. */
	bool unpriv;
	/** Indicate whether the area is actually mapped. */
	bool mapped;
	/** Called when @c mapped field has changed */
	void (*mapped_changed)(void *);
	/** Callback argument */
	void *arg;
} parea_t;

extern void ddi_init(void);
// 初始化和注册一个 parea 结构，确保物理内存区域在内核中正确注册并且可以被操作。
extern void ddi_parea_init(parea_t *);
extern void ddi_parea_register(parea_t *);
// 用于通知某个物理内存区域的映射状态发生变化。
extern void ddi_parea_unmap_notify(parea_t *);

// 将物理地址映射到虚拟地址空间或解除映射。
// 这个功能通常在设备驱动中用来访问硬件的 I/O 端口或设备寄存器。
extern void *pio_map(void *, size_t);
extern void pio_unmap(void *, void *, size_t);

// 提供了系统调用接口，允许用户空间程序请求对物理内存的映射操作。
extern sys_errno_t sys_physmem_map(uintptr_t, size_t, unsigned int, uspace_ptr_uintptr_t,
    uintptr_t);
extern sys_errno_t sys_physmem_unmap(uintptr_t);

// 用来进行 DMA 映射的系统调用。
extern sys_errno_t sys_dmamem_map(size_t, unsigned int, unsigned int, uspace_ptr_uintptr_t,
    uspace_ptr_uintptr_t, uintptr_t);
extern sys_errno_t sys_dmamem_unmap(uintptr_t, size_t, unsigned int);

// 用于启用或禁用 I/O 空间。
extern sys_errno_t sys_iospace_enable(uspace_ptr_ddi_ioarg_t);
extern sys_errno_t sys_iospace_disable(uspace_ptr_ddi_ioarg_t);

/*
 * Interface to be implemented by all architectures.
 */
// 特定于架构的实现，负责在特定的硬件架构上启用或禁用 I/O 空间。
extern errno_t ddi_iospace_enable_arch(task_t *, uintptr_t, size_t);
extern errno_t ddi_iospace_disable_arch(task_t *, uintptr_t, size_t);

#endif

/** @}
 */
