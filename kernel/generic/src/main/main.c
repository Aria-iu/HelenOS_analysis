/*
 * Copyright (c) 2001-2004 Jakub Jermar
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

/** @addtogroup kernel_generic
 * @{
 */

/**
 * @file
 * @brief Main initialization kernel function for all processors.
 *
 * During kernel boot, all processors, after architecture dependent
 * initialization, start executing code found in this file. After
 * bringing up all subsystems, control is passed to scheduler().
 *
 * The bootstrap processor starts executing main_bsp() while
 * the application processors start executing main_ap().
 *
 * @see scheduler()
 * @see main_bsp()
 * @see main_ap()
 */

#include <arch/asm.h>
#include <debug.h>
#include <context.h>
#include <stdio.h>
#include <panic.h>
#include <assert.h>
#include <config.h>
#include <time/clock.h>
#include <time/timeout.h>
#include <proc/scheduler.h>
#include <proc/thread.h>
#include <proc/task.h>
#include <main/kinit.h>
#include <main/version.h>
#include <console/kconsole.h>
#include <console/console.h>
#include <log.h>
#include <cpu.h>
#include <align.h>
#include <interrupt.h>
#include <str.h>
#include <mm/frame.h>
#include <mm/page.h>
#include <genarch/mm/page_pt.h>
#include <mm/km.h>
#include <mm/tlb.h>
#include <mm/as.h>
#include <mm/slab.h>
#include <mm/reserve.h>
#include <synch/waitq.h>
#include <synch/syswaitq.h>
#include <arch/arch.h>
#include <arch.h>
#include <ipc/ipc.h>
#include <macros.h>
#include <smp/smp.h>
#include <ddi/ddi.h>
#include <main/main.h>
#include <ipc/event.h>
#include <sysinfo/sysinfo.h>
#include <sysinfo/stats.h>
#include <lib/ra.h>
#include <cap/cap.h>

/*
 * Ensure [u]int*_t types are of correct size.
 *
 * Probably, this is not the best place for such tests
 * but this file is compiled on all architectures.
 */
