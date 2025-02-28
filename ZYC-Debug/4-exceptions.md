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






