/*
 * Copyright (c) 2010 Jakub Jermar
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

/** @addtogroup kernel_generic_proc
 * @{
 */
/** @file
 */

#ifndef KERN_TASK_H_
#define KERN_TASK_H_

#include <abi/proc/task.h>
#include <abi/sysinfo.h>
#include <adt/list.h>
#include <adt/odict.h>
#include <arch/context.h>
#include <arch/cpu.h>
#include <arch/fpu_context.h>
#include <arch/proc/task.h>
#include <arch/proc/thread.h>
#include <arch.h>
#include <cap/cap.h>
#include <cpu.h>
#include <debug/sections.h>
#include <ipc/event.h>
#include <ipc/ipc.h>
#include <ipc/kbox.h>
#include <mm/as.h>
#include <mm/tlb.h>
#include <proc/scheduler.h>
#include <security/perm.h>
#include <synch/mutex.h>
#include <synch/spinlock.h>
#include <udebug/udebug.h>

#define TASK                 CURRENT->task

struct thread;
struct cap;

/** Task structure. */
// 一个 任务的结构描述
typedef struct task {
	/** Link to @c tasks ordered dictionary */
	// 指向tasks字典的链接。
	odlink_t ltasks;

	/** Task lock.
	 *
	 * Must be acquired before threads_lock and thread lock of any of its
	 * threads.
	 */
	// 锁。
	IRQ_SPINLOCK_DECLARE(lock);

	// 名字。
	char name[TASK_NAME_BUFLEN];
	/** List of threads contained in this task. */
	// 线程列表
	list_t threads;
	/** Address space. */
	// 地址空间
	as_t *as;
	/** Unique identity of task. */
	// 任务 ID
	task_id_t taskid;
	/** Task security container. */
	// 任务容器id？？？？
	container_id_t container;

	/** Number of references (i.e. threads). */
	// 引用计数，管理任务的生命周期
	atomic_refcount_t refcount;
	/** Number of threads that haven't exited yet. */
	// TODO: remove
	// 还没有退出的线程数量
	atomic_size_t lifecount;

	/** Task permissions. */
	// 任务权限
	perm_t perms;

	/** Capabilities */
	// 能力信息。
	cap_info_t *cap_info;

	/* IPC stuff */

	/** Receiving communication endpoint */
	// IPC接收端。
	answerbox_t answerbox;

	/** Spinlock protecting the active_calls list. */
	// 锁，保护 active_calls
	SPINLOCK_DECLARE(active_calls_lock);

	/**
	 * List of all calls sent by this task that have not yet been
	 * answered.
	 */
	// 任务当前活跃的呼叫列表，存储所有该任务发起但尚未完成的IPC调用
	list_t active_calls;

	// 事件数组（events），它可以存储多个不同类型的事件（如任务结束等），用于异步事件通知。
	event_t events[EVENT_TASK_END - EVENT_END];

	/** IPC statistics */
	// IPC统计信息，记录该任务的进程间通信情况
	stats_ipc_t ipc_info;

#ifdef CONFIG_UDEBUG
	/** Debugging stuff. */
	udebug_task_t udebug;

	/** Kernel answerbox. */
	kbox_t kb;
#endif /* CONFIG_UDEBUG */

	/** Architecture specific task data. */
	// 架构特定的数据，通常包含与任务相关的CPU上下文等信息
	// AMD64架构中包含 iomapver 和 iomap 信息。
	task_arch_t arch;

	/** Accumulated accounting. */
	// 用户态和内核态的累计周期数，用于任务性能统计。
	uint64_t ucycles;
	uint64_t kcycles;

	debug_sections_t *debug_sections;
} task_t;

/** Synchronize access to @c tasks */
IRQ_SPINLOCK_EXTERN(tasks_lock);
/** Ordered dictionary of all tasks by ID (of task_t structures) */
extern odict_t tasks;

extern void task_init(void);
extern void task_done(void);
extern task_t *task_create(as_t *, const char *);
extern void task_hold(task_t *);
extern void task_release(task_t *);
extern task_t *task_find_by_id(task_id_t);
extern size_t task_count(void);
extern task_t *task_first(void);
extern task_t *task_next(task_t *);
extern errno_t task_kill(task_id_t);
extern void task_kill_self(bool) __attribute__((noreturn));
extern void task_get_accounting(task_t *, uint64_t *, uint64_t *);
extern void task_print_list(bool);

extern void perm_set(task_t *, perm_t);
extern perm_t perm_get(task_t *);

#ifndef task_create_arch
extern void task_create_arch(task_t *);
#endif

#ifndef task_destroy_arch
extern void task_destroy_arch(task_t *);
#endif

#ifdef __32_BITS__
extern sys_errno_t sys_task_get_id(uspace_ptr_sysarg64_t);
#endif

#ifdef __64_BITS__
extern sysarg_t sys_task_get_id(void);
#endif

extern sys_errno_t sys_task_set_name(uspace_ptr_const_char, size_t);
extern sys_errno_t sys_task_kill(uspace_ptr_task_id_t);
extern sys_errno_t sys_task_exit(sysarg_t);

#endif

/** @}
 */
