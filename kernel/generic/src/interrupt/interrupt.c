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

/** @addtogroup kernel_generic_interrupt
 * @{
 */
/**
 * @file
 * @brief Interrupt redirector.
 *
 * This file provides means of registering interrupt handlers
 * by kernel functions and calling the handlers when interrupts
 * occur.
 *
 */

#include <assert.h>
#include <interrupt.h>
#include <console/kconsole.h>
#include <console/console.h>
#include <console/cmd.h>
#include <synch/mutex.h>
#include <time/delay.h>
#include <macros.h>
#include <panic.h>
#include <stdio.h>
#include <stdarg.h>
#include <symtab.h>
#include <proc/thread.h>
#include <arch/cycle.h>
#include <arch/stack.h>
#include <str.h>
#include <trace.h>

/*
 * If IVT_ITEMS is zero (e.g. for special/abs32le) we hide completely any
 * access to the exception table array and panic if the function is called
 * at all. It also silences (correct) compiler warnings about possible
 * out-of-bound array access.
 */

// 异常处理表
exc_table_t exc_table[IVT_ITEMS];
IRQ_SPINLOCK_INITIALIZE(exctbl_lock);

/** Register exception handler
 *
 * @param n       Exception number.
 * @param name    Description.
 * @param hot     Whether the exception is actually handled
 *                in any meaningful way.
 * @param handler New exception handler.
 *
 * @return Previously registered exception handler.
 *
 */
// 注册异常处理函数handler，返回之前的异常处理函数。
iroutine_t exc_register(unsigned int n, const char *name, bool hot,
    iroutine_t handler)
{
#if (IVT_ITEMS > 0)
	// 注册异常的位置要小于IVT_ITEMS。
	assert(n < IVT_ITEMS);
	
	irq_spinlock_lock(&exctbl_lock, true);

	iroutine_t old = exc_table[n].handler;
	// 注册新的 handler ， name ， hot 
	exc_table[n].handler = handler;
	exc_table[n].name = name;
	exc_table[n].hot = hot;
	// 将该项的cycles和count设置为 0 
	exc_table[n].cycles = 0;
	exc_table[n].count = 0;

	irq_spinlock_unlock(&exctbl_lock, true);

	return old;
#else
	panic("No space for any exception handler, cannot register.");
#endif
}

/** Dispatch exception according to exception table
 *
 * Called directly from the assembler code.
 * CPU is interrupts_disable()'d.
 *
 */
_NO_TRACE void exc_dispatch(unsigned int n, istate_t *istate)
{
#if (IVT_ITEMS > 0)
	assert(n < IVT_ITEMS);

	/* Account user cycles */
	if (THREAD)
		thread_update_accounting(true);

	/* Account CPU usage if it woke up from sleep */
	if (CPU && CPU_LOCAL->idle) {
		uint64_t now = get_cycle();
		atomic_time_increment(&CPU->idle_cycles, now - CPU_LOCAL->last_cycle);
		CPU_LOCAL->last_cycle = now;
		CPU_LOCAL->idle = false;
	}

	uint64_t begin_cycle = get_cycle();

#ifdef CONFIG_UDEBUG
	if (THREAD)
		THREAD->udebug.uspace_state = istate;
#endif
	// 不论 int 几 指令，异常处理流程都会在 exc_table 定义的 handler 里面
	exc_table[n].handler(n + IVT_FIRST, istate);

#ifdef CONFIG_UDEBUG
	if (THREAD)
		THREAD->udebug.uspace_state = NULL;
#endif

	/* This is a safe place to exit exiting thread */
	if ((THREAD) && (THREAD->interrupted) && (istate_from_uspace(istate)))
		thread_exit();

	/* Account exception handling */
	uint64_t end_cycle = get_cycle();

	irq_spinlock_lock(&exctbl_lock, false);
	exc_table[n].cycles += end_cycle - begin_cycle;
	exc_table[n].count++;
	irq_spinlock_unlock(&exctbl_lock, false);

	/* Do not charge THREAD for exception cycles */
	if (THREAD)
		THREAD->last_cycle = end_cycle;
#else
	panic("No space for any exception handler, yet we want to handle some exception.");
#endif
}

/** Default 'null' exception handler
 *	默认的异常处理。
 */
_NO_TRACE static void exc_undef(unsigned int n, istate_t *istate)
{
	// 如果异常来自用户空间，则调用fault_from_uspace函数处理。
	fault_if_from_uspace(istate, "Unhandled exception %u.", n);
	// 如果异常来自内核空间，则触发内核恐慌。
	panic_badtrap(istate, n, "Unhandled exception %u.", n);
}

static _NO_TRACE void
fault_from_uspace_core(istate_t *istate, const char *fmt, va_list args)
{
	printf("Task %s (%" PRIu64 ") killed due to an exception at "
	    "program counter %p.\n", TASK->name, TASK->taskid,
	    (void *) istate_get_pc(istate));

	istate_decode(istate);
	stack_trace_istate(istate);

	printf("Kill message: ");
	vprintf(fmt, args);
	printf("\n");

	task_kill_self(true);
}

/** Terminate thread and task after the exception came from userspace.
 *
 */
