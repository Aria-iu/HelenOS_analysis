# unmapped段
unmapped段是在链接脚本里面没有进行映射的段，也就是说，这部分的地址是物理地址，不需要内存映射或者进行直接映射来直接访问。

START_STACK = 107c00h ----> to kernel  0FFFFFFFF80107C00h

## unmapped_start
定义在连接脚本内部，unmapped_start = 00000000001080e8

## multiboot2 header
在编译时，meson指定multiboot2.S和multiboot.S同为arch源文件，但是multiboot2.S在前面所以先编译。

multiboot2_header_start = 00000000001080e8

multiboot2_header_end - multiboot2_header_start = 00 00 00 90

multiboot2_header_end = 1080e8+90  = 108178 = multiboot2_image_start

## tag_info_req_start
一些标记。

tag_info_req_start = 1080f8

tag_info_req_end = 108110

    tag_info_req_start:
		.word MULTIBOOT2_TAG_INFO_REQ
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_info_req_end - tag_info_req_start
		.long MULTIBOOT2_TAG_CMDLINE
		.long MULTIBOOT2_TAG_MODULE
		.long MULTIBOOT2_TAG_MEMMAP
    #ifdef CONFIG_FB
		.long MULTIBOOT2_TAG_FBINFO
    #endif
	    tag_info_req_end:

## tag_address_start
tag_address_start = 108110

tag_address_end = 108128

    /* Address tag */
	.align 8
	tag_address_start:
		.word MULTIBOOT2_TAG_ADDRESS
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_address_end - tag_address_start
		.long multiboot2_header_start
		.long unmapped_start
		.long 0
		.long 0
	tag_address_end:

## tag_entry_address_start
tag_entry_address_start = 108128

tag_entry_address_end = 108134

	/* Entry address tag */
	.align 8
	tag_entry_address_start:
		.word MULTIBOOT2_TAG_ENTRY_ADDRESS
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_entry_address_end - tag_entry_address_start
		.long multiboot2_image_start
	tag_entry_address_end:

## tag_flags_start
108138 = tag_flags_start

108144 = tag_flags_end

	/* Flags tag */
	.align 8
	tag_flags_start:
		.word MULTIBOOT2_TAG_FLAGS
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_flags_end - tag_flags_start
		.long MULTIBOOT2_FLAGS_CONSOLE
	tag_flags_end:

## tag_framebuffer_start
108148 = tag_framebuffer_start

10815C = tag_framebuffer_end

	#ifdef CONFIG_FB
		/* Framebuffer tag */
		.align 8
		tag_framebuffer_start:
			.word MULTIBOOT2_TAG_FRAMEBUFFER
			.word MULTIBOOT2_FLAGS_REQUIRED
			.long tag_framebuffer_end - tag_framebuffer_start
			.long CONFIG_BFB_WIDTH
			.long CONFIG_BFB_HEIGHT
			.long CONFIG_BFB_BPP
		tag_framebuffer_end:
	#endif

## tag_module_align_start
108160 = tag_module_align_start

10816C = tag_module_align_end

	/* Module alignment tag */
	.align 8
	tag_module_align_start:
		.word MULTIBOOT2_TAG_MODULE_ALIGN
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_module_align_end - tag_module_align_start
		.long 0
	tag_module_align_end:

## tag_terminator_start
108170 = tag_terminator_start

	/* Tag terminator */
	.align 8
	tag_terminator_start:
		.word MULTIBOOT2_TAG_TERMINATOR
		.word MULTIBOOT2_FLAGS_REQUIRED
		.long tag_terminator_end - tag_terminator_start
	tag_terminator_end:

tag_terminator_end = multiboot2_header_end = 108178 = multiboot2_image_start


## 内核代码Boot的开始：multiboot2_image_start
### multiboot2_image_start
multiboot2_image_start = 108178



### pm_error_halt
pm_error_halt = 108221










