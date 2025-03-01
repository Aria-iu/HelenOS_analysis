# AMD64 异常处理机制

```c
#define IDT_ITEMS  64
#define IVT_ITEMS  IDT_ITEMS
// 这里是
exc_table_t exc_table[IVT_ITEMS];

typedef void (*iroutine_t)(unsigned int, istate_t *);

typedef struct {
	const char *name;
	bool hot;
	iroutine_t handler;
	uint64_t cycles;
	uint64_t count;
} exc_table_t;
```
AMD64中，异常和中断，force control transfers from the currently-executing program to a system-
software service routine that handles the interrupting event.

When an exception or interrupt occurs, the processor uses the interrupt vector number as an index into
the interrupt-descriptor table (IDT). An IDT is used in all processor operating modes, including real
mode (also called real-address mode), protected mode, and long mode.
当异常发生时，处理器使用一个中断向量号指示处理程序在IDT中的位置。IDT在所有模式都可以被使用。
（当然，我们已经到了main_bsp_...函数，已经是长模式了）

注册异常处理函数：
```c
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
```

在各个架构没有注册自己特定的异常处理之前，默认都用 exc_undef handler 处理
```c
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

```
那么传递给异常处理函数的参数是哪里来的呢？

可以看出，每个 handler i 宏定义的是一个 int i 的符号，每个 int i 是一个处理程序，
在其中调用 exc_dispatch(i, istate)

参数 i 是 int i 的字面值。参数 istate 类型如下。

另外，我们如何确定当前异常是在内核态发生还是在用户态发生？如果是用户态，需要切换到内核态，
切换到内核的 FS 基地址。

