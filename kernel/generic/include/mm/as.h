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

/** @addtogroup kernel_generic_mm
 * @{
 */
/** @file
 */

#ifndef KERN_AS_H_
#define KERN_AS_H_

#include <typedefs.h>
#include <abi/mm/as.h>
#include <arch/mm/page.h>
#include <arch/mm/as.h>
#include <arch/mm/asid.h>
#include <arch/istate.h>
#include <synch/spinlock.h>
#include <synch/mutex.h>
#include <adt/list.h>
#include <adt/odict.h>
#include <lib/elf.h>
#include <arch.h>
#include <lib/refcount.h>

#define AS                   CURRENT->as

/**
 * Defined to be true if user address space and kernel address space shadow each
 * other.
 *
 */
#define KERNEL_ADDRESS_SPACE_SHADOWED  KERNEL_ADDRESS_SPACE_SHADOWED_ARCH

/**
 * Defined to be true if user address space and kernel address space do not
 * share the same page table.
 */
#define KERNEL_SEPARATE_PTL0 KERNEL_SEPARATE_PTL0_ARCH

#define KERNEL_ADDRESS_SPACE_START  KERNEL_ADDRESS_SPACE_START_ARCH
#define KERNEL_ADDRESS_SPACE_END    KERNEL_ADDRESS_SPACE_END_ARCH
#define USER_ADDRESS_SPACE_START    USER_ADDRESS_SPACE_START_ARCH
#define USER_ADDRESS_SPACE_END      USER_ADDRESS_SPACE_END_ARCH

/** Kernel address space. */
// 标识当前地址空间是否为内核地址空间
#define FLAG_AS_KERNEL  (1 << 0)

/* Address space area attributes. */
#define AS_AREA_ATTR_NONE     0
#define AS_AREA_ATTR_PARTIAL  1  /**< Not fully initialized area. */

/** The page fault was resolved by as_page_fault(). */
#define AS_PF_OK     0

/** The page fault was caused by memcpy_from_uspace() or memcpy_to_uspace(). */
#define AS_PF_DEFER  1

/** The page fault was not resolved by as_page_fault(). */
#define AS_PF_FAULT  2

/** The page fault was not resolved by as_page_fault(). Non-verbose version. */
#define AS_PF_SILENT 3

/** Address space structure.
 *
 * as_t contains the list of as_areas of userspace accessible
 * pages for one or more tasks. Ranges of kernel memory pages are not
 * supposed to figure in the list as they are shared by all tasks and
 * set up during system initialization.
 *
 * 地址空间结构体，包含了关于进程地址空间的信息。
 */
typedef struct as {
	/** Protected by asidlock. */
	link_t inactive_as_with_asid_link;

	/**
	 * Number of processors on which this
	 * address space is active. Protected by
	 * asidlock.
	 */
	size_t cpu_refcount;	// 用来跟踪地址空间在多少个 CPU 核心上活跃

	/** Address space identifier.
	 *
	 * Constant on architectures that do not
	 * support ASIDs. Protected by asidlock.
	 *
	 */
	asid_t asid;	// 地址空间标识符， 用于区分和标识不同的地址空间

	/** Number of references (i.e. tasks that reference this as). */
	atomic_refcount_t refcount;	// 引用计数，标识有多少个任务引用这个地址空间。

	mutex_t lock;

	/** Address space areas in this address space by base address.
	 *
	 * Members are of type as_area_t.
	 */
	odict_t as_areas;	// 地址空间区域的字典，存储了所有可以访问的内存区域。

	/** Non-generic content. */
	as_genarch_t genarch;

	/** Architecture specific content. */
	as_arch_t arch;
} as_t;

// 包含一系列与地址空间操作相关的函数指针。
// 不同的架构可能有不同的实现，因此这里定义了一个操作接口，架构可以根据需要提供特定的实现。
typedef struct {
	pte_t *(*page_table_create)(unsigned int);
	void (*page_table_destroy)(pte_t *);
	void (*page_table_lock)(as_t *, bool);
	void (*page_table_unlock)(as_t *, bool);
	bool (*page_table_locked)(as_t *);
} as_operations_t;