_NO_TRACE void fault_from_uspace(istate_t *istate, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fault_from_uspace_core(istate, fmt, args);
	va_end(args);
}

/** Terminate thread and task if exception came from userspace.
 *
 */
_NO_TRACE void fault_if_from_uspace(istate_t *istate, const char *fmt, ...)
{
	if (!istate_from_uspace(istate))
		return;

	va_list args;
	va_start(args, fmt);
	fault_from_uspace_core(istate, fmt, args);
	va_end(args);
}

/** Get istate structure of a thread.
 *
 * Get pointer to the istate structure at the bottom of the kernel stack.
 *
 * This function can be called in interrupt or user context. In interrupt
 * context the istate structure is created by the low-level exception
 * handler. In user context the istate structure is created by the
 * low-level syscall handler.
 */
istate_t *istate_get(thread_t *thread)
{
	/*
	 * The istate structure should be right at the bottom of the kernel
	 * memory stack.
	 */
	return (istate_t *) &thread->kstack[MEM_STACK_SIZE - sizeof(istate_t)];
}

#ifdef CONFIG_KCONSOLE

static char flag_buf[MAX_CMDLINE + 1];

/** Print all exceptions
 *
 */
_NO_TRACE static int cmd_exc_print(cmd_arg_t *argv)
{
	bool excs_all;

	if (str_cmp(flag_buf, "-a") == 0)
		excs_all = true;
	else if (str_cmp(flag_buf, "") == 0)
		excs_all = false;
	else {
		printf("Unknown argument \"%s\".\n", flag_buf);
		return 1;
	}

#if (IVT_ITEMS > 0)
	unsigned int i;
	unsigned int rows;

	irq_spinlock_lock(&exctbl_lock, true);

#ifdef __32_BITS__
	printf("[exc   ] [description       ] [count   ] [cycles  ]"
	    " [handler ] [symbol\n");
	rows = 1;
#endif

#ifdef __64_BITS__
	printf("[exc   ] [description       ] [count   ] [cycles  ]"
	    " [handler         ]\n");
	printf("         [symbol\n");
	rows = 2;
#endif

	for (i = 0; i < IVT_ITEMS; i++) {
		if ((!excs_all) && (!exc_table[i].hot))
			continue;

		uint64_t count;
		char count_suffix;

		order_suffix(exc_table[i].count, &count, &count_suffix);

		uint64_t cycles;
		char cycles_suffix;

		order_suffix(exc_table[i].cycles, &cycles, &cycles_suffix);

		const char *symbol =
		    symtab_fmt_name_lookup((sysarg_t) exc_table[i].handler);

#ifdef __32_BITS__
		printf("%-8u %-20s %9" PRIu64 "%c %9" PRIu64 "%c %10p %s\n",
		    i + IVT_FIRST, exc_table[i].name, count, count_suffix,
		    cycles, cycles_suffix, exc_table[i].handler, symbol);

		PAGING(rows, 1, irq_spinlock_unlock(&exctbl_lock, true),
		    irq_spinlock_lock(&exctbl_lock, true));
#endif

#ifdef __64_BITS__
		printf("%-8u %-20s %9" PRIu64 "%c %9" PRIu64 "%c %18p\n",
		    i + IVT_FIRST, exc_table[i].name, count, count_suffix,
		    cycles, cycles_suffix, exc_table[i].handler);
		printf("         %s\n", symbol);

		PAGING(rows, 2, irq_spinlock_unlock(&exctbl_lock, true),
		    irq_spinlock_lock(&exctbl_lock, true));
#endif
	}

	irq_spinlock_unlock(&exctbl_lock, true);
#else /* (IVT_ITEMS > 0) */

	printf("No exception table%s.\n", excs_all ? " (showing all exceptions)" : "");

#endif /* (IVT_ITEMS > 0) */

	return 1;
}

static cmd_arg_t exc_argv = {
	.type = ARG_TYPE_STRING_OPTIONAL,
	.buffer = flag_buf,
	.len = sizeof(flag_buf)
};

static cmd_info_t exc_info = {
	.name = "exc",
	.description = "Print exception table (use -a for all exceptions).",
	.func = cmd_exc_print,
	.help = NULL,
	.argc = 1,
	.argv = &exc_argv
};

#endif /* CONFIG_KCONSOLE */

/** Initialize generic exception handling support
 *
 */
void exc_init(void)
{
	// 空操作：可以确保 exc_undef不会被优化掉。
	(void) exc_undef;
	// IVT是中断向量表。
	// AMD64架构 IVT_ITEMS = 64.
#if (IVT_ITEMS > 0)
	unsigned int i;
	// 将每个中断向量表项注册为未定义异常处理函数exc_undef。其中0-31是exc，后面是int
	for (i = 0; i < IVT_ITEMS; i++)
		exc_register(i, "undef", false, (iroutine_t) exc_undef);
#endif
	// 初始化完异常向量表后
	// 若启用了KCONSOLE
#ifdef CONFIG_KCONSOLE
	// 初始化命令 exc_info
	cmd_initialize(&exc_info);
	// 注册命令 exc_info
	if (!cmd_register(&exc_info))
		printf("Cannot register command %s\n", exc_info.name);
#endif
}

/** @}
 */
