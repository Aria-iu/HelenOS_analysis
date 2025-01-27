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

/** @addtogroup kernel_ia32
 * @{
 */
/**
 * @file
 * @brief i8254 chip driver.
 *
 * Low level time functions.
 */

#include <stdint.h>
#include <time/clock.h>
#include <time/delay.h>
#include <arch/cycle.h>
#include <arch/interrupt.h>
#include <genarch/drivers/i8259/i8259.h>
#include <arch/drivers/i8254.h>
#include <cpu.h>
#include <config.h>
#include <arch/pm.h>
#include <arch/asm.h>
#include <arch/cpuid.h>
#include <arch.h>
#include <ddi/irq.h>

#define CLK_PORT1  ((ioport8_t *) 0x40U)
#define CLK_PORT4  ((ioport8_t *) 0x43U)

#define CLK_CONST     1193180
#define MAGIC_NUMBER  1194

#define LOOPS  150000
#define SHIFT  11

// 系统时钟中断
static irq_t i8254_irq;

static irq_ownership_t i8254_claim(irq_t *irq)
{
	return IRQ_ACCEPT;
}

static void i8254_irq_handler(irq_t *irq)
{
	/*
	 * This IRQ is responsible for kernel preemption.
	 * Nevertheless, we are now holding a spinlock which prevents
	 * preemption. For this particular IRQ, we don't need the
	 * lock. We just release it, call clock() and then reacquire it again.
	 */
	irq_spinlock_unlock(&irq->lock, false);
	clock();
	irq_spinlock_lock(&irq->lock, false);
}

void i8254_init(void)
{
	// 初始化中断 i8254_irq ， 会调用 irq_initialize 先将 i8254_irq清零。
	// 在调用irq_spinlock_initialize初始化 i8254_irq -> lock
	// 再设置 irq->inr = -1
	// 最后调用 irq_initialize_arch
	irq_initialize(&i8254_irq);
	// 设置 i8254_irq 的字段。
	i8254_irq.preack = true;
	// IRQ_CLK = 0
	// VECTOR_CLK = IVT_IRQBASE + IRQ_CLK = 32 + 0 = 32
	i8254_irq.inr = IRQ_CLK;
	i8254_irq.claim = i8254_claim;
	i8254_irq.handler = i8254_irq_handler;
	// 注册 i8254_irq 中断。
	irq_register(&i8254_irq);

	// 设置 i8254 定时器的模式，并启用相应的中断
	i8254_normal_operation();
}

void i8254_normal_operation(void)
{
	// 通过 pio_write_8 向 i8254 的控制寄存器写入 0x36，设置定时器的工作模式。
	// 0x36 通常表示计数器 0 工作在 "模式 3"（方波生成模式），并且是计数器 0 的方式。
	pio_write_8(CLK_PORT4, 0x36);
	// 通过 i8259_disable_irqs 禁用 i8254 中断。1 << IRQ_CLK 表示禁用与 IRQ_CLK 对应的中断。
	i8259_disable_irqs(1 << IRQ_CLK);
	// 这两行代码向 i8254 定时器的计数器写入一个值，通常是一个分频值，用来设置定时器的周期。
	// CLK_CONST / HZ 表示时钟常量除以期望的频率值（HZ），然后分成低位和高位写入 i8254。
	pio_write_8(CLK_PORT1, (CLK_CONST / HZ) & 0xf);
	pio_write_8(CLK_PORT1, (CLK_CONST / HZ) >> 8);
	// 通过 i8259_enable_irqs 启用与 IRQ_CLK 对应的中断。
	i8259_enable_irqs(1 << IRQ_CLK);
}

// CLK_PORT4 = 0X43
// CLK_PORT1 = 0X40
// SHIFT = 11
void i8254_calibrate_delay_loop(void)
{
	/*
	 * One-shot timer. Count-down from 0xffff at 1193180Hz
	 * MAGIC_NUMBER is the magic value for 1ms.
	 */
	// 初始化8254定时器
	pio_write_8(CLK_PORT4, 0x30);
	// 将计数器0的初始值设置为0xffff，即65535。这意味着定时器将从65535开始倒计时
	pio_write_8(CLK_PORT1, 0xff);
	pio_write_8(CLK_PORT1, 0xff);

	uint8_t not_ok;
	uint32_t t1;
	uint32_t t2;

	do {
		/* will read both status and count */
		// 读取计数器0的状态和计数值
		pio_write_8(CLK_PORT4, 0xc2);
		// 检查定时器是否准备好（即是否已经从0xffff开始倒计时）。
		not_ok = (uint8_t) ((pio_read_8(CLK_PORT1) >> 6) & 1);
		// 读取计数器0的当前值。
		t1 = pio_read_8(CLK_PORT1);
		t1 |= pio_read_8(CLK_PORT1) << 8;
	} while (not_ok);

	// 延迟循环的次数
	// LOOPS = 150000
	asm_delay_loop(LOOPS);

	// 读取定时器的当前值
	pio_write_8(CLK_PORT4, 0xd2);
	t2 = pio_read_8(CLK_PORT1);
	t2 |= pio_read_8(CLK_PORT1) << 8;

	// 计算校准机制的开销
	/*
	 * We want to determine the overhead of the calibrating mechanism.
	 */
	pio_write_8(CLK_PORT4, 0xd2);
	uint32_t o1 = pio_read_8(CLK_PORT1);
	o1 |= pio_read_8(CLK_PORT1) << 8;

	// 这里并不进行循环，只是将 t2-t1时间中的（o2-o1）的偏差算出来
	asm_fake_loop(LOOPS);

	pio_write_8(CLK_PORT4, 0xd2);
	uint32_t o2 = pio_read_8(CLK_PORT1);
	o2 |= pio_read_8(CLK_PORT1) << 8;

	// 计算延迟循环的常数
	uint32_t delta = (t1 - t2) - (o1 - o2);
	if (!delta)
		delta = 1;

	CPU->delay_loop_const =
	    ((MAGIC_NUMBER * LOOPS) / 1000) / delta +
	    (((MAGIC_NUMBER * LOOPS) / 1000) % delta ? 1 : 0);

	// 计算CPU频率
	uint64_t clk1 = get_cycle();
	delay(1 << SHIFT);
	uint64_t clk2 = get_cycle();

	CPU->frequency_mhz = (clk2 - clk1) >> SHIFT;

	return;
}

/** @}
 */