答案是依赖 istate 中的 cs 字段。代码
```c
cmpq $(GDT_SELECTOR(KTEXT_DES)), ISTATE_OFFSET_CS(%rsp)
```
将 cs 和 GDT_SELECTOR(KTEXT_DES) 进行比较，如果当前 cs 是内核 text 段选择子，那么
就是在内核态，否则是在用户态。
```c
typedef struct istate {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	/* align rbp_frame on multiple of 16 */
	uint64_t alignment;
	/* imitation of frame pointer linkage */
	uint64_t rbp_frame;
	/* imitation of frame address linkage */
	uint64_t rip_frame;
	/* real or fake error word */
	uint64_t error_word;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	/* only if istate_t is from uspace */
	uint64_t rsp;
	/* only if istate_t is from uspace */
	uint64_t ss;
} istate_t;


.macro handler i
SYMBOL(int_\i)

	/*
	 * Choose between version with error code and version without error
	 * code.
	 */

	.iflt \i-32
		.if (1 << \i) & ERROR_WORD_INTERRUPT_LIST
			/*
			 * Version with error word.
			 */
			subq $ISTATE_SOFT_SIZE, %rsp
		.else
			/*
			 * Version without error word.
			 */
			subq $(ISTATE_SOFT_SIZE + 8), %rsp
		.endif
	.else
		/*
		 * Version without error word.
		 */
		subq $(ISTATE_SOFT_SIZE + 8), %rsp
	.endif

	/*
	 * Save the general purpose registers.
	 */
	movq %rax, ISTATE_OFFSET_RAX(%rsp)
	movq %rbx, ISTATE_OFFSET_RBX(%rsp)
	movq %rcx, ISTATE_OFFSET_RCX(%rsp)
	movq %rdx, ISTATE_OFFSET_RDX(%rsp)
	movq %rsi, ISTATE_OFFSET_RSI(%rsp)
	movq %rdi, ISTATE_OFFSET_RDI(%rsp)
	movq %rbp, ISTATE_OFFSET_RBP(%rsp)
	movq %r8, ISTATE_OFFSET_R8(%rsp)
	movq %r9, ISTATE_OFFSET_R9(%rsp)
	movq %r10, ISTATE_OFFSET_R10(%rsp)
	movq %r11, ISTATE_OFFSET_R11(%rsp)
	movq %r12, ISTATE_OFFSET_R12(%rsp)
	movq %r13, ISTATE_OFFSET_R13(%rsp)
	movq %r14, ISTATE_OFFSET_R14(%rsp)
	movq %r15, ISTATE_OFFSET_R15(%rsp)

	/*
	 * Is this trap from the kernel?
	 */
	cmpq $(GDT_SELECTOR(KTEXT_DES)), ISTATE_OFFSET_CS(%rsp)
	jz 0f

	/*
	 * Switch to kernel FS base.
	 * #define AMD_MSR_FS		       0xc0000100
	 * #define KSEG_OFFSET_FSBASE      0x10
	 * #define AMD_MSR_GS		0xc0000101
     * #define AMD_MSR_GS_KERNEL	0xc0000102      在kseg_init()时，将AMD_MSR_GS_KERNEL的值写为kseg结构体的地址。
	 */
	/*
	    SWAPGS 指令交换 MSR 寄存器 AMD_MSR_GS_KERNEL（该寄存器保存着内核数据结构的指针） 
	    与 GS 基址寄存器 
	    AMD_MSR_GS 中的值。交换后，内核就可以使用 GS 前缀来访问内核数据结构了。
	*/
	swapgs
	movl $AMD_MSR_FS, %ecx
	// 查看 kseg_t 结构，这里是将其中的字段 uint64_t fsbase 写入 edx:eax ,最后将其写入 AMD_MSR_FS，这个 fsbase 就是内核的 fs 基地址。
	movl %gs:KSEG_OFFSET_FSBASE, %eax
	movl %gs:KSEG_OFFSET_FSBASE+4, %edx
	wrmsr
	// 通过 swapgs 切换 GS 段寄存器，将 GS 寄存器值和一个特定位置的值进行交换，目的是恢复 GS 值
	swapgs

	/*
	 * Imitate a regular stack frame linkage.
	 * Stop stack traces here if we came from userspace.
	 */
0:	movl $0x0, %edx
	cmovnzq %rdx, %rbp

	movq %rbp, ISTATE_OFFSET_RBP_FRAME(%rsp)
	movq ISTATE_OFFSET_RIP(%rsp), %rax
	movq %rax, ISTATE_OFFSET_RIP_FRAME(%rsp)
	leaq ISTATE_OFFSET_RBP_FRAME(%rsp), %rbp

	movq $(\i), %rdi   /* pass intnum in the first argument */
	movq %rsp, %rsi    /* pass istate address in the second argument */

	cld

	/* Call exc_dispatch(i, istate) */
	call exc_dispatch

	/*
	 * Restore all scratch registers and the preserved registers we have
	 * clobbered in this handler (i.e. RBP).
	 */
	movq ISTATE_OFFSET_RAX(%rsp), %rax
	movq ISTATE_OFFSET_RCX(%rsp), %rcx
	movq ISTATE_OFFSET_RDX(%rsp), %rdx
	movq ISTATE_OFFSET_RSI(%rsp), %rsi
	movq ISTATE_OFFSET_RDI(%rsp), %rdi
	movq ISTATE_OFFSET_RBP(%rsp), %rbp
	movq ISTATE_OFFSET_R8(%rsp), %r8
	movq ISTATE_OFFSET_R9(%rsp), %r9
	movq ISTATE_OFFSET_R10(%rsp), %r10
	movq ISTATE_OFFSET_R11(%rsp), %r11

	/* $8 = Skip error word */
	addq $(ISTATE_SOFT_SIZE + 8), %rsp
	
	/* 观察 istate 结构，现在 rsp 指向 其中的 RIP 字段。
	iretq 指令会从堆栈中恢复以下寄存器的值：
        RIP（Instruction Pointer）：恢复指令指针，指向中断发生前的下一条指令。
        CS（Code Segment）：恢复代码段寄存器，确保返回到正确的代码段。
        RFLAGS：恢复标志寄存器，恢复中断发生前的标志状态。
        RSP（Stack Pointer）：恢复堆栈指针，切换回用户空间的堆栈。
        SS（Stack Segment）：恢复堆栈段寄存器，确保堆栈操作在用户空间进行。
	*/
	iretq
.endm

#define LIST_0_63 \
	0, 1, 2, 3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,\
	28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,\
	53,54,55,56,57,58,59,60,61,62,63

SYMBOL(interrupt_handlers)
.irp cnt, LIST_0_63
	handler \cnt
.endr


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

	// 不论 int 几 指令，异常处理流程都会在 exc_table 定义的 handler 里面
	exc_table[n].handler(n + IVT_FIRST, istate);

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
```


