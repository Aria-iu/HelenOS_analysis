# unmapped段
unmapped段是在链接脚本里面没有进行映射的段，也就是说，这部分的地址是物理地址，不需要内存映射或者进行直接映射来直接访问。

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

## 

```asm
Disassembly of section .unmapped:

# Comment: #define MULTIBOOT2_HEADER_MAGIC      0xe85250d6
#		   #define MULTIBOOT2_HEADER_ARCH_I386  0
#		   multiboot2_header_end - multiboot2_header_start
#		   -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCH_I386 + (multiboot2_header_end - multiboot2_header_start))
# 这个应该是 multiboot2_header_start ： 也就是操作系统镜像的Multiboot2的头部
# 即		d6 50 52 e8
#			00 00 00 00
#			?? ?? ?? ??  -> 90 00 00 00
#			?? ?? ?? ??  -> 9a ae ad 17
00000000001080e8 <unmapped_start>:
  1080e8:	d6                   	(bad)
  1080e9:	50                   	push   %rax
  1080ea:	52                   	push   %rdx
  1080eb:	e8 00 00 00 00       	call   1080f0 <unmapped_start+0x8>
  1080f0:	90                   	nop
  1080f1:	00 00                	add    %al,(%rax)
  1080f3:	00                   	.byte 0
  1080f4:	9a                   	(bad)
  1080f5:	ae                   	scas   %es:(%rdi),%al
  1080f6:	ad                   	lods   %ds:(%rsi),%eax
  1080f7:	17                   	(bad)

# #define MULTIBOOT2_TAG_INFO_REQ       1
# #define MULTIBOOT2_FLAGS_REQUIRED  0
# tag_info_req_end - tag_info_req_start
# #define MULTIBOOT2_TAG_CMDLINE 1
# #define MULTIBOOT2_TAG_MODULE  3
# #define MULTIBOOT2_TAG_MEMMAP  6
# #define MULTIBOOT2_TAG_FBINFO  8
# 这个应该是 tag_info_req_start 
# 即		01 00 00 00				(.word)
#			18 00 00 00
#			01 00 00 00
#			03 00 00 00
#			06 00 00 00
#			08 00 00 00
00000000001080f8 <tag_info_req_start>:
  1080f8:	01 00                	add    %eax,(%rax)
  1080fa:	00 00                	add    %al,(%rax)
  1080fc:	18 00                	sbb    %al,(%rax)
  1080fe:	00 00                	add    %al,(%rax)
  108100:	01 00                	add    %eax,(%rax)
  108102:	00 00                	add    %al,(%rax)
  108104:	03 00                	add    (%rax),%eax
  108106:	00 00                	add    %al,(%rax)
  108108:	06                   	(bad)
  108109:	00 00                	add    %al,(%rax)
  10810b:	00 08                	add    %cl,(%rax)
  10810d:	00 00                	add    %al,(%rax)
	...

0000000000108110 <tag_address_start>:
  108110:	02 00                	add    (%rax),%al
  108112:	00 00                	add    %al,(%rax)
  108114:	18 00                	sbb    %al,(%rax)
  108116:	00 00                	add    %al,(%rax)
  108118:	e8 80 10 00 e8       	call   ffffffffe810919d <kdata_end+0x67f5e274>
  10811d:	80 10 00             	adcb   $0x0,(%rax)
	...

0000000000108128 <tag_address_end>:
  108128:	03 00                	add    (%rax),%eax
  10812a:	00 00                	add    %al,(%rax)
  10812c:	0c 00                	or     $0x0,%al
  10812e:	00 00                	add    %al,(%rax)
  108130:	78 81                	js     1080b3 <ap_gdtr+0xfeea0>
  108132:	10 00                	adc    %al,(%rax)

0000000000108134 <tag_entry_address_end>:
  108134:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000108138 <tag_flags_start>:
  108138:	04 00                	add    $0x0,%al
  10813a:	00 00                	add    %al,(%rax)
  10813c:	0c 00                	or     $0x0,%al
  10813e:	00 00                	add    %al,(%rax)
  108140:	03 00                	add    (%rax),%eax
	...

0000000000108144 <tag_flags_end>:
  108144:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000108148 <tag_framebuffer_start>:
  108148:	05 00 00 00 14       	add    $0x14000000,%eax
  10814d:	00 00                	add    %al,(%rax)
  10814f:	00 00                	add    %al,(%rax)
  108151:	04 00                	add    $0x0,%al
  108153:	00 00                	add    %al,(%rax)
  108155:	03 00                	add    (%rax),%eax
  108157:	00 10                	add    %dl,(%rax)
  108159:	00 00                	add    %al,(%rax)
	...

000000000010815c <tag_framebuffer_end>:
  10815c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000108160 <tag_module_align_start>:
  108160:	06                   	(bad)
  108161:	00 00                	add    %al,(%rax)
  108163:	00 0c 00             	add    %cl,(%rax,%rax,1)
  108166:	00 00                	add    %al,(%rax)
  108168:	00 00                	add    %al,(%rax)
	...

000000000010816c <tag_module_align_end>:
  10816c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000108170 <tag_terminator_start>:
  108170:	00 00                	add    %al,(%rax)
  108172:	00 00                	add    %al,(%rax)
  108174:	08 00                	or     %al,(%rax)
	...

## 上面是multiboot2标准的操作系统镜像的头部字段，反汇编工具显然不够智能，将其识别为text指令。
## 这里才是操作系统被GRUB正确加载之后的入口。
0000000000108178 <multiboot2_image_start>:
  108178:	fa                   	cli
  108179:	fc                   	cld
  10817a:	bc 00 7c 10 00       	mov    $0x107c00,%esp
  10817f:	0f 01 15 86 90 10 00 	lgdt   0x109086(%rip)        # 21120c <unmapped_end+0xfd20c>
  108186:	0f 01 1d 80 90 10 00 	lidt   0x109080(%rip)        # 21120d <unmapped_end+0xfd20d>
  10818d:	66 b9 10 00          	mov    $0x10,%cx
  108191:	8e c1                	mov    %ecx,%es
  108193:	8e d9                	mov    %ecx,%ds
  108195:	8e d1                	mov    %ecx,%ss
  108197:	66 b9 18 00          	mov    $0x18,%cx
  10819b:	8e e1                	mov    %ecx,%fs
  10819d:	8e e9                	mov    %ecx,%gs
  10819f:	ea                   	(bad)
  1081a0:	a6                   	cmpsb  %es:(%rdi),%ds:(%rsi)
  1081a1:	81                   	.byte 0x81
  1081a2:	10 00                	adc    %al,(%rax)
  1081a4:	28 00                	sub    %al,(%rax)

00000000001081a6 <multiboot2_meeting_point>:
  1081a6:	a3 8c 90 10 00 89 1d 	movabs %eax,0x90901d890010908c
  1081ad:	90 90 
  1081af:	10 00                	adc    %al,(%rax)
  1081b1:	b8 00 00 00 80       	mov    $0x80000000,%eax
  1081b6:	0f a2                	cpuid
  1081b8:	3d 00 00 00 80       	cmp    $0x80000000,%eax
  1081bd:	77 02                	ja     1081c1 <extended_cpuid_supported>
  1081bf:	eb 60                	jmp    108221 <pm_error_halt>

00000000001081c1 <extended_cpuid_supported>:
  1081c1:	b8 01 00 00 80       	mov    $0x80000001,%eax
  1081c6:	0f a2                	cpuid
  1081c8:	0f ba e2 1d          	bt     $0x1d,%edx
  1081cc:	72 02                	jb     1081d0 <long_mode_supported>
  1081ce:	eb 51                	jmp    108221 <pm_error_halt>

00000000001081d0 <long_mode_supported>:
  1081d0:	0f ba e2 14          	bt     $0x14,%edx
  1081d4:	72 02                	jb     1081d8 <noexecute_supported>
  1081d6:	eb 49                	jmp    108221 <pm_error_halt>

00000000001081d8 <noexecute_supported>:
  1081d8:	b8 01 00 00 00       	mov    $0x1,%eax
  1081dd:	0f a2                	cpuid
  1081df:	0f ba e2 18          	bt     $0x18,%edx
  1081e3:	72 02                	jb     1081e7 <fx_supported>
  1081e5:	eb 3a                	jmp    108221 <pm_error_halt>

00000000001081e7 <fx_supported>:
  1081e7:	0f ba e2 1a          	bt     $0x1a,%edx
  1081eb:	72 02                	jb     1081ef <sse2_supported>
  1081ed:	eb 32                	jmp    108221 <pm_error_halt>

00000000001081ef <sse2_supported>:
  1081ef:	0f 20 e0             	mov    %cr4,%rax
  1081f2:	83 c8 20             	or     $0x20,%eax
  1081f5:	0f 22 e0             	mov    %rax,%cr4
  1081f8:	8d 05 00 30 11 00    	lea    0x113000(%rip),%eax        # 21b1fe <unmapped_end+0x1071fe>
  1081fe:	0f 22 d8             	mov    %rax,%cr3
  108201:	b9 80 00 00 c0       	mov    $0xc0000080,%ecx
  108206:	0f 32                	rdmsr
  108208:	0d 00 01 00 00       	or     $0x100,%eax
  10820d:	0f 30                	wrmsr
  10820f:	0f 20 c0             	mov    %cr0,%rax
  108212:	0d 00 00 00 80       	or     $0x80000000,%eax
  108217:	0f 22 c0             	mov    %rax,%cr0
  10821a:	ea                   	(bad)
  10821b:	25 82 10 00 08       	and    $0x8001082,%eax
	...

0000000000108221 <pm_error_halt>:
  108221:	fa                   	cli

0000000000108222 <hlt1>:
  108222:	f4                   	hlt
  108223:	eb fd                	jmp    108222 <hlt1>

0000000000108225 <start64>:
  108225:	48 c7 c4 00 7c 10 80 	mov    $0xffffffff80107c00,%rsp
  10822c:	6a 00                	push   $0x0
  10822e:	48 89 e5             	mov    %rsp,%rbp
  108231:	8b 3c 25 8c 90 10 00 	mov    0x10908c,%edi
  108238:	8b 34 25 90 90 10 00 	mov    0x109090,%esi
  10823f:	e8 2c 93 01 80       	call   ffffffff80121570 <amd64_pre_main>
  108244:	e8 17 da 00 80       	call   ffffffff80115c60 <main_bsp>
  108249:	fa                   	cli

000000000010824a <hlt0>:
  10824a:	f4                   	hlt
  10824b:	eb fd                	jmp    10824a <hlt0>
  10824d:	0f 1f 00             	nopl   (%rax)

0000000000108250 <multiboot_header>:
  108250:	02 b0 ad 1b 03 00    	add    0x31bad(%rax),%dh
  108256:	01 00                	add    %eax,(%rax)
  108258:	fb                   	sti
  108259:	4f 51                	rex.WRXB push %r9
  10825b:	e4 50                	in     $0x50,%al
  10825d:	82                   	(bad)
  10825e:	10 00                	adc    %al,(%rax)
  108260:	e8 80 10 00 00       	call   1092e5 <unmapped_ap_gdtr+0xd2>
  108265:	00 00                	add    %al,(%rax)
  108267:	00 00                	add    %al,(%rax)
  108269:	00 00                	add    %al,(%rax)
  10826b:	00 70 82             	add    %dh,-0x7e(%rax)
  10826e:	10 00                	adc    %al,(%rax)

0000000000108270 <multiboot_image_start>:
  108270:	fa                   	cli
  108271:	fc                   	cld
  108272:	bc 00 7c 10 00       	mov    $0x107c00,%esp
  108277:	0f 01 15 86 90 10 00 	lgdt   0x109086(%rip)        # 211304 <unmapped_end+0xfd304>
  10827e:	0f 01 1d 80 90 10 00 	lidt   0x109080(%rip)        # 211305 <unmapped_end+0xfd305>
  108285:	66 b9 10 00          	mov    $0x10,%cx
  108289:	8e c1                	mov    %ecx,%es
  10828b:	8e d9                	mov    %ecx,%ds
  10828d:	8e d1                	mov    %ecx,%ss
  10828f:	66 b9 18 00          	mov    $0x18,%cx
  108293:	8e e1                	mov    %ecx,%fs
  108295:	8e e9                	mov    %ecx,%gs
  108297:	ea                   	(bad)
  108298:	9e                   	sahf
  108299:	82                   	(bad)
  10829a:	10 00                	adc    %al,(%rax)
  10829c:	28 00                	sub    %al,(%rax)

000000000010829e <multiboot_meeting_point>:
  10829e:	a3 8c 90 10 00 89 1d 	movabs %eax,0x90901d890010908c
  1082a5:	90 90 
  1082a7:	10 00                	adc    %al,(%rax)
  1082a9:	56                   	push   %rsi
  1082aa:	be c3 91 10 00       	mov    $0x1091c3,%esi
  1082af:	e8 1e 02 00 00       	call   1084d2 <pm_early_puts>
  1082b4:	5e                   	pop    %rsi
  1082b5:	b8 00 00 00 80       	mov    $0x80000000,%eax
  1082ba:	0f a2                	cpuid
  1082bc:	3d 00 00 00 80       	cmp    $0x80000000,%eax
  1082c1:	77 0a                	ja     1082cd <extended_cpuid_supported>
  1082c3:	be 94 90 10 00       	mov    $0x109094,%esi
  1082c8:	e9 76 01 00 00       	jmp    108443 <pm_error_halt>

00000000001082cd <extended_cpuid_supported>:
  1082cd:	b8 01 00 00 80       	mov    $0x80000001,%eax
  1082d2:	0f a2                	cpuid
  1082d4:	0f ba e2 1d          	bt     $0x1d,%edx
  1082d8:	72 0a                	jb     1082e4 <long_mode_supported>
  1082da:	be dd 90 10 00       	mov    $0x1090dd,%esi
  1082df:	e9 5f 01 00 00       	jmp    108443 <pm_error_halt>

00000000001082e4 <long_mode_supported>:
  1082e4:	0f ba e2 14          	bt     $0x14,%edx
  1082e8:	72 0a                	jb     1082f4 <noexecute_supported>
  1082ea:	be 13 91 10 00       	mov    $0x109113,%esi
  1082ef:	e9 4f 01 00 00       	jmp    108443 <pm_error_halt>

00000000001082f4 <noexecute_supported>:
  1082f4:	b8 01 00 00 00       	mov    $0x1,%eax
  1082f9:	0f a2                	cpuid
  1082fb:	0f ba e2 18          	bt     $0x18,%edx
  1082ff:	72 0a                	jb     10830b <fx_supported>
  108301:	be 49 91 10 00       	mov    $0x109149,%esi
  108306:	e9 38 01 00 00       	jmp    108443 <pm_error_halt>

000000000010830b <fx_supported>:
  10830b:	0f ba e2 1a          	bt     $0x1a,%edx
  10830f:	72 0a                	jb     10831b <sse2_supported>
  108311:	be 8c 91 10 00       	mov    $0x10918c,%esi
  108316:	e9 28 01 00 00       	jmp    108443 <pm_error_halt>

000000000010831b <sse2_supported>:
  10831b:	56                   	push   %rsi
  10831c:	be cb 91 10 00       	mov    $0x1091cb,%esi
  108321:	e8 ac 01 00 00       	call   1084d2 <pm_early_puts>
  108326:	5e                   	pop    %rsi
  108327:	be ad 85 10 00       	mov    $0x1085ad,%esi
  10832c:	bf 00 00 08 00       	mov    $0x80000,%edi
  108331:	b9 97 06 00 00       	mov    $0x697,%ecx
  108336:	f3 a4                	rep movsb %ds:(%rsi),%es:(%rdi)
  108338:	56                   	push   %rsi
  108339:	be d8 91 10 00       	mov    $0x1091d8,%esi
  10833e:	e8 8f 01 00 00       	call   1084d2 <pm_early_puts>
  108343:	5e                   	pop    %rsi
  108344:	a1 8c 90 10 00 3d 02 	movabs 0xadb0023d0010908c,%eax
  10834b:	b0 ad 
  10834d:	2b 75 44             	sub    0x44(%rbp),%esi
  108350:	8b 1d 90 90 10 00    	mov    0x109090(%rip),%ebx        # 2113e6 <unmapped_end+0xfd3e6>
  108356:	8b 03                	mov    (%rbx),%eax
  108358:	0f ba e0 02          	bt     $0x2,%eax
  10835c:	73 36                	jae    108394 <no_cmdline>
  10835e:	8b 73 10             	mov    0x10(%rbx),%esi

0000000000108361 <skip_loop>:
  108361:	ac                   	lods   %ds:(%rsi),%al
  108362:	3c 00                	cmp    $0x0,%al
  108364:	74 2e                	je     108394 <no_cmdline>
  108366:	3c 20                	cmp    $0x20,%al
  108368:	74 02                	je     10836c <skip_loop_done>
  10836a:	eb f5                	jmp    108361 <skip_loop>

000000000010836c <skip_loop_done>:
  10836c:	8a 06                	mov    (%rsi),%al
  10836e:	3c 00                	cmp    $0x0,%al
  108370:	74 22                	je     108394 <no_cmdline>
  108372:	3c 20                	cmp    $0x20,%al
  108374:	75 03                	jne    108379 <space_loop_done>
  108376:	46 eb f3             	rex.RX jmp 10836c <skip_loop_done>

0000000000108379 <space_loop_done>:
  108379:	bf 00 00 08 00       	mov    $0x80000,%edi
  10837e:	81 c7 52 06 00 00    	add    $0x652,%edi
  108384:	b9 17 00 00 00       	mov    $0x17,%ecx

0000000000108389 <cmd_loop>:
  108389:	ac                   	lods   %ds:(%rsi),%al
  10838a:	aa                   	stos   %al,%es:(%rdi)
  10838b:	3c 00                	cmp    $0x0,%al
  10838d:	74 02                	je     108391 <cmd_loop_done>
  10838f:	e2 f8                	loop   108389 <cmd_loop>

0000000000108391 <cmd_loop_done>:
  108391:	31 c0                	xor    %eax,%eax
  108393:	aa                   	stos   %al,%es:(%rdi)

0000000000108394 <no_cmdline>:
  108394:	56                   	push   %rsi
  108395:	be ed 91 10 00       	mov    $0x1091ed,%esi
  10839a:	e8 33 01 00 00       	call   1084d2 <pm_early_puts>
  10839f:	5e                   	pop    %rsi
  1083a0:	bf 00 00 08 00       	mov    $0x80000,%edi
  1083a5:	ff e7                	jmp    *%rdi

00000000001083a7 <vesa_meeting_point>:
  1083a7:	0f 01 15 86 90 10 00 	lgdt   0x109086(%rip)        # 211434 <unmapped_end+0xfd434>
  1083ae:	0f 01 1d 80 90 10 00 	lidt   0x109080(%rip)        # 211435 <unmapped_end+0xfd435>
  1083b5:	0f b7 c8             	movzwl %ax,%ecx
  1083b8:	89 0d 30 8f 18 00    	mov    %ecx,0x188f30(%rip)        # 2912ee <unmapped_end+0x17d2ee>
  1083be:	c1 e8 10             	shr    $0x10,%eax
  1083c1:	66 a3 34 8f 18 00 0f 	movabs %ax,0x89cbb70f00188f34
  1083c8:	b7 cb 89 
  1083cb:	0d 38 8f 18 00       	or     $0x188f38,%eax
  1083d0:	c1 eb 10             	shr    $0x10,%ebx
  1083d3:	89 1d 3c 8f 18 00    	mov    %ebx,0x188f3c(%rip)        # 291315 <unmapped_end+0x17d315>
  1083d9:	88 15 2b 8f 18 00    	mov    %dl,0x188f2b(%rip)        # 29130a <unmapped_end+0x17d30a>
  1083df:	c1 ea 08             	shr    $0x8,%edx
  1083e2:	88 15 2a 8f 18 00    	mov    %dl,0x188f2a(%rip)        # 291312 <unmapped_end+0x17d312>
  1083e8:	c1 ea 08             	shr    $0x8,%edx
  1083eb:	88 15 2d 8f 18 00    	mov    %dl,0x188f2d(%rip)        # 29131e <unmapped_end+0x17d31e>
  1083f1:	c1 ea 08             	shr    $0x8,%edx
  1083f4:	88 15 2c 8f 18 00    	mov    %dl,0x188f2c(%rip)        # 291326 <unmapped_end+0x17d326>
  1083fa:	89 f2                	mov    %esi,%edx
  1083fc:	88 15 29 8f 18 00    	mov    %dl,0x188f29(%rip)        # 29132b <unmapped_end+0x17d32b>
  108402:	c1 ea 08             	shr    $0x8,%edx
  108405:	88 15 28 8f 18 00    	mov    %dl,0x188f28(%rip)        # 291333 <unmapped_end+0x17d333>
  10840b:	89 3d 40 8f 18 00    	mov    %edi,0x188f40(%rip)        # 291351 <unmapped_end+0x17d351>
  108411:	0f 20 e0             	mov    %cr4,%rax
  108414:	83 c8 20             	or     $0x20,%eax
  108417:	0f 22 e0             	mov    %rax,%cr4
  10841a:	8d 05 00 30 11 00    	lea    0x113000(%rip),%eax        # 21b420 <unmapped_end+0x107420>
  108420:	0f 22 d8             	mov    %rax,%cr3
  108423:	b9 80 00 00 c0       	mov    $0xc0000080,%ecx
  108428:	0f 32                	rdmsr
  10842a:	0d 00 01 00 00       	or     $0x100,%eax
  10842f:	0f 30                	wrmsr
  108431:	0f 20 c0             	mov    %cr0,%rax
  108434:	0d 00 00 00 80       	or     $0x80000000,%eax
  108439:	0f 22 c0             	mov    %rax,%cr0
  10843c:	ea                   	(bad)
  10843d:	68 85 10 00 08       	push   $0x8001085
	...

0000000000108443 <pm_error_halt>:
  108443:	bf 00 80 0b 00       	mov    $0xb8000,%edi
  108448:	31 c0                	xor    %eax,%eax
  10844a:	66 ba d4 03          	mov    $0x3d4,%dx
  10844e:	b0 0e                	mov    $0xe,%al
  108450:	ee                   	out    %al,(%dx)
  108451:	66 ba d5 03          	mov    $0x3d5,%dx
  108455:	ec                   	in     (%dx),%al
  108456:	66 c1 e0 08          	shl    $0x8,%ax
  10845a:	66 ba d4 03          	mov    $0x3d4,%dx
  10845e:	b0 0f                	mov    $0xf,%al
  108460:	ee                   	out    %al,(%dx)
  108461:	66 ba d5 03          	mov    $0x3d5,%dx
  108465:	ec                   	in     (%dx),%al
  108466:	66 3d d0 07          	cmp    $0x7d0,%ax
  10846a:	72 04                	jb     108470 <err_cursor_ok>
  10846c:	66 b8 ce 07          	mov    $0x7ce,%ax

0000000000108470 <err_cursor_ok>:
  108470:	66 89 c3             	mov    %ax,%bx
  108473:	d1 e0                	shl    %eax
  108475:	01 c7                	add    %eax,%edi

0000000000108477 <err_ploop>:
  108477:	ac                   	lods   %ds:(%rsi),%al
  108478:	3c 00                	cmp    $0x0,%al
  10847a:	74 36                	je     1084b2 <err_ploop_end>
  10847c:	b4 0c                	mov    $0xc,%ah
  10847e:	66 ab                	stos   %ax,%es:(%rdi)
  108480:	66 43                	data16 rex.XB
  108482:	66 81 fb d0 07       	cmp    $0x7d0,%bx
  108487:	72 ee                	jb     108477 <err_ploop>
  108489:	89 f2                	mov    %esi,%edx
  10848b:	be a0 80 0b 00       	mov    $0xb80a0,%esi
  108490:	bf 00 80 0b 00       	mov    $0xb8000,%edi
  108495:	b9 c0 03 00 00       	mov    $0x3c0,%ecx
  10849a:	f3 a5                	rep movsl %ds:(%rsi),%es:(%rdi)
  10849c:	31 c0                	xor    %eax,%eax
  10849e:	b9 28 00 00 00       	mov    $0x28,%ecx
  1084a3:	f3 ab                	rep stos %eax,%es:(%rdi)
  1084a5:	89 d6                	mov    %edx,%esi
  1084a7:	bf 00 8f 0b 00       	mov    $0xb8f00,%edi
  1084ac:	66 bb 80 07          	mov    $0x780,%bx
  1084b0:	eb c5                	jmp    108477 <err_ploop>

00000000001084b2 <err_ploop_end>:
  1084b2:	66 ba d4 03          	mov    $0x3d4,%dx
  1084b6:	b0 0e                	mov    $0xe,%al
  1084b8:	ee                   	out    %al,(%dx)
  1084b9:	66 ba d5 03          	mov    $0x3d5,%dx
  1084bd:	88 f8                	mov    %bh,%al
  1084bf:	ee                   	out    %al,(%dx)
  1084c0:	66 ba d4 03          	mov    $0x3d4,%dx
  1084c4:	b0 0f                	mov    $0xf,%al
  1084c6:	ee                   	out    %al,(%dx)
  1084c7:	66 ba d5 03          	mov    $0x3d5,%dx
  1084cb:	88 d8                	mov    %bl,%al
  1084cd:	ee                   	out    %al,(%dx)
  1084ce:	fa                   	cli

00000000001084cf <hlt1>:
  1084cf:	f4                   	hlt
  1084d0:	eb fd                	jmp    1084cf <hlt1>

00000000001084d2 <pm_early_puts>:
  1084d2:	50                   	push   %rax
  1084d3:	53                   	push   %rbx
  1084d4:	51                   	push   %rcx
  1084d5:	52                   	push   %rdx
  1084d6:	57                   	push   %rdi
  1084d7:	bf 00 80 0b 00       	mov    $0xb8000,%edi
  1084dc:	31 c0                	xor    %eax,%eax
  1084de:	66 ba d4 03          	mov    $0x3d4,%dx
  1084e2:	b0 0e                	mov    $0xe,%al
  1084e4:	ee                   	out    %al,(%dx)
  1084e5:	66 ba d5 03          	mov    $0x3d5,%dx
  1084e9:	ec                   	in     (%dx),%al
  1084ea:	66 c1 e0 08          	shl    $0x8,%ax
  1084ee:	66 ba d4 03          	mov    $0x3d4,%dx
  1084f2:	b0 0f                	mov    $0xf,%al
  1084f4:	ee                   	out    %al,(%dx)
  1084f5:	66 ba d5 03          	mov    $0x3d5,%dx
  1084f9:	ec                   	in     (%dx),%al
  1084fa:	66 3d d0 07          	cmp    $0x7d0,%ax
  1084fe:	72 04                	jb     108504 <pm_puts_cursor_ok>
  108500:	66 b8 ce 07          	mov    $0x7ce,%ax

0000000000108504 <pm_puts_cursor_ok>:
  108504:	66 89 c3             	mov    %ax,%bx
  108507:	d1 e0                	shl    %eax
  108509:	01 c7                	add    %eax,%edi

000000000010850b <pm_puts_ploop>:
  10850b:	ac                   	lods   %ds:(%rsi),%al
  10850c:	3c 00                	cmp    $0x0,%al
  10850e:	74 36                	je     108546 <pm_puts_ploop_end>
  108510:	b4 0a                	mov    $0xa,%ah
  108512:	66 ab                	stos   %ax,%es:(%rdi)
  108514:	66 43                	data16 rex.XB
  108516:	66 81 fb d0 07       	cmp    $0x7d0,%bx
  10851b:	72 ee                	jb     10850b <pm_puts_ploop>
  10851d:	89 f2                	mov    %esi,%edx
  10851f:	be a0 80 0b 00       	mov    $0xb80a0,%esi
  108524:	bf 00 80 0b 00       	mov    $0xb8000,%edi
  108529:	b9 c0 03 00 00       	mov    $0x3c0,%ecx
  10852e:	f3 a5                	rep movsl %ds:(%rsi),%es:(%rdi)
  108530:	31 c0                	xor    %eax,%eax
  108532:	b9 28 00 00 00       	mov    $0x28,%ecx
  108537:	f3 ab                	rep stos %eax,%es:(%rdi)
  108539:	89 d6                	mov    %edx,%esi
  10853b:	bf 00 8f 0b 00       	mov    $0xb8f00,%edi
  108540:	66 bb 80 07          	mov    $0x780,%bx
  108544:	eb c5                	jmp    10850b <pm_puts_ploop>

0000000000108546 <pm_puts_ploop_end>:
  108546:	66 ba d4 03          	mov    $0x3d4,%dx
  10854a:	b0 0e                	mov    $0xe,%al
  10854c:	ee                   	out    %al,(%dx)
  10854d:	66 ba d5 03          	mov    $0x3d5,%dx
  108551:	88 f8                	mov    %bh,%al
  108553:	ee                   	out    %al,(%dx)
  108554:	66 ba d4 03          	mov    $0x3d4,%dx
  108558:	b0 0f                	mov    $0xf,%al
  10855a:	ee                   	out    %al,(%dx)
  10855b:	66 ba d5 03          	mov    $0x3d5,%dx
  10855f:	88 d8                	mov    %bl,%al
  108561:	ee                   	out    %al,(%dx)
  108562:	5f                   	pop    %rdi
  108563:	5a                   	pop    %rdx
  108564:	59                   	pop    %rcx
  108565:	5b                   	pop    %rbx
  108566:	58                   	pop    %rax
  108567:	c3                   	ret

0000000000108568 <start64>:
  108568:	48 c7 c4 00 7c 10 80 	mov    $0xffffffff80107c00,%rsp
  10856f:	6a 00                	push   $0x0
  108571:	48 89 e5             	mov    %rsp,%rbp
  108574:	57                   	push   %rdi
  108575:	48 c7 c7 03 92 10 00 	mov    $0x109203,%rdi
  10857c:	e8 2b 00 00 00       	call   1085ac <early_puts>
  108581:	5f                   	pop    %rdi
  108582:	8b 3c 25 8c 90 10 00 	mov    0x10908c,%edi
  108589:	8b 34 25 90 90 10 00 	mov    0x109090,%esi
  108590:	e8 db 8f 01 80       	call   ffffffff80121570 <amd64_pre_main>
  108595:	57                   	push   %rdi
  108596:	48 c7 c7 0b 92 10 00 	mov    $0x10920b,%rdi
  10859d:	e8 0a 00 00 00       	call   1085ac <early_puts>
  1085a2:	5f                   	pop    %rdi
  1085a3:	e8 b8 d6 00 80       	call   ffffffff80115c60 <main_bsp>
  1085a8:	fa                   	cli

00000000001085a9 <hlt0>:
  1085a9:	f4                   	hlt
  1085aa:	eb fd                	jmp    1085a9 <hlt0>

00000000001085ac <early_puts>:
  1085ac:	c3                   	ret

00000000001085ad <vesa_init>:
  1085ad:	0f 01 1d c9 85 10 00 	lidt   0x1085c9(%rip)        # 210b7d <unmapped_end+0xfcb7d>
  1085b4:	66 bb 48 00          	mov    $0x48,%bx
  1085b8:	8e c3                	mov    %ebx,%es
  1085ba:	8e e3                	mov    %ebx,%fs
  1085bc:	8e eb                	mov    %ebx,%gs
  1085be:	8e db                	mov    %ebx,%ds
  1085c0:	8e d3                	mov    %ebx,%ss
  1085c2:	ea                   	(bad)
  1085c3:	22 00                	and    (%rax),%al
  1085c5:	00 00                	add    %al,(%rax)
  1085c7:	40                   	rex
	...

00000000001085c9 <vesa_idtr>:
  1085c9:	ff 03                	incl   (%rbx)
  1085cb:	00 00                	add    %al,(%rax)
	...

00000000001085cf <vesa_init_real>:
  1085cf:	0f 20 c0             	mov    %cr0,%rax
  1085d2:	66 83 e0 fe          	and    $0xfffe,%ax
  1085d6:	0f 22 c0             	mov    %rax,%cr0
  1085d9:	ea                   	(bad)
  1085da:	31 00                	xor    %eax,(%rax)
  1085dc:	00                   	.byte 0
  1085dd:	80                   	.byte 0x80

00000000001085de <vesa_init_real2>:
  1085de:	bb 00 80 8e c3       	mov    $0xc38e8000,%ebx
  1085e3:	8e e3                	mov    %ebx,%fs
  1085e5:	8e eb                	mov    %ebx,%gs
  1085e7:	8e db                	mov    %ebx,%ds
  1085e9:	8e d3                	mov    %ebx,%ss
  1085eb:	66 89 e0             	mov    %sp,%ax
  1085ee:	66 bc fc ff          	mov    $0xfffc,%sp
  1085f2:	00 00                	add    %al,(%rax)
  1085f4:	66 bd fc ff          	mov    $0xfffc,%bp
  1085f8:	00 00                	add    %al,(%rax)
  1085fa:	66 50                	push   %ax
  1085fc:	bf 52 06 66 31       	mov    $0x31660652,%edi
  108601:	c0 66 31 db          	shlb   $0xdb,0x31(%rsi)
  108605:	66 b9 08 00          	mov    $0x8,%cx
	...

000000000010860b <parse_width>:
  10860b:	8a 05 3c 30 72 1b    	mov    0x1b72303c(%rip),%al        # 1b82b64d <unmapped_end+0x1b71764d>
  108611:	3c 39                	cmp    $0x39,%al
  108613:	77 17                	ja     10862c <parse_width_done>
  108615:	2c 30                	sub    $0x30,%al
  108617:	8b 1e                	mov    (%rsi),%ebx
  108619:	4d 06                	rex.WRB (bad)
  10861b:	67 66 8d 1c 9b       	lea    (%ebx,%ebx,4),%bx
  108620:	66 d1 e3             	shl    %bx
  108623:	01 c3                	add    %eax,%ebx
  108625:	89 1e                	mov    %ebx,(%rsi)
  108627:	4d 06                	rex.WRB (bad)
  108629:	47 e2 df             	rex.RXB loop 10860b <parse_width>

000000000010862c <parse_width_done>:
  10862c:	8a 05 3c 00 74 56    	mov    0x5674003c(%rip),%al        # 5684866e <unmapped_end+0x5673466e>
  108632:	47                   	rex.RXB
  108633:	66 b9 08 00          	mov    $0x8,%cx
	...

0000000000108639 <parse_height>:
  108639:	8a 05 3c 30 72 1b    	mov    0x1b72303c(%rip),%al        # 1b82b67b <unmapped_end+0x1b71767b>
  10863f:	3c 39                	cmp    $0x39,%al
  108641:	77 17                	ja     10865a <parse_height_done>
  108643:	2c 30                	sub    $0x30,%al
  108645:	8b 1e                	mov    (%rsi),%ebx
  108647:	4f 06                	rex.WRXB (bad)
  108649:	67 66 8d 1c 9b       	lea    (%ebx,%ebx,4),%bx
  10864e:	66 d1 e3             	shl    %bx
  108651:	01 c3                	add    %eax,%ebx
  108653:	89 1e                	mov    %ebx,(%rsi)
  108655:	4f 06                	rex.WRXB (bad)
  108657:	47 e2 df             	rex.RXB loop 108639 <parse_height>

000000000010865a <parse_height_done>:
  10865a:	8a 05 3c 00 74 28    	mov    0x2874003c(%rip),%al        # 2884869c <unmapped_end+0x2873469c>
  108660:	47                   	rex.RXB
  108661:	66 b9 04 00          	mov    $0x4,%cx
	...

0000000000108667 <parse_bpp>:
  108667:	8a 05 3c 30 72 1b    	mov    0x1b72303c(%rip),%al        # 1b82b6a9 <unmapped_end+0x1b7176a9>
  10866d:	3c 39                	cmp    $0x39,%al
  10866f:	77 17                	ja     108688 <parse_bpp_done>
  108671:	2c 30                	sub    $0x30,%al
  108673:	8b 1e                	mov    (%rsi),%ebx
  108675:	51                   	push   %rcx
  108676:	06                   	(bad)
  108677:	67 66 8d 1c 9b       	lea    (%ebx,%ebx,4),%bx
  10867c:	66 d1 e3             	shl    %bx
  10867f:	01 c3                	add    %eax,%ebx
  108681:	89 1e                	mov    %ebx,(%rsi)
  108683:	51                   	push   %rcx
  108684:	06                   	(bad)
  108685:	47 e2 df             	rex.RXB loop 108667 <parse_bpp>

0000000000108688 <parse_bpp_done>:
  108688:	b8 00 4f bf 97       	mov    $0x97bf4f00,%eax
  10868d:	06                   	(bad)
  10868e:	57                   	push   %rdi
  10868f:	66 c7 05 56 42 45 32 	movw   $0x10cd,0x32454256(%rip)        # 3255c8ee <unmapped_end+0x324488ee>
  108696:	cd 10 
  108698:	5f                   	pop    %rdi
  108699:	83 f8 4f             	cmp    $0x4f,%eax
  10869c:	0f 85 0c 01 8b 75    	jne    759b87ae <unmapped_end+0x758a47ae>
  1086a2:	10 8e ee 8b 75 0e    	adc    %cl,0xe758bee(%rsi)
  1086a8:	81                   	.byte 0x81
  1086a9:	c7                   	.byte 0xc7
  1086aa:	00                   	.byte 0
  1086ab:	04                   	.byte 0x4

00000000001086ac <next_mode>:
  1086ac:	65 8b 0c 83          	mov    %gs:(%rbx,%rax,4),%ecx
  1086b0:	f9                   	stc
  1086b1:	ff 0f                	decl   (%rdi)
  1086b3:	84 f6                	test   %dh,%dh
  1086b5:	00 46 46             	add    %al,0x46(%rsi)
  1086b8:	51                   	push   %rcx
  1086b9:	57                   	push   %rdi
  1086ba:	56                   	push   %rsi
  1086bb:	b8 01 4f cd 10       	mov    $0x10cd4f01,%eax
  1086c0:	5e                   	pop    %rsi
  1086c1:	5f                   	pop    %rdi
  1086c2:	59                   	pop    %rcx
  1086c3:	83 f8 4f             	cmp    $0x4f,%eax
  1086c6:	0f 85 e2 00 8b 05    	jne    59b87ae <unmapped_end+0x58a47ae>
  1086cc:	25 99 00 3d 99       	and    $0x993d0099,%eax
  1086d1:	00 75 d8             	add    %dh,-0x28(%rbp)
  1086d4:	a1 4d 06 3b 45 12 75 	movabs 0xa1d07512453b064d,%eax
  1086db:	d0 a1 
  1086dd:	4f 06                	rex.WRXB (bad)
  1086df:	3b 45 14             	cmp    0x14(%rbp),%eax
  1086e2:	75 c8                	jne    1086ac <next_mode>
  1086e4:	a0 51 06 3a 45 19 74 	movabs 0xb00f7419453a0651,%al
  1086eb:	0f b0 
  1086ed:	18 3a                	sbb    %bh,(%rdx)
  1086ef:	06                   	(bad)
  1086f0:	51                   	push   %rcx
  1086f1:	06                   	(bad)
  1086f2:	75 b8                	jne    1086ac <next_mode>
  1086f4:	b0 20                	mov    $0x20,%al
  1086f6:	3a 45 19             	cmp    0x19(%rbp),%al
  1086f9:	75 b1                	jne    1086ac <next_mode>

00000000001086fb <set_mode>:
  1086fb:	89 cb                	mov    %ecx,%ebx
  1086fd:	81 cb 00 c0 57 b8    	or     $0xb857c000,%ebx
  108703:	02 4f cd             	add    -0x33(%rdi),%cl
  108706:	10 5f 83             	adc    %bl,-0x7d(%rdi)
  108709:	f8                   	clc
  10870a:	4f 0f 85 9d 00 8a 45 	rex.WRXB jne 459a87ae <unmapped_end+0x458947ae>
  108711:	19 3c 08             	sbb    %edi,(%rax,%rcx,1)
  108714:	75 43                	jne    108759 <vga_not_set>
  108716:	8b 05 57 bf 4d 02    	mov    0x24dbf57(%rip),%eax        # 25e4673 <unmapped_end+0x24d0673>
  10871c:	66 b9 00 01          	mov    $0x100,%cx
  108720:	00 00                	add    %al,(%rax)
  108722:	0f ba e0 05          	bt     $0x5,%eax
  108726:	73 0e                	jae    108736 <vga_compat>
  108728:	b8 09 4f 30 db       	mov    $0xdb304f09,%eax
  10872d:	31 d2                	xor    %edx,%edx
  10872f:	cd 10                	int    $0x10
  108731:	80 fc 00             	cmp    $0x0,%ah
  108734:	74 22                	je     108758 <vga_not_compat>

0000000000108736 <vga_compat>:
  108736:	ba c6 03 b0 ff       	mov    $0xffb003c6,%edx
  10873b:	ee                   	out    %al,(%dx)
  10873c:	ba c8 03 30 c0       	mov    $0xc03003c8,%edx
  108741:	ee                   	out    %al,(%dx)
  108742:	ba                   	.byte 0xba
  108743:	c9                   	leave
  108744:	03                   	.byte 0x3

0000000000108745 <vga_loop>:
  108745:	26 8a 45 02          	es mov 0x2(%rbp),%al
  108749:	ee                   	out    %al,(%dx)
  10874a:	26 8a 45 01          	es mov 0x1(%rbp),%al
  10874e:	ee                   	out    %al,(%dx)
  10874f:	26 8a 05 ee 83 c7 04 	es mov 0x4c783ee(%rip),%al        # 4d80b44 <unmapped_end+0x4c6cb44>
  108756:	e2 ed                	loop   108745 <vga_loop>

0000000000108758 <vga_not_compat>:
  108758:	5f                   	pop    %rdi

0000000000108759 <vga_not_set>:
  108759:	8a 45 19             	mov    0x19(%rbp),%al
  10875c:	30 e4                	xor    %ah,%ah
  10875e:	66 c1 e0 10          	shl    $0x10,%ax
  108762:	8b 45 10             	mov    0x10(%rbp),%eax
  108765:	8b 5d 12             	mov    0x12(%rbp),%ebx
  108768:	66 c1 e3 10          	shl    $0x10,%bx
  10876c:	8b 5d 14             	mov    0x14(%rbp),%ebx
  10876f:	8a 55 23             	mov    0x23(%rbp),%dl
  108772:	66 c1 e2 08          	shl    $0x8,%dx
  108776:	8a 55 24             	mov    0x24(%rbp),%dl
  108779:	66 89 d6             	mov    %dx,%si
  10877c:	8a 55 1f             	mov    0x1f(%rbp),%dl
  10877f:	66 c1 e2 08          	shl    $0x8,%dx
  108783:	8a 55 20             	mov    0x20(%rbp),%dl
  108786:	66 c1 e2 08          	shl    $0x8,%dx
  10878a:	8a 55 21             	mov    0x21(%rbp),%dl
  10878d:	66 c1 e2 08          	shl    $0x8,%dx
  108791:	8a 55 22             	mov    0x22(%rbp),%dl
  108794:	66 8b 7d 28          	mov    0x28(%rbp),%di

0000000000108798 <vesa_leave_real>:
  108798:	0f 20 c1             	mov    %cr0,%rcx
  10879b:	66 83 c9 01          	or     $0x1,%cx
  10879f:	0f 22 c1             	mov    %rcx,%cr0
  1087a2:	eb 00                	jmp    1087a4 <vesa_leave_real2>

00000000001087a4 <vesa_leave_real2>:
  1087a4:	66 ea                	data16 (bad)
  1087a6:	76 06                	jbe    1087ae <no_mode+0x2>
  1087a8:	08 00                	or     %al,(%rax)
  1087aa:	28 00                	sub    %al,(%rax)

00000000001087ac <no_mode>:
  1087ac:	57                   	push   %rdi
  1087ad:	bf 97 06 8b 75       	mov    $0x758b0697,%edi
  1087b2:	10 8e ee 8b 75 0e    	adc    %cl,0xe758bee(%rsi)
  1087b8:	5f                   	pop    %rdi

00000000001087b9 <find_fallback_mode>:
  1087b9:	65 8b 0c 81          	mov    %gs:(%rcx,%rax,4),%ecx
  1087bd:	f9                   	stc
  1087be:	11 01                	adc    %eax,(%rcx)
  1087c0:	74 09                	je     1087cb <fallback_mode_listed>
  1087c2:	83 f9 ff             	cmp    $0xffffffff,%ecx
  1087c5:	74 20                	je     1087e7 <text_mode>
  1087c7:	46                   	rex.RX
  1087c8:	46 eb ee             	rex.RX jmp 1087b9 <find_fallback_mode>

00000000001087cb <fallback_mode_listed>:
  1087cb:	b9 11 01 57 51       	mov    $0x51570111,%ecx
  1087d0:	b8 01 4f cd 10       	mov    $0x10cd4f01,%eax
  1087d5:	59                   	pop    %rcx
  1087d6:	5f                   	pop    %rdi
  1087d7:	83 f8 4f             	cmp    $0x4f,%eax
  1087da:	75 0b                	jne    1087e7 <text_mode>
  1087dc:	25 99 00 3d 99       	and    $0x993d0099,%eax
  1087e1:	00 75 03             	add    %dh,0x3(%rbp)
  1087e4:	e9                   	.byte 0xe9
  1087e5:	14 ff                	adc    $0xff,%al

00000000001087e7 <text_mode>:
  1087e7:	b8 03 00 cd 10       	mov    $0x10cd0003,%eax
  1087ec:	66 31 c0             	xor    %ax,%ax
  1087ef:	66 31 db             	xor    %bx,%bx
  1087f2:	66 31 d2             	xor    %dx,%dx
  1087f5:	66 31 ff             	xor    %di,%di
  1087f8:	74 9e                	je     108798 <vesa_leave_real>

00000000001087fa <vga323>:
  1087fa:	3f                   	(bad)
  1087fb:	3f                   	(bad)
  1087fc:	3f                   	(bad)
  1087fd:	00 36                	add    %dh,(%rsi)
  1087ff:	3f                   	(bad)
  108800:	3f                   	(bad)
  108801:	00 2d 3f 3f 00 24    	add    %ch,0x24003f3f(%rip)        # 2410c746 <unmapped_end+0x23ff8746>
  108807:	3f                   	(bad)
  108808:	3f                   	(bad)
  108809:	00 1b                	add    %bl,(%rbx)
  10880b:	3f                   	(bad)
  10880c:	3f                   	(bad)
  10880d:	00 12                	add    %dl,(%rdx)
  10880f:	3f                   	(bad)
  108810:	3f                   	(bad)
  108811:	00 09                	add    %cl,(%rcx)
  108813:	3f                   	(bad)
  108814:	3f                   	(bad)
  108815:	00 00                	add    %al,(%rax)
  108817:	3f                   	(bad)
  108818:	3f                   	(bad)
  108819:	00 3f                	add    %bh,(%rdi)
  10881b:	2a 3f                	sub    (%rdi),%bh
  10881d:	00 36                	add    %dh,(%rsi)
  10881f:	2a 3f                	sub    (%rdi),%bh
  108821:	00 2d 2a 3f 00 24    	add    %ch,0x24003f2a(%rip)        # 2410c751 <unmapped_end+0x23ff8751>
  108827:	2a 3f                	sub    (%rdi),%bh
  108829:	00 1b                	add    %bl,(%rbx)
  10882b:	2a 3f                	sub    (%rdi),%bh
  10882d:	00 12                	add    %dl,(%rdx)
  10882f:	2a 3f                	sub    (%rdi),%bh
  108831:	00 09                	add    %cl,(%rcx)
  108833:	2a 3f                	sub    (%rdi),%bh
  108835:	00 00                	add    %al,(%rax)
  108837:	2a 3f                	sub    (%rdi),%bh
  108839:	00 3f                	add    %bh,(%rdi)
  10883b:	15 3f 00 36 15       	adc    $0x1536003f,%eax
  108840:	3f                   	(bad)
  108841:	00 2d 15 3f 00 24    	add    %ch,0x24003f15(%rip)        # 2410c75c <unmapped_end+0x23ff875c>
  108847:	15 3f 00 1b 15       	adc    $0x151b003f,%eax
  10884c:	3f                   	(bad)
  10884d:	00 12                	add    %dl,(%rdx)
  10884f:	15 3f 00 09 15       	adc    $0x1509003f,%eax
  108854:	3f                   	(bad)
  108855:	00 00                	add    %al,(%rax)
  108857:	15 3f 00 3f 00       	adc    $0x3f003f,%eax
  10885c:	3f                   	(bad)
  10885d:	00 36                	add    %dh,(%rsi)
  10885f:	00 3f                	add    %bh,(%rdi)
  108861:	00 2d 00 3f 00 24    	add    %ch,0x24003f00(%rip)        # 2410c767 <unmapped_end+0x23ff8767>
  108867:	00 3f                	add    %bh,(%rdi)
  108869:	00 1b                	add    %bl,(%rbx)
  10886b:	00 3f                	add    %bh,(%rdi)
  10886d:	00 12                	add    %dl,(%rdx)
  10886f:	00 3f                	add    %bh,(%rdi)
  108871:	00 09                	add    %cl,(%rcx)
  108873:	00 3f                	add    %bh,(%rdi)
  108875:	00 00                	add    %al,(%rax)
  108877:	00 3f                	add    %bh,(%rdi)
  108879:	00 3f                	add    %bh,(%rdi)
  10887b:	3f                   	(bad)
  10887c:	36 00 36             	ss add %dh,(%rsi)
  10887f:	3f                   	(bad)
  108880:	36 00 2d 3f 36 00 24 	ss add %ch,0x2400363f(%rip)        # 2410bec6 <unmapped_end+0x23ff7ec6>
  108887:	3f                   	(bad)
  108888:	36 00 1b             	ss add %bl,(%rbx)
  10888b:	3f                   	(bad)
  10888c:	36 00 12             	ss add %dl,(%rdx)
  10888f:	3f                   	(bad)
  108890:	36 00 09             	ss add %cl,(%rcx)
  108893:	3f                   	(bad)
  108894:	36 00 00             	ss add %al,(%rax)
  108897:	3f                   	(bad)
  108898:	36 00 3f             	ss add %bh,(%rdi)
  10889b:	2a 36                	sub    (%rsi),%dh
  10889d:	00 36                	add    %dh,(%rsi)
  10889f:	2a 36                	sub    (%rsi),%dh
  1088a1:	00 2d 2a 36 00 24    	add    %ch,0x2400362a(%rip)        # 2410bed1 <unmapped_end+0x23ff7ed1>
  1088a7:	2a 36                	sub    (%rsi),%dh
  1088a9:	00 1b                	add    %bl,(%rbx)
  1088ab:	2a 36                	sub    (%rsi),%dh
  1088ad:	00 12                	add    %dl,(%rdx)
  1088af:	2a 36                	sub    (%rsi),%dh
  1088b1:	00 09                	add    %cl,(%rcx)
  1088b3:	2a 36                	sub    (%rsi),%dh
  1088b5:	00 00                	add    %al,(%rax)
  1088b7:	2a 36                	sub    (%rsi),%dh
  1088b9:	00 3f                	add    %bh,(%rdi)
  1088bb:	15 36 00 36 15       	adc    $0x15360036,%eax
  1088c0:	36 00 2d 15 36 00 24 	ss add %ch,0x24003615(%rip)        # 2410bedc <unmapped_end+0x23ff7edc>
  1088c7:	15 36 00 1b 15       	adc    $0x151b0036,%eax
  1088cc:	36 00 12             	ss add %dl,(%rdx)
  1088cf:	15 36 00 09 15       	adc    $0x15090036,%eax
  1088d4:	36 00 00             	ss add %al,(%rax)
  1088d7:	15 36 00 3f 00       	adc    $0x3f0036,%eax
  1088dc:	36 00 36             	ss add %dh,(%rsi)
  1088df:	00 36                	add    %dh,(%rsi)
  1088e1:	00 2d 00 36 00 24    	add    %ch,0x24003600(%rip)        # 2410bee7 <unmapped_end+0x23ff7ee7>
  1088e7:	00 36                	add    %dh,(%rsi)
  1088e9:	00 1b                	add    %bl,(%rbx)
  1088eb:	00 36                	add    %dh,(%rsi)
  1088ed:	00 12                	add    %dl,(%rdx)
  1088ef:	00 36                	add    %dh,(%rsi)
  1088f1:	00 09                	add    %cl,(%rcx)
  1088f3:	00 36                	add    %dh,(%rsi)
  1088f5:	00 00                	add    %al,(%rax)
  1088f7:	00 36                	add    %dh,(%rsi)
  1088f9:	00 3f                	add    %bh,(%rdi)
  1088fb:	3f                   	(bad)
  1088fc:	2d 00 36 3f 2d       	sub    $0x2d3f3600,%eax
  108901:	00 2d 3f 2d 00 24    	add    %ch,0x24002d3f(%rip)        # 2410b646 <unmapped_end+0x23ff7646>
  108907:	3f                   	(bad)
  108908:	2d 00 1b 3f 2d       	sub    $0x2d3f1b00,%eax
  10890d:	00 12                	add    %dl,(%rdx)
  10890f:	3f                   	(bad)
  108910:	2d 00 09 3f 2d       	sub    $0x2d3f0900,%eax
  108915:	00 00                	add    %al,(%rax)
  108917:	3f                   	(bad)
  108918:	2d 00 3f 2a 2d       	sub    $0x2d2a3f00,%eax
  10891d:	00 36                	add    %dh,(%rsi)
  10891f:	2a 2d 00 2d 2a 2d    	sub    0x2d2a2d00(%rip),%ch        # 2d3ab625 <unmapped_end+0x2d297625>
  108925:	00 24 2a             	add    %ah,(%rdx,%rbp,1)
  108928:	2d 00 1b 2a 2d       	sub    $0x2d2a1b00,%eax
  10892d:	00 12                	add    %dl,(%rdx)
  10892f:	2a 2d 00 09 2a 2d    	sub    0x2d2a0900(%rip),%ch        # 2d3a9235 <unmapped_end+0x2d295235>
  108935:	00 00                	add    %al,(%rax)
  108937:	2a 2d 00 3f 15 2d    	sub    0x2d153f00(%rip),%ch        # 2d25c83d <unmapped_end+0x2d14883d>
  10893d:	00 36                	add    %dh,(%rsi)
  10893f:	15 2d 00 2d 15       	adc    $0x152d002d,%eax
  108944:	2d 00 24 15 2d       	sub    $0x2d152400,%eax
  108949:	00 1b                	add    %bl,(%rbx)
  10894b:	15 2d 00 12 15       	adc    $0x1512002d,%eax
  108950:	2d 00 09 15 2d       	sub    $0x2d150900,%eax
  108955:	00 00                	add    %al,(%rax)
  108957:	15 2d 00 3f 00       	adc    $0x3f002d,%eax
  10895c:	2d 00 36 00 2d       	sub    $0x2d003600,%eax
  108961:	00 2d 00 2d 00 24    	add    %ch,0x24002d00(%rip)        # 2410b667 <unmapped_end+0x23ff7667>
  108967:	00 2d 00 1b 00 2d    	add    %ch,0x2d001b00(%rip)        # 2d10a46d <unmapped_end+0x2cff646d>
  10896d:	00 12                	add    %dl,(%rdx)
  10896f:	00 2d 00 09 00 2d    	add    %ch,0x2d000900(%rip)        # 2d109275 <unmapped_end+0x2cff5275>
  108975:	00 00                	add    %al,(%rax)
  108977:	00 2d 00 3f 3f 24    	add    %ch,0x243f3f00(%rip)        # 244fc87d <unmapped_end+0x243e887d>
  10897d:	00 36                	add    %dh,(%rsi)
  10897f:	3f                   	(bad)
  108980:	24 00                	and    $0x0,%al
  108982:	2d 3f 24 00 24       	sub    $0x2400243f,%eax
  108987:	3f                   	(bad)
  108988:	24 00                	and    $0x0,%al
  10898a:	1b 3f                	sbb    (%rdi),%edi
  10898c:	24 00                	and    $0x0,%al
  10898e:	12 3f                	adc    (%rdi),%bh
  108990:	24 00                	and    $0x0,%al
  108992:	09 3f                	or     %edi,(%rdi)
  108994:	24 00                	and    $0x0,%al
  108996:	00 3f                	add    %bh,(%rdi)
  108998:	24 00                	and    $0x0,%al
  10899a:	3f                   	(bad)
  10899b:	2a 24 00             	sub    (%rax,%rax,1),%ah
  10899e:	36 2a 24 00          	ss sub (%rax,%rax,1),%ah
  1089a2:	2d 2a 24 00 24       	sub    $0x2400242a,%eax
  1089a7:	2a 24 00             	sub    (%rax,%rax,1),%ah
  1089aa:	1b 2a                	sbb    (%rdx),%ebp
  1089ac:	24 00                	and    $0x0,%al
  1089ae:	12 2a                	adc    (%rdx),%ch
  1089b0:	24 00                	and    $0x0,%al
  1089b2:	09 2a                	or     %ebp,(%rdx)
  1089b4:	24 00                	and    $0x0,%al
  1089b6:	00 2a                	add    %ch,(%rdx)
  1089b8:	24 00                	and    $0x0,%al
  1089ba:	3f                   	(bad)
  1089bb:	15 24 00 36 15       	adc    $0x15360024,%eax
  1089c0:	24 00                	and    $0x0,%al
  1089c2:	2d 15 24 00 24       	sub    $0x24002415,%eax
  1089c7:	15 24 00 1b 15       	adc    $0x151b0024,%eax
  1089cc:	24 00                	and    $0x0,%al
  1089ce:	12 15 24 00 09 15    	adc    0x15090024(%rip),%dl        # 151989f8 <unmapped_end+0x150849f8>
  1089d4:	24 00                	and    $0x0,%al
  1089d6:	00 15 24 00 3f 00    	add    %dl,0x3f0024(%rip)        # 4f8a00 <unmapped_end+0x3e4a00>
  1089dc:	24 00                	and    $0x0,%al
  1089de:	36 00 24 00          	ss add %ah,(%rax,%rax,1)
  1089e2:	2d 00 24 00 24       	sub    $0x24002400,%eax
  1089e7:	00 24 00             	add    %ah,(%rax,%rax,1)
  1089ea:	1b 00                	sbb    (%rax),%eax
  1089ec:	24 00                	and    $0x0,%al
  1089ee:	12 00                	adc    (%rax),%al
  1089f0:	24 00                	and    $0x0,%al
  1089f2:	09 00                	or     %eax,(%rax)
  1089f4:	24 00                	and    $0x0,%al
  1089f6:	00 00                	add    %al,(%rax)
  1089f8:	24 00                	and    $0x0,%al
  1089fa:	3f                   	(bad)
  1089fb:	3f                   	(bad)
  1089fc:	1b 00                	sbb    (%rax),%eax
  1089fe:	36 3f                	ss (bad)
  108a00:	1b 00                	sbb    (%rax),%eax
  108a02:	2d 3f 1b 00 24       	sub    $0x24001b3f,%eax
  108a07:	3f                   	(bad)
  108a08:	1b 00                	sbb    (%rax),%eax
  108a0a:	1b 3f                	sbb    (%rdi),%edi
  108a0c:	1b 00                	sbb    (%rax),%eax
  108a0e:	12 3f                	adc    (%rdi),%bh
  108a10:	1b 00                	sbb    (%rax),%eax
  108a12:	09 3f                	or     %edi,(%rdi)
  108a14:	1b 00                	sbb    (%rax),%eax
  108a16:	00 3f                	add    %bh,(%rdi)
  108a18:	1b 00                	sbb    (%rax),%eax
  108a1a:	3f                   	(bad)
  108a1b:	2a 1b                	sub    (%rbx),%bl
  108a1d:	00 36                	add    %dh,(%rsi)
  108a1f:	2a 1b                	sub    (%rbx),%bl
  108a21:	00 2d 2a 1b 00 24    	add    %ch,0x24001b2a(%rip)        # 2410a551 <unmapped_end+0x23ff6551>
  108a27:	2a 1b                	sub    (%rbx),%bl
  108a29:	00 1b                	add    %bl,(%rbx)
  108a2b:	2a 1b                	sub    (%rbx),%bl
  108a2d:	00 12                	add    %dl,(%rdx)
  108a2f:	2a 1b                	sub    (%rbx),%bl
  108a31:	00 09                	add    %cl,(%rcx)
  108a33:	2a 1b                	sub    (%rbx),%bl
  108a35:	00 00                	add    %al,(%rax)
  108a37:	2a 1b                	sub    (%rbx),%bl
  108a39:	00 3f                	add    %bh,(%rdi)
  108a3b:	15 1b 00 36 15       	adc    $0x1536001b,%eax
  108a40:	1b 00                	sbb    (%rax),%eax
  108a42:	2d 15 1b 00 24       	sub    $0x24001b15,%eax
  108a47:	15 1b 00 1b 15       	adc    $0x151b001b,%eax
  108a4c:	1b 00                	sbb    (%rax),%eax
  108a4e:	12 15 1b 00 09 15    	adc    0x1509001b(%rip),%dl        # 15198a6f <unmapped_end+0x15084a6f>
  108a54:	1b 00                	sbb    (%rax),%eax
  108a56:	00 15 1b 00 3f 00    	add    %dl,0x3f001b(%rip)        # 4f8a77 <unmapped_end+0x3e4a77>
  108a5c:	1b 00                	sbb    (%rax),%eax
  108a5e:	36 00 1b             	ss add %bl,(%rbx)
  108a61:	00 2d 00 1b 00 24    	add    %ch,0x24001b00(%rip)        # 2410a567 <unmapped_end+0x23ff6567>
  108a67:	00 1b                	add    %bl,(%rbx)
  108a69:	00 1b                	add    %bl,(%rbx)
  108a6b:	00 1b                	add    %bl,(%rbx)
  108a6d:	00 12                	add    %dl,(%rdx)
  108a6f:	00 1b                	add    %bl,(%rbx)
  108a71:	00 09                	add    %cl,(%rcx)
  108a73:	00 1b                	add    %bl,(%rbx)
  108a75:	00 00                	add    %al,(%rax)
  108a77:	00 1b                	add    %bl,(%rbx)
  108a79:	00 3f                	add    %bh,(%rdi)
  108a7b:	3f                   	(bad)
  108a7c:	12 00                	adc    (%rax),%al
  108a7e:	36 3f                	ss (bad)
  108a80:	12 00                	adc    (%rax),%al
  108a82:	2d 3f 12 00 24       	sub    $0x2400123f,%eax
  108a87:	3f                   	(bad)
  108a88:	12 00                	adc    (%rax),%al
  108a8a:	1b 3f                	sbb    (%rdi),%edi
  108a8c:	12 00                	adc    (%rax),%al
  108a8e:	12 3f                	adc    (%rdi),%bh
  108a90:	12 00                	adc    (%rax),%al
  108a92:	09 3f                	or     %edi,(%rdi)
  108a94:	12 00                	adc    (%rax),%al
  108a96:	00 3f                	add    %bh,(%rdi)
  108a98:	12 00                	adc    (%rax),%al
  108a9a:	3f                   	(bad)
  108a9b:	2a 12                	sub    (%rdx),%dl
  108a9d:	00 36                	add    %dh,(%rsi)
  108a9f:	2a 12                	sub    (%rdx),%dl
  108aa1:	00 2d 2a 12 00 24    	add    %ch,0x2400122a(%rip)        # 24109cd1 <unmapped_end+0x23ff5cd1>
  108aa7:	2a 12                	sub    (%rdx),%dl
  108aa9:	00 1b                	add    %bl,(%rbx)
  108aab:	2a 12                	sub    (%rdx),%dl
  108aad:	00 12                	add    %dl,(%rdx)
  108aaf:	2a 12                	sub    (%rdx),%dl
  108ab1:	00 09                	add    %cl,(%rcx)
  108ab3:	2a 12                	sub    (%rdx),%dl
  108ab5:	00 00                	add    %al,(%rax)
  108ab7:	2a 12                	sub    (%rdx),%dl
  108ab9:	00 3f                	add    %bh,(%rdi)
  108abb:	15 12 00 36 15       	adc    $0x15360012,%eax
  108ac0:	12 00                	adc    (%rax),%al
  108ac2:	2d 15 12 00 24       	sub    $0x24001215,%eax
  108ac7:	15 12 00 1b 15       	adc    $0x151b0012,%eax
  108acc:	12 00                	adc    (%rax),%al
  108ace:	12 15 12 00 09 15    	adc    0x15090012(%rip),%dl        # 15198ae6 <unmapped_end+0x15084ae6>
  108ad4:	12 00                	adc    (%rax),%al
  108ad6:	00 15 12 00 3f 00    	add    %dl,0x3f0012(%rip)        # 4f8aee <unmapped_end+0x3e4aee>
  108adc:	12 00                	adc    (%rax),%al
  108ade:	36 00 12             	ss add %dl,(%rdx)
  108ae1:	00 2d 00 12 00 24    	add    %ch,0x24001200(%rip)        # 24109ce7 <unmapped_end+0x23ff5ce7>
  108ae7:	00 12                	add    %dl,(%rdx)
  108ae9:	00 1b                	add    %bl,(%rbx)
  108aeb:	00 12                	add    %dl,(%rdx)
  108aed:	00 12                	add    %dl,(%rdx)
  108aef:	00 12                	add    %dl,(%rdx)
  108af1:	00 09                	add    %cl,(%rcx)
  108af3:	00 12                	add    %dl,(%rdx)
  108af5:	00 00                	add    %al,(%rax)
  108af7:	00 12                	add    %dl,(%rdx)
  108af9:	00 3f                	add    %bh,(%rdi)
  108afb:	3f                   	(bad)
  108afc:	09 00                	or     %eax,(%rax)
  108afe:	36 3f                	ss (bad)
  108b00:	09 00                	or     %eax,(%rax)
  108b02:	2d 3f 09 00 24       	sub    $0x2400093f,%eax
  108b07:	3f                   	(bad)
  108b08:	09 00                	or     %eax,(%rax)
  108b0a:	1b 3f                	sbb    (%rdi),%edi
  108b0c:	09 00                	or     %eax,(%rax)
  108b0e:	12 3f                	adc    (%rdi),%bh
  108b10:	09 00                	or     %eax,(%rax)
  108b12:	09 3f                	or     %edi,(%rdi)
  108b14:	09 00                	or     %eax,(%rax)
  108b16:	00 3f                	add    %bh,(%rdi)
  108b18:	09 00                	or     %eax,(%rax)
  108b1a:	3f                   	(bad)
  108b1b:	2a 09                	sub    (%rcx),%cl
  108b1d:	00 36                	add    %dh,(%rsi)
  108b1f:	2a 09                	sub    (%rcx),%cl
  108b21:	00 2d 2a 09 00 24    	add    %ch,0x2400092a(%rip)        # 24109451 <unmapped_end+0x23ff5451>
  108b27:	2a 09                	sub    (%rcx),%cl
  108b29:	00 1b                	add    %bl,(%rbx)
  108b2b:	2a 09                	sub    (%rcx),%cl
  108b2d:	00 12                	add    %dl,(%rdx)
  108b2f:	2a 09                	sub    (%rcx),%cl
  108b31:	00 09                	add    %cl,(%rcx)
  108b33:	2a 09                	sub    (%rcx),%cl
  108b35:	00 00                	add    %al,(%rax)
  108b37:	2a 09                	sub    (%rcx),%cl
  108b39:	00 3f                	add    %bh,(%rdi)
  108b3b:	15 09 00 36 15       	adc    $0x15360009,%eax
  108b40:	09 00                	or     %eax,(%rax)
  108b42:	2d 15 09 00 24       	sub    $0x24000915,%eax
  108b47:	15 09 00 1b 15       	adc    $0x151b0009,%eax
  108b4c:	09 00                	or     %eax,(%rax)
  108b4e:	12 15 09 00 09 15    	adc    0x15090009(%rip),%dl        # 15198b5d <unmapped_end+0x15084b5d>
  108b54:	09 00                	or     %eax,(%rax)
  108b56:	00 15 09 00 3f 00    	add    %dl,0x3f0009(%rip)        # 4f8b65 <unmapped_end+0x3e4b65>
  108b5c:	09 00                	or     %eax,(%rax)
  108b5e:	36 00 09             	ss add %cl,(%rcx)
  108b61:	00 2d 00 09 00 24    	add    %ch,0x24000900(%rip)        # 24109467 <unmapped_end+0x23ff5467>
  108b67:	00 09                	add    %cl,(%rcx)
  108b69:	00 1b                	add    %bl,(%rbx)
  108b6b:	00 09                	add    %cl,(%rcx)
  108b6d:	00 12                	add    %dl,(%rdx)
  108b6f:	00 09                	add    %cl,(%rcx)
  108b71:	00 09                	add    %cl,(%rcx)
  108b73:	00 09                	add    %cl,(%rcx)
  108b75:	00 00                	add    %al,(%rax)
  108b77:	00 09                	add    %cl,(%rcx)
  108b79:	00 3f                	add    %bh,(%rdi)
  108b7b:	3f                   	(bad)
  108b7c:	00 00                	add    %al,(%rax)
  108b7e:	36 3f                	ss (bad)
  108b80:	00 00                	add    %al,(%rax)
  108b82:	2d 3f 00 00 24       	sub    $0x2400003f,%eax
  108b87:	3f                   	(bad)
  108b88:	00 00                	add    %al,(%rax)
  108b8a:	1b 3f                	sbb    (%rdi),%edi
  108b8c:	00 00                	add    %al,(%rax)
  108b8e:	12 3f                	adc    (%rdi),%bh
  108b90:	00 00                	add    %al,(%rax)
  108b92:	09 3f                	or     %edi,(%rdi)
  108b94:	00 00                	add    %al,(%rax)
  108b96:	00 3f                	add    %bh,(%rdi)
  108b98:	00 00                	add    %al,(%rax)
  108b9a:	3f                   	(bad)
  108b9b:	2a 00                	sub    (%rax),%al
  108b9d:	00 36                	add    %dh,(%rsi)
  108b9f:	2a 00                	sub    (%rax),%al
  108ba1:	00 2d 2a 00 00 24    	add    %ch,0x2400002a(%rip)        # 24108bd1 <unmapped_end+0x23ff4bd1>
  108ba7:	2a 00                	sub    (%rax),%al
  108ba9:	00 1b                	add    %bl,(%rbx)
  108bab:	2a 00                	sub    (%rax),%al
  108bad:	00 12                	add    %dl,(%rdx)
  108baf:	2a 00                	sub    (%rax),%al
  108bb1:	00 09                	add    %cl,(%rcx)
  108bb3:	2a 00                	sub    (%rax),%al
  108bb5:	00 00                	add    %al,(%rax)
  108bb7:	2a 00                	sub    (%rax),%al
  108bb9:	00 3f                	add    %bh,(%rdi)
  108bbb:	15 00 00 36 15       	adc    $0x15360000,%eax
  108bc0:	00 00                	add    %al,(%rax)
  108bc2:	2d 15 00 00 24       	sub    $0x24000015,%eax
  108bc7:	15 00 00 1b 15       	adc    $0x151b0000,%eax
  108bcc:	00 00                	add    %al,(%rax)
  108bce:	12 15 00 00 09 15    	adc    0x15090000(%rip),%dl        # 15198bd4 <unmapped_end+0x15084bd4>
  108bd4:	00 00                	add    %al,(%rax)
  108bd6:	00 15 00 00 3f 00    	add    %dl,0x3f0000(%rip)        # 4f8bdc <unmapped_end+0x3e4bdc>
  108bdc:	00 00                	add    %al,(%rax)
  108bde:	36 00 00             	ss add %al,(%rax)
  108be1:	00 2d 00 00 00 24    	add    %ch,0x24000000(%rip)        # 24108be7 <unmapped_end+0x23ff4be7>
  108be7:	00 00                	add    %al,(%rax)
  108be9:	00 1b                	add    %bl,(%rbx)
  108beb:	00 00                	add    %al,(%rax)
  108bed:	00 12                	add    %dl,(%rdx)
  108bef:	00 00                	add    %al,(%rax)
  108bf1:	00 09                	add    %cl,(%rcx)
  108bf3:	00 00                	add    %al,(%rax)
  108bf5:	00 00                	add    %al,(%rax)
  108bf7:	00 00                	add    %al,(%rax)
	...

0000000000108bfa <default_width>:
	...

0000000000108bfc <default_height>:
	...

0000000000108bfe <default_bpp>:
	...

0000000000108bff <default_mode>:
  108bff:	31 30                	xor    %esi,(%rax)
  108c01:	32 34 78             	xor    (%rax,%rdi,2),%dh
  108c04:	37                   	(bad)
  108c05:	36 38 2d 31 36 00 00 	ss cmp %ch,0x3631(%rip)        # 10c23d <ptl_2_2g+0x23d>
	...

0000000000108c23 <vesa_init_protected>:
  108c23:	fa                   	cli
  108c24:	fc                   	cld
  108c25:	bc 00 7c 10 00       	mov    $0x107c00,%esp
  108c2a:	66 b9 10 00          	mov    $0x10,%cx
  108c2e:	8e c1                	mov    %ecx,%es
  108c30:	8e d9                	mov    %ecx,%ds
  108c32:	8e d1                	mov    %ecx,%ss
  108c34:	66 b9 18 00          	mov    $0x18,%cx
  108c38:	8e e1                	mov    %ecx,%fs
  108c3a:	8e e9                	mov    %ecx,%gs
  108c3c:	ea                   	(bad)
  108c3d:	a7                   	cmpsl  %es:(%rdi),%ds:(%rsi)
  108c3e:	83 10 00             	adcl   $0x0,(%rax)
  108c41:	28 00                	sub    %al,(%rax)
  108c43:	90                   	nop

0000000000108c44 <e_vesa_init>:
  108c44:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c4b:	00 00 00 
  108c4e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c55:	00 00 00 
  108c58:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c5f:	00 00 00 
  108c62:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c69:	00 00 00 
  108c6c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c73:	00 00 00 
  108c76:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c7d:	00 00 00 
  108c80:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c87:	00 00 00 
  108c8a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c91:	00 00 00 
  108c94:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108c9b:	00 00 00 
  108c9e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ca5:	00 00 00 
  108ca8:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108caf:	00 00 00 
  108cb2:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108cb9:	00 00 00 
  108cbc:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108cc3:	00 00 00 
  108cc6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ccd:	00 00 00 
  108cd0:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108cd7:	00 00 00 
  108cda:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ce1:	00 00 00 
  108ce4:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ceb:	00 00 00 
  108cee:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108cf5:	00 00 00 
  108cf8:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108cff:	00 00 00 
  108d02:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d09:	00 00 00 
  108d0c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d13:	00 00 00 
  108d16:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d1d:	00 00 00 
  108d20:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d27:	00 00 00 
  108d2a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d31:	00 00 00 
  108d34:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d3b:	00 00 00 
  108d3e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d45:	00 00 00 
  108d48:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d4f:	00 00 00 
  108d52:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d59:	00 00 00 
  108d5c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d63:	00 00 00 
  108d66:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d6d:	00 00 00 
  108d70:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d77:	00 00 00 
  108d7a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d81:	00 00 00 
  108d84:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d8b:	00 00 00 
  108d8e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d95:	00 00 00 
  108d98:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108d9f:	00 00 00 
  108da2:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108da9:	00 00 00 
  108dac:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108db3:	00 00 00 
  108db6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108dbd:	00 00 00 
  108dc0:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108dc7:	00 00 00 
  108dca:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108dd1:	00 00 00 
  108dd4:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ddb:	00 00 00 
  108dde:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108de5:	00 00 00 
  108de8:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108def:	00 00 00 
  108df2:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108df9:	00 00 00 
  108dfc:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e03:	00 00 00 
  108e06:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e0d:	00 00 00 
  108e10:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e17:	00 00 00 
  108e1a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e21:	00 00 00 
  108e24:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e2b:	00 00 00 
  108e2e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e35:	00 00 00 
  108e38:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e3f:	00 00 00 
  108e42:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e49:	00 00 00 
  108e4c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e53:	00 00 00 
  108e56:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e5d:	00 00 00 
  108e60:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e67:	00 00 00 
  108e6a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e71:	00 00 00 
  108e74:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e7b:	00 00 00 
  108e7e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e85:	00 00 00 
  108e88:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e8f:	00 00 00 
  108e92:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108e99:	00 00 00 
  108e9c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ea3:	00 00 00 
  108ea6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ead:	00 00 00 
  108eb0:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108eb7:	00 00 00 
  108eba:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ec1:	00 00 00 
  108ec4:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ecb:	00 00 00 
  108ece:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ed5:	00 00 00 
  108ed8:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108edf:	00 00 00 
  108ee2:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ee9:	00 00 00 
  108eec:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ef3:	00 00 00 
  108ef6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108efd:	00 00 00 
  108f00:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f07:	00 00 00 
  108f0a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f11:	00 00 00 
  108f14:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f1b:	00 00 00 
  108f1e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f25:	00 00 00 
  108f28:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f2f:	00 00 00 
  108f32:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f39:	00 00 00 
  108f3c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f43:	00 00 00 
  108f46:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f4d:	00 00 00 
  108f50:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f57:	00 00 00 
  108f5a:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f61:	00 00 00 
  108f64:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f6b:	00 00 00 
  108f6e:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f75:	00 00 00 
  108f78:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f7f:	00 00 00 
  108f82:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f89:	00 00 00 
  108f8c:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f93:	00 00 00 
  108f96:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108f9d:	00 00 00 
  108fa0:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fa7:	00 00 00 
  108faa:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fb1:	00 00 00 
  108fb4:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fbb:	00 00 00 
  108fbe:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fc5:	00 00 00 
  108fc8:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fcf:	00 00 00 
  108fd2:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fd9:	00 00 00 
  108fdc:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fe3:	00 00 00 
  108fe6:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108fed:	00 00 00 
  108ff0:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  108ff7:	00 00 00 
  108ffa:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)

0000000000109000 <unmapped_ap_boot>:
  109000:	fa                   	cli
  109001:	31 c0                	xor    %eax,%eax
  109003:	8e d8                	mov    %eax,%ds
  109005:	66 0f 01 16          	data16 lgdt (%rsi)
  109009:	13 92 0f 20 c0 66    	adc    0x66c0200f(%rdx),%edx
  10900f:	83 c8 01             	or     $0x1,%eax
  109012:	0f 22 c0             	mov    %rax,%cr0
  109015:	66 ea                	data16 (bad)
  109017:	1d 90 00 00 28       	sbb    $0x28000090,%eax
	...

000000000010901d <jump_to_kernel>:
  10901d:	66 b8 10 00          	mov    $0x10,%ax
  109021:	8e d8                	mov    %eax,%ds
  109023:	8e c0                	mov    %eax,%es
  109025:	8e d0                	mov    %eax,%ss
  109027:	66 b8 18 00          	mov    $0x18,%ax
  10902b:	8e e8                	mov    %eax,%gs
  10902d:	0f 20 e0             	mov    %cr4,%rax
  109030:	83 c8 20             	or     $0x20,%eax
  109033:	0f 22 e0             	mov    %rax,%cr4
  109036:	8d 05 00 30 11 00    	lea    0x113000(%rip),%eax        # 21c03c <unmapped_end+0x10803c>
  10903c:	0f 22 d8             	mov    %rax,%cr3
  10903f:	b9 80 00 00 c0       	mov    $0xc0000080,%ecx
  109044:	0f 32                	rdmsr
  109046:	0d 00 01 00 00       	or     $0x100,%eax
  10904b:	0f 30                	wrmsr
  10904d:	0f 20 c0             	mov    %cr0,%rax
  109050:	0d 00 00 00 80       	or     $0x80000000,%eax
  109055:	0f 22 c0             	mov    %rax,%cr0
  109058:	ea                   	(bad)
  109059:	5f                   	pop    %rdi
  10905a:	90                   	nop
  10905b:	00 00                	add    %al,(%rax)
  10905d:	08 00                	or     %al,(%rax)

000000000010905f <start64>:
  10905f:	48 bc 40 a6 16 80 ff 	movabs $0xffffffff8016a640,%rsp
  109066:	ff ff ff 
  109069:	48 8b 24 24          	mov    (%rsp),%rsp
  10906d:	6a 00                	push   $0x0
  10906f:	6a 00                	push   $0x0
  109071:	48 89 e5             	mov    %rsp,%rbp
  109074:	48 b8 e0 5c 11 80 ff 	movabs $0xffffffff80115ce0,%rax
  10907b:	ff ff ff 
  10907e:	ff d0                	call   *%rax

0000000000109080 <bootstrap_idtr>:
  109080:	00 00                	add    %al,(%rax)
  109082:	00 00                	add    %al,(%rax)
	...

0000000000109086 <bootstrap_gdtr>:
  109086:	50                   	push   %rax
  109087:	00 00                	add    %al,(%rax)
  109089:	fe                   	(bad)
  10908a:	16                   	(bad)
	...

000000000010908c <multiboot_eax>:
  10908c:	00 00                	add    %al,(%rax)
	...

0000000000109090 <multiboot_ebx>:
  109090:	00 00                	add    %al,(%rax)
	...

0000000000109094 <err_extended_cpuid>:
  109094:	45 72 72             	rex.RB jb 109109 <err_long_mode+0x2c>
  109097:	6f                   	outsl  %ds:(%rsi),(%dx)
  109098:	72 3a                	jb     1090d4 <err_extended_cpuid+0x40>
  10909a:	20 45 78             	and    %al,0x78(%rbp)
  10909d:	74 65                	je     109104 <err_long_mode+0x27>
  10909f:	6e                   	outsb  %ds:(%rsi),(%dx)
  1090a0:	64 65 64 20 43 50    	fs gs and %al,%fs:0x50(%rbx)
  1090a6:	55                   	push   %rbp
  1090a7:	49                   	rex.WB
  1090a8:	44 20 6e 6f          	and    %r13b,0x6f(%rsi)
  1090ac:	74 20                	je     1090ce <err_extended_cpuid+0x3a>
  1090ae:	73 75                	jae    109125 <err_noexecute+0x12>
  1090b0:	70 70                	jo     109122 <err_noexecute+0xf>
  1090b2:	6f                   	outsl  %ds:(%rsi),(%dx)
  1090b3:	72 74                	jb     109129 <err_noexecute+0x16>
  1090b5:	65 64 20 2d 2d 20 43 	gs and %ch,%fs:0x5043202d(%rip)        # 5053b0ea <unmapped_end+0x504270ea>
  1090bc:	50 
  1090bd:	55                   	push   %rbp
  1090be:	20 69 73             	and    %ch,0x73(%rcx)
  1090c1:	20 6e 6f             	and    %ch,0x6f(%rsi)
  1090c4:	74 20                	je     1090e6 <err_long_mode+0x9>
  1090c6:	36 34 2d             	ss xor $0x2d,%al
  1090c9:	62                   	(bad)
  1090ca:	69 74 2e 20 53 79 73 	imul   $0x74737953,0x20(%rsi,%rbp,1),%esi
  1090d1:	74 
  1090d2:	65 6d                	gs insl (%dx),%es:(%rdi)
  1090d4:	20 68 61             	and    %ch,0x61(%rax)
  1090d7:	6c                   	insb   (%dx),%es:(%rdi)
  1090d8:	74 65                	je     10913f <err_noexecute+0x2c>
  1090da:	64                   	fs
  1090db:	2e                   	cs
	...

00000000001090dd <err_long_mode>:
  1090dd:	45 72 72             	rex.RB jb 109152 <err_fx+0x9>
  1090e0:	6f                   	outsl  %ds:(%rsi),(%dx)
  1090e1:	72 3a                	jb     10911d <err_noexecute+0xa>
  1090e3:	20 36                	and    %dh,(%rsi)
  1090e5:	34 2d                	xor    $0x2d,%al
  1090e7:	62                   	(bad)
  1090e8:	69 74 20 6c 6f 6e 67 	imul   $0x20676e6f,0x6c(%rax,%riz,1),%esi
  1090ef:	20 
  1090f0:	6d                   	insl   (%dx),%es:(%rdi)
  1090f1:	6f                   	outsl  %ds:(%rsi),(%dx)
  1090f2:	64 65 20 6e 6f       	fs and %ch,%gs:0x6f(%rsi)
  1090f7:	74 20                	je     109119 <err_noexecute+0x6>
  1090f9:	73 75                	jae    109170 <err_fx+0x27>
  1090fb:	70 70                	jo     10916d <err_fx+0x24>
  1090fd:	6f                   	outsl  %ds:(%rsi),(%dx)
  1090fe:	72 74                	jb     109174 <err_fx+0x2b>
  109100:	65 64 2e 20 53 79    	gs fs and %dl,%fs:0x79(%rbx)
  109106:	73 74                	jae    10917c <err_fx+0x33>
  109108:	65 6d                	gs insl (%dx),%es:(%rdi)
  10910a:	20 68 61             	and    %ch,0x61(%rax)
  10910d:	6c                   	insb   (%dx),%es:(%rdi)
  10910e:	74 65                	je     109175 <err_fx+0x2c>
  109110:	64                   	fs
  109111:	2e                   	cs
	...

0000000000109113 <err_noexecute>:
  109113:	45 72 72             	rex.RB jb 109188 <err_fx+0x3f>
  109116:	6f                   	outsl  %ds:(%rsi),(%dx)
  109117:	72 3a                	jb     109153 <err_fx+0xa>
  109119:	20 4e 6f             	and    %cl,0x6f(%rsi)
  10911c:	2d 65 78 65 63       	sub    $0x63657865,%eax
  109121:	75 74                	jne    109197 <err_sse2+0xb>
  109123:	65 20 70 61          	and    %dh,%gs:0x61(%rax)
  109127:	67 65 73 20          	addr32 gs jae 10914b <err_fx+0x2>
  10912b:	6e                   	outsb  %ds:(%rsi),(%dx)
  10912c:	6f                   	outsl  %ds:(%rsi),(%dx)
  10912d:	74 20                	je     10914f <err_fx+0x6>
  10912f:	73 75                	jae    1091a6 <err_sse2+0x1a>
  109131:	70 70                	jo     1091a3 <err_sse2+0x17>
  109133:	6f                   	outsl  %ds:(%rsi),(%dx)
  109134:	72 74                	jb     1091aa <err_sse2+0x1e>
  109136:	65 64 2e 20 53 79    	gs fs and %dl,%fs:0x79(%rbx)
  10913c:	73 74                	jae    1091b2 <err_sse2+0x26>
  10913e:	65 6d                	gs insl (%dx),%es:(%rdi)
  109140:	20 68 61             	and    %ch,0x61(%rax)
  109143:	6c                   	insb   (%dx),%es:(%rdi)
  109144:	74 65                	je     1091ab <err_sse2+0x1f>
  109146:	64                   	fs
  109147:	2e                   	cs
	...

0000000000109149 <err_fx>:
  109149:	45 72 72             	rex.RB jb 1091be <err_sse2+0x32>
  10914c:	6f                   	outsl  %ds:(%rsi),(%dx)
  10914d:	72 3a                	jb     109189 <err_fx+0x40>
  10914f:	20 46 58             	and    %al,0x58(%rsi)
  109152:	53                   	push   %rbx
  109153:	41 56                	push   %r14
  109155:	45 2f                	rex.RB (bad)
  109157:	46 58                	rex.RX pop %rax
  109159:	52                   	push   %rdx
  10915a:	45 53                	rex.RB push %r11
  10915c:	54                   	push   %rsp
  10915d:	4f 52                	rex.WRXB push %r10
  10915f:	45 20 69 6e          	and    %r13b,0x6e(%r9)
  109163:	73 74                	jae    1091d9 <status_multiboot_cmdline+0x1>
  109165:	72 75                	jb     1091dc <status_multiboot_cmdline+0x4>
  109167:	63 74 69 6f          	movsxd 0x6f(%rcx,%rbp,2),%esi
  10916b:	6e                   	outsb  %ds:(%rsi),(%dx)
  10916c:	73 20                	jae    10918e <err_sse2+0x2>
  10916e:	6e                   	outsb  %ds:(%rsi),(%dx)
  10916f:	6f                   	outsl  %ds:(%rsi),(%dx)
  109170:	74 20                	je     109192 <err_sse2+0x6>
  109172:	73 75                	jae    1091e9 <status_multiboot_cmdline+0x11>
  109174:	70 70                	jo     1091e6 <status_multiboot_cmdline+0xe>
  109176:	6f                   	outsl  %ds:(%rsi),(%dx)
  109177:	72 74                	jb     1091ed <status_vesa_real>
  109179:	65 64 2e 20 53 79    	gs fs and %dl,%fs:0x79(%rbx)
  10917f:	73 74                	jae    1091f5 <status_vesa_real+0x8>
  109181:	65 6d                	gs insl (%dx),%es:(%rdi)
  109183:	20 68 61             	and    %ch,0x61(%rax)
  109186:	6c                   	insb   (%dx),%es:(%rdi)
  109187:	74 65                	je     1091ee <status_vesa_real+0x1>
  109189:	64                   	fs
  10918a:	2e                   	cs
	...

000000000010918c <err_sse2>:
  10918c:	45 72 72             	rex.RB jb 109201 <status_prot2+0x7>
  10918f:	6f                   	outsl  %ds:(%rsi),(%dx)
  109190:	72 3a                	jb     1091cc <status_vesa_copy+0x1>
  109192:	20 53 53             	and    %dl,0x53(%rbx)
  109195:	45 32 20             	xor    (%r8),%r12b
  109198:	69 6e 73 74 72 75 63 	imul   $0x63757274,0x73(%rsi),%ebp
  10919f:	74 69                	je     10920a <status_long+0x7>
  1091a1:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091a2:	6e                   	outsb  %ds:(%rsi),(%dx)
  1091a3:	73 20                	jae    1091c5 <status_prot+0x2>
  1091a5:	6e                   	outsb  %ds:(%rsi),(%dx)
  1091a6:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091a7:	74 20                	je     1091c9 <status_prot+0x6>
  1091a9:	73 75                	jae    109220 <unmapped_ap_gdtr+0xd>
  1091ab:	70 70                	jo     10921d <unmapped_ap_gdtr+0xa>
  1091ad:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091ae:	72 74                	jb     109224 <unmapped_ap_gdtr+0x11>
  1091b0:	65 64 2e 20 53 79    	gs fs and %dl,%fs:0x79(%rbx)
  1091b6:	73 74                	jae    10922c <unmapped_ap_gdtr+0x19>
  1091b8:	65 6d                	gs insl (%dx),%es:(%rdi)
  1091ba:	20 68 61             	and    %ch,0x61(%rax)
  1091bd:	6c                   	insb   (%dx),%es:(%rdi)
  1091be:	74 65                	je     109225 <unmapped_ap_gdtr+0x12>
  1091c0:	64                   	fs
  1091c1:	2e                   	cs
	...

00000000001091c3 <status_prot>:
  1091c3:	5b                   	pop    %rbx
  1091c4:	70 72                	jo     109238 <unmapped_ap_gdtr+0x25>
  1091c6:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091c7:	74 5d                	je     109226 <unmapped_ap_gdtr+0x13>
  1091c9:	20 00                	and    %al,(%rax)

00000000001091cb <status_vesa_copy>:
  1091cb:	5b                   	pop    %rbx
  1091cc:	76 65                	jbe    109233 <unmapped_ap_gdtr+0x20>
  1091ce:	73 61                	jae    109231 <unmapped_ap_gdtr+0x1e>
  1091d0:	5f                   	pop    %rdi
  1091d1:	63 6f 70             	movsxd 0x70(%rdi),%ebp
  1091d4:	79 5d                	jns    109233 <unmapped_ap_gdtr+0x20>
  1091d6:	20 00                	and    %al,(%rax)

00000000001091d8 <status_multiboot_cmdline>:
  1091d8:	5b                   	pop    %rbx
  1091d9:	6d                   	insl   (%dx),%es:(%rdi)
  1091da:	75 6c                	jne    109248 <unmapped_ap_gdtr+0x35>
  1091dc:	74 69                	je     109247 <unmapped_ap_gdtr+0x34>
  1091de:	62                   	(bad)
  1091df:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091e0:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091e1:	74 5f                	je     109242 <unmapped_ap_gdtr+0x2f>
  1091e3:	63 6d 64             	movsxd 0x64(%rbp),%ebp
  1091e6:	6c                   	insb   (%dx),%es:(%rdi)
  1091e7:	69                   	.byte 0x69
  1091e8:	6e                   	outsb  %ds:(%rsi),(%dx)
  1091e9:	65 5d                	gs pop %rbp
  1091eb:	20 00                	and    %al,(%rax)

00000000001091ed <status_vesa_real>:
  1091ed:	5b                   	pop    %rbx
  1091ee:	76 65                	jbe    109255 <unmapped_ap_gdtr+0x42>
  1091f0:	73 61                	jae    109253 <unmapped_ap_gdtr+0x40>
  1091f2:	5f                   	pop    %rdi
  1091f3:	72 65                	jb     10925a <unmapped_ap_gdtr+0x47>
  1091f5:	61                   	(bad)
  1091f6:	6c                   	insb   (%dx),%es:(%rdi)
  1091f7:	5d                   	pop    %rbp
  1091f8:	20 00                	and    %al,(%rax)

00000000001091fa <status_prot2>:
  1091fa:	5b                   	pop    %rbx
  1091fb:	70 72                	jo     10926f <unmapped_ap_gdtr+0x5c>
  1091fd:	6f                   	outsl  %ds:(%rsi),(%dx)
  1091fe:	74 32                	je     109232 <unmapped_ap_gdtr+0x1f>
  109200:	5d                   	pop    %rbp
  109201:	20 00                	and    %al,(%rax)

0000000000109203 <status_long>:
  109203:	5b                   	pop    %rbx
  109204:	6c                   	insb   (%dx),%es:(%rdi)
  109205:	6f                   	outsl  %ds:(%rsi),(%dx)
  109206:	6e                   	outsb  %ds:(%rsi),(%dx)
  109207:	67 5d                	addr32 pop %rbp
  109209:	20 00                	and    %al,(%rax)

000000000010920b <status_main>:
  10920b:	5b                   	pop    %rbx
  10920c:	6d                   	insl   (%dx),%es:(%rdi)
  10920d:	61                   	(bad)
  10920e:	69                   	.byte 0x69
  10920f:	6e                   	outsb  %ds:(%rsi),(%dx)
  109210:	5d                   	pop    %rbp
  109211:	20 00                	and    %al,(%rax)

```