/** Single anonymous page mapping. */
// 表示一个匿名页面的映射
typedef struct {
	/** Containing pagemap structure */
	struct as_pagemap *pagemap;
	/** Link to @c shinfo->pagemap ordered dictionary */
	odlink_t lpagemap;
	/** Virtual address */
	uintptr_t vaddr;			// 虚拟地址
	/** Physical frame address */
	uintptr_t frame;			// 物理页框的地址
} as_page_mapping_t;

/** Map of anonymous pages in a shared area. */
typedef struct as_pagemap {
	/**
	 * Dictionary ordered by virtual address. Members are of type
	 * as_page_mapping_t
	 */
	odict_t map;
} as_pagemap_t;

/** Used space interval */
typedef struct {
	/** Containing used_space structure */
	struct used_space *used_space;
	/** Link to @c used_space->ivals */
	odlink_t lused_space;
	/** First page address */
	uintptr_t page;
	/** Count of pages */
	size_t count;
} used_space_ival_t;

/** Map of used space in an address space area */
typedef struct used_space {
	/**
	 * Dictionary of intervals by start address.
	 * Members are of type @c used_space_ival_t.
	 */
	odict_t ivals;
	/** Total number of used pages. */
	size_t pages;
} used_space_t;

/**
 * This structure contains information associated with the shared address space
 * area.
 *
 */
typedef struct {
	/** This lock must be acquired only when the as_area lock is held. */
	mutex_t lock;
	/** This structure can be deallocated if refcount drops to 0. */
	size_t refcount;
	/** True if the area has been ever shared. */
	bool shared;

	/** Complete map of anonymous pages of the shared area. */
	as_pagemap_t pagemap;

	/** Address space area backend. */
	struct mem_backend *backend;
	/** Address space area shared data. */
	void *backend_shared_data;
} share_info_t;

/** Page fault access type. */
typedef enum {
	PF_ACCESS_READ,
	PF_ACCESS_WRITE,
	PF_ACCESS_EXEC,
	PF_ACCESS_UNKNOWN
} pf_access_t;

struct mem_backend;

/** Backend data stored in address space area. */
typedef union mem_backend_data {
	/* anon_backend members */
	struct {
	};

	/** elf_backend members */
	struct {
		uintptr_t elf_base;
		elf_header_t *elf;
		elf_segment_header_t *segment;
	};

	/** phys_backend members */
	struct {
		uintptr_t base;
		size_t frames;
		bool anonymous;
		struct parea *parea;
	};

	/** user_backend members */
	struct {
		as_area_pager_info_t pager_info;
	};

} mem_backend_data_t;

/** Address space area structure.
 *
 * Each as_area_t structure describes one contiguous area of virtual memory.
 * 
 * 地址空间中的一个虚拟内存区域。每个区域都有自己的属性。
 * 
 */
typedef struct {
	mutex_t lock;					// 该区域的锁

	/** Containing address space. */
	as_t *as;

	/** Link to @c as->as_areas */
	odlink_t las_areas;

	/** Memory flags. */
	unsigned int flags;

	/** Address space area attributes. */
	unsigned int attributes;

	/** Number of pages in the area. */
	size_t pages;					// 该区域的页数

	/** Base address of this area. */
	uintptr_t base;					// 该区域的基地址

	/** Map of used space. */
	used_space_t used_space;		// 该区域内已用的空间

	/**
	 * If the address space area is shared. this is
	 * a reference to the share info structure.
	 */
	share_info_t *sh_info;

	/** Memory backend backing this address space area. */
	struct mem_backend *backend;

	/** Data to be used by the backend. */
	mem_backend_data_t backend_data;
} as_area_t;

/** Address space area backend structure. */
// 表示内存后端的结构体，每个后端提供了不同的操作方式，例如创建、销毁、共享地址空间区域，以及处理页错误等。
typedef struct mem_backend {
	bool (*create)(as_area_t *);
	bool (*resize)(as_area_t *, size_t);
	void (*share)(as_area_t *);
	void (*destroy)(as_area_t *);

	bool (*is_resizable)(as_area_t *);
	bool (*is_shareable)(as_area_t *);

	int (*page_fault)(as_area_t *, uintptr_t, pf_access_t);
	void (*frame_free)(as_area_t *, uintptr_t, uintptr_t);

	bool (*create_shared_data)(as_area_t *);
	void (*destroy_shared_data)(void *);
} mem_backend_t;

