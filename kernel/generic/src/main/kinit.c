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
 * @brief Kernel initialization thread.
 *
 * This file contains kinit kernel thread which carries out
 * high level system initialization.
 *
 * This file is responsible for finishing SMP configuration
 * and creation of userspace init tasks.
 */

#include <assert.h>
#include <main/kinit.h>
#include <config.h>
#include <arch.h>
#include <proc/scheduler.h>
#include <proc/task.h>
#include <proc/thread.h>
#include <proc/program.h>
#include <panic.h>
#include <halt.h>
#include <cpu.h>
#include <arch/asm.h>
#include <mm/page.h>
#include <arch/mm/page.h>
#include <mm/as.h>
#include <mm/frame.h>
#include <mm/km.h>
#include <stdio.h>
#include <log.h>
#include <memw.h>
#include <console/console.h>
#include <interrupt.h>
#include <console/kconsole.h>
#include <security/perm.h>
#include <lib/rd.h>
#include <ipc/ipc.h>
#include <str.h>
#include <str_error.h>
#include <sysinfo/stats.h>
#include <sysinfo/sysinfo.h>
#include <align.h>
#include <stdlib.h>
#include <debug/register.h>

#ifdef CONFIG_SMP
#include <smp/smp.h>
#endif /* CONFIG_SMP */

#include <synch/waitq.h>
#include <synch/spinlock.h>

#define ALIVE_CHARS  4

#ifdef CONFIG_KCONSOLE
static char alive[ALIVE_CHARS] = "-\\|/";
#endif

#define INIT_PREFIX      "init:"
#define INIT_PREFIX_LEN  5

/** Kernel initialization thread.
 *
 * kinit takes care of higher level kernel
 * initialization (i.e. thread creation,
 * userspace initialization etc.).
 *
 * @param arg Not used.
 */
