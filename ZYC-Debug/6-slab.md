## Slab 数据结构
```c
typedef struct {
	link_t link;
	size_t busy;  /**< Count of full slots in magazine */
	size_t size;  /**< Number of slots in magazine */
	void *objs[];  /**< Slots in magazine */
} slab_magazine_t;

typedef struct {
	slab_magazine_t *current;
	slab_magazine_t *last;
	IRQ_SPINLOCK_DECLARE(lock);
} slab_mag_cache_t;

typedef struct {
	const char *name;
	link_t link;

	/* Configuration */

	/** Size of slab position - align_up(sizeof(obj)) */
	size_t size;

	errno_t (*constructor)(void *obj, unsigned int kmflag);
	size_t (*destructor)(void *obj);

	/** Flags changing behaviour of cache */
	unsigned int flags;

	/* Computed values */
	size_t frames;   /**< Number of frames to be allocated */
	size_t objects;  /**< Number of objects that fit in */

	/* Statistics */
	atomic_size_t allocated_slabs;
	atomic_size_t allocated_objs;
	atomic_size_t cached_objs;
	/** How many magazines in magazines list */
	atomic_size_t magazine_counter;

	/* Slabs */
	list_t full_slabs;     /**< List of full slabs */
	list_t partial_slabs;  /**< List of partial slabs */
	IRQ_SPINLOCK_DECLARE(slablock);
	/* Magazines */
	list_t magazines;  /**< List of full magazines */
	IRQ_SPINLOCK_DECLARE(maglock);

	/** CPU cache */
	slab_mag_cache_t *mag_cache;
} slab_cache_t;
```