#define CHECK_INT_TYPE_(signness, size) \
	static_assert(sizeof(signness##size##_t) * 8 == size, \
	    #signness #size "_t does not have " #size " bits");

#define CHECK_INT_TYPE(size) \
	CHECK_INT_TYPE_(int, size); \
	CHECK_INT_TYPE_(uint, size)

CHECK_INT_TYPE(8);
CHECK_INT_TYPE(16);
CHECK_INT_TYPE(32);
CHECK_INT_TYPE(64);

/** Global configuration structure. */
config_t config = {
	.identity_configured = false,
	.non_identity_configured = false,
	.physmem_end = 0
};

/** Boot arguments. */
char bargs[CONFIG_BOOT_ARGUMENTS_BUFLEN] = { };

/** Initial user-space tasks */
init_t init = {
	.cnt = 0
};

/** Boot allocations. */
ballocs_t ballocs = {
	.base = (uintptr_t) NULL,
	.size = 0
};

static context_t ctx;

// NOTE: All kernel stacks must be aligned to STACK_SIZE, see CURRENT.
// 8K大小。
static const size_t bootstrap_stack_size = STACK_SIZE;
// 内核的新栈。。。
static _Alignas(STACK_SIZE) uint8_t bootstrap_stack[STACK_SIZE];

/* Just a convenient value for some assembly code. */
uint8_t *const bootstrap_stack_top = bootstrap_stack + STACK_SIZE;

/*
 * These two functions prevent stack from underflowing during the
 * kernel boot phase when SP is set to the very top of the reserved
 * space. The stack could get corrupted by a fooled compiler-generated
 * pop sequence otherwise.
 */
static void main_bsp_separated_stack(void);

#ifdef CONFIG_SMP
static void main_ap_separated_stack(void);
#endif

/** Main kernel routine for bootstrap CPU.
 *
 * The code here still runs on the boot stack, which knows nothing about
 * preemption counts.  Because of that, this function cannot directly call
 * functions that disable or enable preemption (e.g. spinlock_lock()). The
 * primary task of this function is to calculate address of a new stack and
 * switch to it.
 *
 * Assuming interrupts_disable().
 *
 */
/*
	#define _NO_TRACE  __attribute__((no_instrument_function))
	宏定义 _NO_TRACE 使用了 GCC 的 __attribute__((no_instrument_function)) 属性。
	这个属性的作用是告诉编译器不要对指定的函数进行函数调用跟踪（instrumentation）。
	具体作用可见 /ZYC-Debug/Images/-finstrument-functions.png
	*/ 

_NO_TRACE void main_bsp(void)
{
	// 设置当前系统中的CPU数量为1。这通常在多处理器系统中会被更新。
	config.cpu_count = 1;
	// 设置当前活跃的CPU数量为1。这表示主核心已经启动并运行。
	config.cpu_active = 1;
	// 设置内核的基地址。kernel_load_address 是内核加载到内存中的起始地址。
	config.base = (uintptr_t) kernel_load_address;
	// 计算内核的大小。kdata_end 是内核数据段的结束地址，config.base 是内核的基地址。
	// ALIGN_UP 宏用于将计算结果向上对齐到页面大小（PAGE_SIZE）。
	// 暂时的_link.ld 中， kdata_end			=  FFFF FFFF 801A AF29
	//					   kernel_load_address	=  FFFF FFFF 8010 8000
	// PAGE_SIZE = (1 << 12)
	config.kernel_size =
	    ALIGN_UP((uintptr_t) kdata_end - config.base, PAGE_SIZE);

	context_create(&ctx, main_bsp_separated_stack,
	    bootstrap_stack, bootstrap_stack_size);
	context_restore(&ctx);
	// context_restore中，将新的任务的信息存到当前CPU上，由于是启动CPU，
	// 也就是说，唯一的CPU去执行 main_bsp_separated_stack 这个任务，那么
	// 当前函数就像是时光一样
	// 永远不会返回。。。。
	/* not reached */
}

/** Main kernel routine for bootstrap CPU using new stack.
 *
 * Second part of main_bsp().
 *
 */
void main_bsp_separated_stack(void)
{
	/* Keep this the first thing. */
	// 将当前栈的基地址处作为CURRENT的起始地址，方便直接使用CURRENT得到当前的current_t结构。
	current_initialize(CURRENT);
	// 这里使用了printf！！！
	version_print();
	// 最终调用dummy_printf
	LOG("\nconfig.base=%p config.kernel_size=%zu",
	    (void *) config.base, config.kernel_size);

	// AMD64架构 CONFIG_KCONSOLE 被定义了。
#ifdef CONFIG_KCONSOLE
	/*
	 * kconsole data structures must be initialized very early
	 * because other subsystems will register their respective
	 * commands.
	 */
	/*
	* 内核控制台初始化。
	*/
	kconsole_init();
#endif

	/*
	 * Exception handler initialization, before architecture
	 * starts adding its own handlers
	 */
	/*
	* 异常处理机制 init。
	* 
	*/
	exc_init();

	/*
	 * Memory management subsystems initialization.
	 * 内存管理子系统初始化
	 */
	// 这个宏在AMD64架构下调用 amd64_pre_mm_init
	ARCH_OP(pre_mm_init);
	// 将内核的内存进行直接映射。
	km_identity_init();
	// 物理页帧初始化。。
	frame_init();
	// zones_print_list();
	// slab 分配器初始化。
	slab_cache_init();
	// 初始化malloc分配器。malloc依赖slab分配器来分配内存。
	malloc_init();
	// 内存区域初始化，是在slab分配器的基础上创建了一个名为ra_segment_t的slab_cash
	// 用来分配 ra_segment_t 结构的内存。
	ra_init();
	// 在slab分配器的基础上创建了一个名为sysinfo_item_t的slab_cash
	// 用来分配 sysinfo_item_t的内存。
	sysinfo_init();
	as_init();
	page_init();
	tlb_init();
	km_non_identity_init();
	ddi_init();
	// 这个宏在AMD64架构下调用 amd64_pre_mm_init
	ARCH_OP(post_mm_init);
	reserve_init();
	// 对于AMD64架构来说，这里是调用 amd64_pre_smp_init
	// 其中会初始化几个驱动。。。
	ARCH_OP(pre_smp_init);
	smp_init();

	/* Slab must be initialized after we know the number of processors. */
	slab_enable_cpucache();

	uint64_t size;
	const char *size_suffix;
	bin_order_suffix(zones_total_size(), &size, &size_suffix, false);
	printf("Detected %u CPU(s), %" PRIu64 " %s free memory\n",
	    config.cpu_count, size, size_suffix);

	// cpu_init是内核.text段的开头。
	cpu_init();
	calibrate_delay_loop();
	ARCH_OP(post_cpu_init);

	clock_counter_init();
	timeout_init();
	scheduler_init();
	caps_init();
	task_init();
	thread_init();
	sys_waitq_init();

	sysinfo_set_item_data("boot_args", NULL, bargs, str_size(bargs) + 1);

	if (init.cnt > 0) {
		size_t i;
		for (i = 0; i < init.cnt; i++)
			LOG("init[%zu].addr=%p, init[%zu].size=%zu",
			    i, (void *) init.tasks[i].paddr, i, init.tasks[i].size);
	} else
		printf("No init binaries found.\n");

	ipc_init();
	event_init();
	kio_init();
	log_init();
	stats_init();

	/*
	 * Create kernel task.
	 */
	task_t *kernel = task_create(AS_KERNEL, "kernel");
	if (!kernel)
		panic("Cannot create kernel task.");

	/*
	 * Create the first thread.
	 */
	thread_t *kinit_thread = thread_create(kinit, NULL, kernel,
	    THREAD_FLAG_UNCOUNTED, "kinit");
	if (!kinit_thread)
		panic("Cannot create kinit thread.");
	thread_start(kinit_thread);
	thread_detach(kinit_thread);

	/*
	 * This call to scheduler_run() will return to kinit,
	 * starting the thread of kernel threads.
	 */
	current_copy(CURRENT, (current_t *) CPU_LOCAL->stack);
	context_replace(scheduler_run, CPU_LOCAL->stack, STACK_SIZE);
	/* not reached */
}

#ifdef CONFIG_SMP

/** Main kernel routine for application CPUs.
 *
 * Executed by application processors, temporary stack
 * is at ctx.sp which was set during BSP boot.
 * This function passes control directly to
 * main_ap_separated_stack().
 *
 * Assuming interrupts_disable()'d.
 *
 */
void main_ap(void)
{
	/*
	 * Incrementing the active CPU counter will guarantee that the
	 * *_init() functions can find out that they need to
	 * do initialization for AP only.
	 */
	config.cpu_active++;

	/*
	 * The CURRENT structure is well defined because ctx.sp is used as stack.
	 */
	current_initialize(CURRENT);

	ARCH_OP(pre_mm_init);
	frame_init();
	page_init();
	tlb_init();
	ARCH_OP(post_mm_init);

	cpu_init();
	calibrate_delay_loop();
	ARCH_OP(post_cpu_init);

	/*
	 * If we woke kmp up before we left the kernel stack, we could
	 * collide with another CPU coming up. To prevent this, we
	 * switch to this cpu's private stack prior to waking kmp up.
	 */
	current_copy(CURRENT, (current_t *) CPU_LOCAL->stack);
	context_replace(main_ap_separated_stack, CPU_LOCAL->stack, STACK_SIZE);
	/* not reached */
}

/** Main kernel routine for application CPUs using new stack.
 *
 * Second part of main_ap().
 *
 */
void main_ap_separated_stack(void)
{
	/*
	 * Configure timeouts for this cpu.
	 */
	timeout_init();

	semaphore_up(&ap_completion_semaphore);
	scheduler_run();
	/* not reached */
}

#endif /* CONFIG_SMP */

/** @}
 */