/**
* Attention Plz！！！
* 这里第一个内核级别的线程。。kinit
* kinit处理高级别内核初始化，如线程创建，用户空间初始化。
* 
* 可以说 kinit 是内核的灵魂，一系列初始化之后，第一个内核线程，
* 它的任务十分重大，不仅要拉起其他AP，创建更多辅助性内核线程
* 更重要的是它需要执行 init 的任务。
*/
void kinit(void *arg)
{
	thread_t *thread;
	// 鍏抽棴涓柇銆?
	interrupts_disable();

#ifdef CONFIG_SMP
	if (config.cpu_count > 1) {
		semaphore_initialize(&ap_completion_semaphore, 0);

		/*
		 * Create the kmp thread and wait for its completion.
		 * cpu1 through cpuN-1 will come up consecutively and
		 * not mess together with kcpulb threads.
		 * Just a beautification.
		 */
		// BSP创建一个新的线程kmp
		// kmp用来启动AP。BSP等待kmp执行完
		// 在kmp中，会初始化BSP的Local APIC，而 AP的Local APIC 在
		// amd64_post_cpu_init中初始化。
		thread = thread_create(kmp, NULL, TASK,
		    THREAD_FLAG_UNCOUNTED, "kmp");
		if (!thread)
			panic("Unable to create kmp thread.");
		// kmp鏄湪BSP锛屼篃灏辨槸cpu[0]涓婃墽琛屻�?
		thread_wire(thread, &cpus[0]);
		thread_start(thread);
		thread_join(thread);

		/*
		 * For each CPU, create its load balancing thread.
		 */
		unsigned int i;

		for (i = 0; i < config.cpu_count; i++) {
			thread = thread_create(kcpulb, NULL, TASK,
			    THREAD_FLAG_UNCOUNTED, "kcpulb");
			if (thread != NULL) {
				// 鍚勪釜cpu鎵ц鑷繁鐨刱cpulb绾跨▼銆?
				thread_wire(thread, &cpus[i]);
				thread_start(thread);
				thread_detach(thread);
			} else
				log(LF_OTHER, LVL_ERROR,
				    "Unable to create kcpulb thread for cpu%u", i);
		}
	}
#endif /* CONFIG_SMP */

	/*
	 * At this point SMP, if present, is configured.
	 */
	// 缁堜簬鍒颁簡鏋舵瀯鐨勭粓鐐癸紝amd64_post_smp_init
	// 鍏朵腑鍒濆鍖栦簡閿洏鍜岄紶鏍囩殑涓柇
	// 鍒濆鍖杗s16550杩欎釜涓插彛
	ARCH_OP(post_smp_init);

	/* Start thread computing system load */
	// 鐢ㄤ簬璁＄畻绯荤粺璐熻浇鐨勭嚎绋?kload
	thread = thread_create(kload, NULL, TASK, THREAD_FLAG_NONE,
	    "kload");
	if (thread != NULL) {
		thread_start(thread);
		thread_detach(thread);
	} else {
		log(LF_OTHER, LVL_ERROR, "Unable to create kload thread");
	}

#ifdef CONFIG_KCONSOLE
	if (stdin) {
		/*
		 * Create kernel console.
		 */
		// 鍒涘缓kconsole_thread绾跨▼锛岄渶瑕佷娇鐢ㄥ埌stdin銆?
		thread = thread_create(kconsole_thread, NULL, TASK,
		    THREAD_FLAG_NONE, "kconsole");
		if (thread != NULL) {
			thread_start(thread);
			thread_detach(thread);
		} else {
			log(LF_OTHER, LVL_ERROR,
			    "Unable to create kconsole thread");
		}
	}
#endif /* CONFIG_KCONSOLE */

	/*
	 * Store the default stack size in sysinfo so that uspace can create
	 * stack with this default size.
	 */
	// 璁剧疆绯荤粺淇℃伅 default.stack_size 涓?STACK_SIZE_USER锛?024*1024锛?
	sysinfo_set_item_val("default.stack_size", NULL, STACK_SIZE_USER);


	// 开启中断
	interrupts_enable();

	/*
	 * Create user tasks, load RAM disk images.
	 */
	size_t i;
	program_t programs[CONFIG_INIT_TASKS] = { };

	// FIXME: do not propagate arguments through sysinfo
	// but pass them directly to the tasks
	for (i = 0; i < init.cnt; i++) {
		const char *arguments = init.tasks[i].arguments;
		if (str_length(arguments) == 0)
			continue;
		if (str_length(init.tasks[i].name) == 0)
			continue;
		size_t arguments_size = str_size(arguments);

		void *arguments_copy = malloc(arguments_size);
		if (arguments_copy == NULL)
			continue;
		memcpy(arguments_copy, arguments, arguments_size);

		char item_name[CONFIG_TASK_NAME_BUFLEN + 15];
		snprintf(item_name, CONFIG_TASK_NAME_BUFLEN + 15,
		    "init_args.%s", init.tasks[i].name);

		sysinfo_set_item_data(item_name, NULL, arguments_copy, arguments_size);
	}

	for (i = 0; i < init.cnt; i++) {
		if (init.tasks[i].paddr % FRAME_SIZE) {
			log(LF_OTHER, LVL_ERROR,
			    "init[%zu]: Address is not frame aligned", i);
			programs[i].task = NULL;
			continue;
		}

		/*
		 * Construct task name from the 'init:' prefix and the
		 * name stored in the init structure (if any).
		 */

		char namebuf[TASK_NAME_BUFLEN];

		const char *name = init.tasks[i].name;
		if (name[0] == 0)
			name = "<unknown>";

		static_assert(TASK_NAME_BUFLEN >= INIT_PREFIX_LEN, "");
		str_cpy(namebuf, TASK_NAME_BUFLEN, INIT_PREFIX);
		str_cpy(namebuf + INIT_PREFIX_LEN,
		    TASK_NAME_BUFLEN - INIT_PREFIX_LEN, name);

		/*
		 * Create virtual memory mappings for init task images.
		 */
		uintptr_t page = km_map(init.tasks[i].paddr,
		    init.tasks[i].size, PAGE_SIZE,
		    PAGE_READ | PAGE_WRITE | PAGE_CACHEABLE);
		assert(page);

		if (str_cmp(name, "kernel.dbg") == 0) {
			/*
			 * Not an actual init task, but rather debug sections extracted
			 * from the kernel ELF file and handed to us here so we can use
			 * it for debugging.
			 */

			register_debug_data((void *) page, init.tasks[i].size);
			programs[i].task = NULL;
			continue;
		}

		if (str_cmp(name, "loader") == 0) {
			/* Register image as the program loader */
			if (program_loader == NULL) {
				program_loader = (void *) page;
				log(LF_OTHER, LVL_NOTE, "Program loader at %p",
				    program_loader);
			} else {
				log(LF_OTHER, LVL_ERROR,
				    "init[%zu]: Second binary named \"loader\""
				    " present.", i);
			}

			programs[i].task = NULL;
			continue;
		}

		errno_t rc = program_create_from_image((void *) page, init.tasks[i].size, namebuf,
		    &programs[i]);

		if (rc == 0) {
			assert(programs[i].task != NULL);

			/*
			 * Set permissions to init userspace tasks.
			 */
			perm_set(programs[i].task,
			    PERM_PERM | PERM_MEM_MANAGER |
			    PERM_IO_MANAGER | PERM_IRQ_REG);

			if (!ipc_box_0) {
				ipc_box_0 = &programs[i].task->answerbox;
				/*
				 * Hold the first task so that
				 * ipc_box_0 remains a valid pointer
				 * even if the first task exits for
				 * whatever reason.
				 */
				task_hold(programs[i].task);
			}

		} else if (str_cmp(name, "initrd.img") == 0) {
			init_rd((void *) init.tasks[i].paddr, init.tasks[i].size);
		} else {
			log(LF_OTHER, LVL_ERROR,
			    "init[%zu]: Init binary load failed "
			    "(error %s, loader status %s)", i,
			    str_error_name(rc), str_error_name(programs[i].loader_status));
		}
	}

	/*
	 * Run user tasks.
	 */
	for (i = 0; i < init.cnt; i++) {
		if (programs[i].task != NULL) {
			program_ready(&programs[i]);
			task_release(programs[i].task);
		}
	}

#ifdef CONFIG_KCONSOLE
	if (!stdin) {
		thread_sleep(10);
		printf("kinit: No stdin\nKernel alive: .");

		unsigned int i = 0;
		while (true) {
			printf("\b%c", alive[i % ALIVE_CHARS]);
			thread_sleep(1);
			i++;
		}
	}
#endif /* CONFIG_KCONSOLE */
}

/** @}
 */
