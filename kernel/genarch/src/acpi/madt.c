/*
 * Copyright (c) 2005 Jakub Jermar
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

/** @addtogroup kernel_genarch
 * @{
 */
/**
 * @file
 * @brief Multiple APIC Description Table (MADT) parsing.
 */

#include <assert.h>
#include <typedefs.h>
#include <genarch/acpi/acpi.h>
#include <genarch/acpi/madt.h>
#include <arch/smp/apic.h>
#include <arch/smp/smp.h>
#include <panic.h>
#include <config.h>
#include <log.h>
#include <stdlib.h>
#include <gsort.h>

struct acpi_madt *acpi_madt = NULL;

#ifdef CONFIG_SMP

/**
 * Standard ISA IRQ map; can be overriden by
 * Interrupt Source Override entries of MADT.
 */
static int isa_irq_map[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

struct madt_l_apic *madt_l_apic_entries = NULL;
struct madt_io_apic *madt_io_apic_entries = NULL;

static size_t madt_l_apic_entry_index = 0;
static size_t madt_io_apic_entry_index = 0;
static size_t madt_l_apic_entry_cnt = 0;
static size_t madt_io_apic_entry_cnt = 0;

static struct madt_apic_header **madt_entries_index = NULL;

const char *entry[] = {
	"L_APIC",
	"IO_APIC",
	"INTR_SRC_OVRD",
	"NMI_SRC",
	"L_APIC_NMI",
	"L_APIC_ADDR_OVRD",
	"IO_SAPIC",
	"L_SAPIC",
	"PLATFORM_INTR_SRC"
};

static uint8_t madt_cpu_apic_id(size_t i)
{
	assert(i < madt_l_apic_entry_cnt);

	return ((struct madt_l_apic *)
	    madt_entries_index[madt_l_apic_entry_index + i])->apic_id;
}

static bool madt_cpu_enabled(size_t i)
{
	assert(i < madt_l_apic_entry_cnt);

	/*
	 * FIXME: The current local APIC driver limits usable
	 * CPU IDs to 8.
	 *
	 */
	if (i > 7)
		return false;

	return ((struct madt_l_apic *)
	    madt_entries_index[madt_l_apic_entry_index + i])->flags & 0x1;
}

static bool madt_cpu_bootstrap(size_t i)
{
	assert(i < madt_l_apic_entry_cnt);

	return ((struct madt_l_apic *)
	    madt_entries_index[madt_l_apic_entry_index + i])->apic_id ==
	    bsp_l_apic;
}

static int madt_irq_to_pin(unsigned int irq)
{
	if (irq >= sizeof(isa_irq_map) / sizeof(int))
		return (int) irq;

	return isa_irq_map[irq];
}

/** ACPI MADT Implementation of SMP configuration interface.
 *
 */
struct smp_config_operations madt_config_operations = {
	.cpu_enabled = madt_cpu_enabled,
	.cpu_bootstrap = madt_cpu_bootstrap,
	.cpu_apic_id = madt_cpu_apic_id,
	.irq_to_pin = madt_irq_to_pin
};

static int madt_cmp(void *a, void *b, void *arg)
{
	uint8_t typea = (*((struct madt_apic_header **) a))->type;
	uint8_t typeb = (*((struct madt_apic_header **) b))->type;

	if (typea > typeb)
		return 1;

	if (typea < typeb)
		return -1;

	return 0;
}

static void madt_l_apic_entry(struct madt_l_apic *la, size_t i)
{
	if (madt_l_apic_entry_cnt == 0)
		madt_l_apic_entry_index = i;

	// 每处理一个 MADT_L_APIC 条目，就会增加 madt_l_apic_entry_cnt 的计数。
	// 这个计数器用于记录系统中存在的 MADT_L_APIC 条目的数量（即系统中支持的处理器数量）。
	madt_l_apic_entry_cnt++;

	// la->flags 是一个标志，指示当前 CPU 是否可用。如果 la->flags & 0x1 的结果为 0，
	// 意味着该 CPU 不可用，因此跳过这个 CPU，继续处理下一个条目。
	if (!(la->flags & 0x1)) {
		/* Processor is unusable, skip it. */
		return;
	}

	// 该行代码将当前处理的 APIC ID 标记为已使用。apic_id_mask 是一个位掩码，
	// 每个处理器都有一个唯一的 APIC ID。通过将对应的位设置为 1，
	// 可以记录系统中哪些 APIC ID 已经被使用。
	// 比如，如果 la->apic_id 为 2，则该操作会将 apic_id_mask 的第 2 位设为 1，
	// 表示系统中存在一个 APIC ID 为 2 的处理器。
	apic_id_mask |= 1 << la->apic_id;
}

// madt_io_apic_entry 函数的作用是处理 MADT_IO_APIC 条目，
// 该条目描述了系统中的 I/O APIC（输入/输出高级可编程中断控制器）。
static void madt_io_apic_entry(struct madt_io_apic *ioa, size_t i)
{
	if (madt_io_apic_entry_cnt == 0) {
		/* Remember index of the first io apic entry */
		madt_io_apic_entry_index = i;
		// ioa->io_apic_address 是指向 I/O APIC 寄存器的物理地址。
		// 这个地址会被保存到 io_apic 变量中，以便后续访问 I/O APIC 的硬件寄存器。
		io_apic = (uint32_t *) (sysarg_t) ioa->io_apic_address;
	} else {
		// 如果有超过一个 I/O APIC 条目，当前的实现暂时不支持进一步的处理。
		// 在这种情况下，函数只是跳过了额外的 I/O APIC 条目。
		// 如果系统有多个 I/O APIC，该部分代码会被执行，但没有具体的操作。
		/* Currently not supported */
	}

	madt_io_apic_entry_cnt++;
}

// 该条目用于重定向中断源。具体来说，它指定了某个特定中断源（IRQ）应映射到哪个全局中断。
static void madt_intr_src_ovrd_entry(struct madt_intr_src_ovrd *override,
    size_t i)
{
	assert(override->source < sizeof(isa_irq_map) / sizeof(int));
	// override->source 是 MADT_INTR_SRC_OVRD 条目中的 source 字段，
	// 它表示需要重定向的中断源（通常是 IRQ 值）。
	// isa_irq_map 是一个数组，它用来映射 ISA 总线上的中断源（IRQ）到全局中断（global_int）
	isa_irq_map[override->source] = override->global_int;
}

// 解析MADT表，根据表中的条目来初始化与系统中断和处理器相关的信息。
void acpi_madt_parse(void)
{
	struct madt_apic_header *end = (struct madt_apic_header *)
	    (((uint8_t *) acpi_madt) + acpi_madt->header.length);
	struct madt_apic_header *hdr;

	// 将 MADT 中存储的本地 APIC 地址赋值给 l_apic 变量。
	l_apic = (uint32_t *) (sysarg_t) acpi_madt->l_apic_address;

	/* Count MADT entries */
	// 这段代码遍历 MADT 中的所有条目，并计算条目的数量
	unsigned int madt_entries_index_cnt = 0;
	for (hdr = acpi_madt->apic_header; hdr < end;
	    hdr = (struct madt_apic_header *) (((uint8_t *) hdr) + hdr->length))
		madt_entries_index_cnt++;

	/* Create MADT APIC entries index array */
	// 根据计算的条目数量分配内存空间，用于保存所有 MADT 条目的指针
	madt_entries_index = (struct madt_apic_header **)
	    malloc(madt_entries_index_cnt * sizeof(struct madt_apic_header *));
	if (!madt_entries_index)
		panic("Memory allocation error.");

	size_t i = 0;

	// 遍历 MADT 表中的每个条目，将其指针保存到 madt_entries_index 数组中。
	for (hdr = acpi_madt->apic_header; hdr < end;
	    hdr = (struct madt_apic_header *) (((uint8_t *) hdr) + hdr->length)) {
		madt_entries_index[i] = hdr;
		i++;
	}

	/* Sort MADT index structure */
	if (!gsort(madt_entries_index, madt_entries_index_cnt,
	    sizeof(struct madt_apic_header *), madt_cmp, NULL))
		panic("Sorting error.");

	/* Parse MADT entries */
	// 解析 MADT 条目
	for (i = 0; i < madt_entries_index_cnt; i++) {
		hdr = madt_entries_index[i];

		// 根据条目的 type（类型）来决定如何处理
		switch (hdr->type) {
		case MADT_L_APIC:
			// 本地 APIC
			madt_l_apic_entry((struct madt_l_apic *) hdr, i);
			break;
		case MADT_IO_APIC:
			// IO APIC
			madt_io_apic_entry((struct madt_io_apic *) hdr, i);
			break;
		case MADT_INTR_SRC_OVRD:
			// 中断源重定向。
			madt_intr_src_ovrd_entry((struct madt_intr_src_ovrd *) hdr, i);
			break;
		case MADT_NMI_SRC:
		case MADT_L_APIC_NMI:
		case MADT_L_APIC_ADDR_OVRD:
		case MADT_IO_SAPIC:
		case MADT_L_SAPIC:
		case MADT_PLATFORM_INTR_SRC:
			log(LF_ARCH, LVL_WARN,
			    "MADT: Skipping %s entry (type=%" PRIu8 ")",
			    entry[hdr->type], hdr->type);
			break;
		default:
			if ((hdr->type >= MADT_RESERVED_SKIP_BEGIN) &&
			    (hdr->type <= MADT_RESERVED_SKIP_END))
				log(LF_ARCH, LVL_NOTE,
				    "MADT: Skipping reserved entry (type=%" PRIu8 ")",
				    hdr->type);

			if (hdr->type >= MADT_RESERVED_OEM_BEGIN)
				log(LF_ARCH, LVL_NOTE,
				    "MADT: Skipping OEM entry (type=%" PRIu8 ")",
				    hdr->type);

			break;
		}
	}

	if (madt_l_apic_entry_cnt > 0)
		config.cpu_count = madt_l_apic_entry_cnt;
}

#endif /* CONFIG_SMP */

/** @}
 */
