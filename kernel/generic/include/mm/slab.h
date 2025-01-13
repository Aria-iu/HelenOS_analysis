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

// ��ʾһ��magazine����ÿ��CPU�ϵĶ������棬�����Ż������ϵͳ���ڴ���䡣
typedef struct {
	// ���ӵ�����magazine������ָ��
	link_t link;
	// ��ǰmagazine�Ѿ�ռ�õĲ�λ��
	size_t busy;  /**< Count of full slots in magazine */
	// magazine�еĲ�λ����
	size_t size;  /**< Number of slots in magazine */
	// �洢����Ĳ�λ���飬ÿ����λ�洢һ������ָ��
	void *objs[];  /**< Slots in magazine */
} slab_magazine_t;

// ÿ��CPU�ϵĻ���ṹ�����ڴ洢��ǰCPU��magazine��
// ÿ�� CPU ����һ�� slab_mag_cache_t �ṹ��
// �����䱾�ص� magazine�������� CPU ����ͬһ�� magazine��
typedef struct {
	// ��ǰ����ʹ�õ� magazine��
	slab_magazine_t *current;
	// ��һ��ʹ�ù��� magazine��
	slab_magazine_t *last;
	// ���ڱ��� magazine ���ʵ���������
	IRQ_SPINLOCK_DECLARE(lock);
} slab_mag_cache_t;

// SLAB �������е���Ҫ���ݽṹ����ʾһ�� ���棨cache����
// ÿ�� SLAB cache ���ڹ�����ͬ��С������ڴ�ء�
typedef struct {
	// ��������
	const char *name;

	// 
	link_t link;

	/* Configuration */

	/** Size of slab position - align_up(sizeof(obj)) */
	// ÿ������Ĵ�С
	size_t size;

	// �����ʼ�����캯��
	errno_t (*constructor)(void *obj, unsigned int kmflag);
	// �������ٵ���������
	size_t (*destructor)(void *obj);

	/** Flags changing behaviour of cache */
	// �����־�����ƻ�����Ϊ
	unsigned int flags;

	/* Computed values */
	// ÿ��SLAB�����ҳ����
	size_t frames;   /**< Number of frames to be allocated */
	// ÿ��SLAB�������ɵĶ�����
	size_t objects;  /**< Number of objects that fit in */

	/* Statistics */
	// �ѷ���� SLAB ������
	atomic_size_t allocated_slabs;
	// �ѷ���Ķ���������
	atomic_size_t allocated_objs;
	// ����Ķ���������
	atomic_size_t cached_objs;
	/** How many magazines in magazines list */
	// magazine ������
	atomic_size_t magazine_counter;

	/* Slabs */
	// ����������� SLAB �б�
	list_t full_slabs;     /**< List of full slabs */
	// ��Ų������� SLAB �б�
	list_t partial_slabs;  /**< List of partial slabs */
	// ���� SLAB �б��������
	IRQ_SPINLOCK_DECLARE(slablock);
	/* Magazines */
	// ��� magazine ���б�
	list_t magazines;  /**< List o full magazines */
	// ����magazine �б��������
	IRQ_SPINLOCK_DECLARE(maglock);

	/** CPU cache */
	// ָ�� CPU ����Ļ����ָ��
	slab_mag_cache_t *mag_cache;
} slab_cache_t;

// ����һ���µ� SLAB ���档
extern slab_cache_t *slab_cache_create(const char *, size_t, size_t,
    errno_t (*)(void *, unsigned int), size_t (*)(void *), unsigned int);
// ����һ�� SLAB ���棬�ͷ������Դ��
extern void slab_cache_destroy(slab_cache_t *);

// ��ָ���� SLAB �����з���һ������
extern void *slab_alloc(slab_cache_t *, unsigned int)
    __attribute__((malloc));
// �ͷ�һ�����󣬲����䷵�ظ�ָ���� SLAB ���档
extern void slab_free(slab_cache_t *, void *);
// ���Ի��� SLAB �����е��ڴ棬ͨ�����ڴ����ʱ���á�
extern size_t slab_reclaim(unsigned int);

/* slab subsytem initialization */
// ��ʼ�� SLAB �����������ö���ڲ����棨����־���桢SLAB ���桢SLAB ����������ȣ���
extern void slab_cache_init(void);
// ����ÿ CPU �� SLAB ���档���������ڶ��ϵͳ��������ܣ������������á�
extern void slab_enable_cpucache(void);

/* kconsole debug */
// ���Ժ��������ڴ�ӡ��ǰ SLAB ������б���Ϣ��
extern void slab_print_list(void);

#endif

/** @}
 */