extern as_t *AS_KERNEL;

extern const as_operations_t *as_operations;
extern list_t inactive_as_with_asid_list;

extern void as_init(void);

// 创建地址空间
extern as_t *as_create(unsigned int);
extern void as_hold(as_t *);
extern void as_release(as_t *);
// 切换当前地址空间
extern void as_switch(as_t *, as_t *);
// 处理页面错误
extern int as_page_fault(uintptr_t, pf_access_t, istate_t *);

// 创建新的区域
extern as_area_t *as_area_create(as_t *, unsigned int, size_t, unsigned int,
    mem_backend_t *, mem_backend_data_t *, uintptr_t *, uintptr_t);
extern errno_t as_area_destroy(as_t *, uintptr_t);
// 改变区域的大小
extern errno_t as_area_resize(as_t *, uintptr_t, size_t, unsigned int);
// 共享区域
extern errno_t as_area_share(as_t *, uintptr_t, size_t, as_t *, unsigned int,
    uintptr_t *, uintptr_t);
extern errno_t as_area_change_flags(as_t *, unsigned int, uintptr_t);
extern as_area_t *as_area_first(as_t *);
extern as_area_t *as_area_next(as_area_t *);

extern void as_pagemap_initialize(as_pagemap_t *);
extern void as_pagemap_finalize(as_pagemap_t *);
extern as_page_mapping_t *as_pagemap_first(as_pagemap_t *);
extern as_page_mapping_t *as_pagemap_next(as_page_mapping_t *);
extern errno_t as_pagemap_find(as_pagemap_t *, uintptr_t, uintptr_t *);
extern void as_pagemap_insert(as_pagemap_t *, uintptr_t, uintptr_t);
extern void as_pagemap_remove(as_page_mapping_t *);

extern unsigned int as_area_get_flags(as_area_t *);
extern bool as_area_check_access(as_area_t *, pf_access_t);
extern size_t as_area_get_size(uintptr_t);
extern used_space_ival_t *used_space_first(used_space_t *);
extern used_space_ival_t *used_space_next(used_space_ival_t *);
extern used_space_ival_t *used_space_find_gteq(used_space_t *, uintptr_t);
extern bool used_space_insert(used_space_t *, uintptr_t, size_t);

/* Interface to be implemented by architectures. */

#ifndef as_constructor_arch
extern errno_t as_constructor_arch(as_t *, unsigned int);
#endif /* !def as_constructor_arch */

#ifndef as_destructor_arch
extern int as_destructor_arch(as_t *);
#endif /* !def as_destructor_arch */

#ifndef as_create_arch
extern errno_t as_create_arch(as_t *, unsigned int);
#endif /* !def as_create_arch */

#ifndef as_install_arch
extern void as_install_arch(as_t *);
#endif /* !def as_install_arch */

#ifndef as_deinstall_arch
extern void as_deinstall_arch(as_t *);
#endif /* !def as_deinstall_arch */

/* Backend declarations and functions. */
extern mem_backend_t anon_backend;
extern mem_backend_t elf_backend;
extern mem_backend_t phys_backend;
extern mem_backend_t user_backend;

/* Address space area related syscalls. */
extern sysarg_t sys_as_area_create(uintptr_t, size_t, unsigned int, uintptr_t,
    uspace_ptr_as_area_pager_info_t);
extern sys_errno_t sys_as_area_resize(uintptr_t, size_t, unsigned int);
extern sys_errno_t sys_as_area_change_flags(uintptr_t, unsigned int);
extern sys_errno_t sys_as_area_get_info(uintptr_t, uspace_ptr_as_area_info_t);
extern sys_errno_t sys_as_area_destroy(uintptr_t);

/* Introspection functions. */
extern as_area_info_t *as_get_area_info(as_t *, size_t *);
extern void as_print(as_t *);

#endif

/** @}
 */
