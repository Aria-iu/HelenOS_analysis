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

/** @addtogroup kernel_generic_mm
 * @{
 */
/** @file
 */

#ifndef KERN_SLAB_H_
#define KERN_SLAB_H_

#include <adt/list.h>
#include <synch/spinlock.h>
#include <atomic.h>
#include <mm/frame.h>

/** Initial Magazine size (TODO: dynamically growing magazines) */
#define SLAB_MAG_SIZE  4

/** If object size is less, store control structure inside SLAB */
#define SLAB_INSIDE_SIZE  (PAGE_SIZE >> 3)

/** Maximum wasted space we allow for cache */
#define SLAB_MAX_BADNESS(cache) \
	(FRAMES2SIZE((cache)->frames) >> 2)

/* slab_reclaim constants */

/** Reclaim all possible memory, because we are in memory stress */
#define SLAB_RECLAIM_ALL  0x01

/* cache_create flags */

/** Do not use per-cpu cache */
#define SLAB_CACHE_NOMAGAZINE   0x01
/** Have control structure inside SLAB */
#define SLAB_CACHE_SLINSIDE     0x02
/** We add magazine cache later, if we have this flag */
#define SLAB_CACHE_MAGDEFERRED  (0x04 | SLAB_CACHE_NOMAGAZINE)

// 表示一个magazine，即每个CPU上的独立缓存，用于优化多核心系统的内存分配。
typedef struct {
	// 链接到其他magazine的链表指针
	link_t link;
	// 当前magazine已经占用的槽位数
	size_t busy;  /**< Count of full slots in magazine */
	// magazine中的槽位总数
	size_t size;  /**< Number of slots in magazine */
	// 存储对象的槽位数组，每个槽位存储一个对象指针
	void *objs[];  /**< Slots in magazine */
} slab_magazine_t;

// 每个CPU上的缓存结构，用于存储当前CPU的magazine。
// 每个 CPU 都有一个 slab_mag_cache_t 结构，
// 管理其本地的 magazine，避免多个 CPU 争夺同一个 magazine。
typedef struct {
	// 当前正在使用的 magazine。
	slab_magazine_t *current;
	// 上一个使用过的 magazine。
	slab_magazine_t *last;
	// 用于保护 magazine 访问的自旋锁。
	IRQ_SPINLOCK_DECLARE(lock);
} slab_mag_cache_t;

// SLAB 分配器中的主要数据结构，表示一个 缓存（cache）。
// 每个 SLAB cache 用于管理相同大小对象的内存池。
typedef struct {
	// 缓存名称
	const char *name;

	// 
	link_t link;

	/* Configuration */

	/** Size of slab position - align_up(sizeof(obj)) */
	// 每个对象的大小
	size_t size;

	// 对象初始化构造函数
	errno_t (*constructor)(void *obj, unsigned int kmflag);
	// 对象销毁的析构函数
	size_t (*destructor)(void *obj);

	/** Flags changing behaviour of cache */
	// 缓存标志，控制缓存行为
	unsigned int flags;

	/* Computed values */
	// 每个SLAB所需的页框数
	size_t frames;   /**< Number of frames to be allocated */
	// 每个SLAB可以容纳的对象数
	size_t objects;  /**< Number of objects that fit in */

	/* Statistics */
	// 已分配的 SLAB 数量。
	atomic_size_t allocated_slabs;
	// 已分配的对象数量。
	atomic_size_t allocated_objs;
	// 缓存的对象数量。
	atomic_size_t cached_objs;
	/** How many magazines in magazines list */
	// magazine 的数量
	atomic_size_t magazine_counter;

	/* Slabs */
	// 存放已填满的 SLAB 列表。
	list_t full_slabs;     /**< List of full slabs */
	// 存放部分填充的 SLAB 列表。
	list_t partial_slabs;  /**< List of partial slabs */
	// 访问 SLAB 列表的自旋锁
	IRQ_SPINLOCK_DECLARE(slablock);
	/* Magazines */
	// 存放 magazine 的列表。
	list_t magazines;  /**< List o full magazines */
	// 访问magazine 列表的自旋锁
	IRQ_SPINLOCK_DECLARE(maglock);

	/** CPU cache */
	// 指向 CPU 级别的缓存的指针
	slab_mag_cache_t *mag_cache;
} slab_cache_t;

// 创建一个新的 SLAB 缓存。
extern slab_cache_t *slab_cache_create(const char *, size_t, size_t,
    errno_t (*)(void *, unsigned int), size_t (*)(void *), unsigned int);
// 销毁一个 SLAB 缓存，释放相关资源。
extern void slab_cache_destroy(slab_cache_t *);

// 从指定的 SLAB 缓存中分配一个对象。
extern void *slab_alloc(slab_cache_t *, unsigned int)
    __attribute__((malloc));
// 释放一个对象，并将其返回给指定的 SLAB 缓存。
extern void slab_free(slab_cache_t *, void *);
// 尝试回收 SLAB 缓存中的内存，通常在内存紧张时调用。
extern size_t slab_reclaim(unsigned int);

/* slab subsytem initialization */
// 初始化 SLAB 分配器，设置多个内部缓存（如杂志缓存、SLAB 缓存、SLAB 描述符缓存等）。
extern void slab_cache_init(void);
// 启用每 CPU 的 SLAB 缓存。这有助于在多核系统中提高性能，减少锁的争用。
extern void slab_enable_cpucache(void);

/* kconsole debug */
// 调试函数，用于打印当前 SLAB 缓存的列表信息。
extern void slab_print_list(void);

#endif

/** @}
 */
