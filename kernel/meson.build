#
# Copyright (c) 2005 Martin Decky
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# - The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# 定义变量 arch_src ， 保存体系结构相关的源码。
arch_src = []

# Fills arch_src. 填充 arch_src 的内容
subdir('arch' / KARCH)
##############################################################################
#							变量表
#	arch_src								files(
										# 	'src/amd64.c',
										# 	'src/asm.S',
										# 	'src/bios/bios.c',
										# 	'src/boot/memmap.c',
										# 	'src/boot/multiboot2.S',
										# 	'src/boot/multiboot.S',
										# 	'src/context.S',
										# 	'src/cpu/cpu.c',
										# 	'src/ddi/ddi.c',
										# 	'src/debug/stacktrace_asm.S',
										# 	'src/debug/stacktrace.c',
										# 	'src/delay.S',
										# 	'src/drivers/i8254.c',
										# 	'src/fpu_context.c',
										# 	'src/interrupt.c',
										# 	'src/kseg.c',
										# 	'src/mm/as.c',
										# 	'src/mm/frame.c',
										# 	'src/mm/km.c',
										# 	'src/mm/page.c',
										# 	'src/mm/tlb.c',
										# 	'src/pm.c',
										# 	'src/proc/scheduler.c',
										# 	'src/proc/task.c',
										# 	'src/proc/thread.c',
										# 	'src/smc.c',
										# 	'src/syscall.c',
										# 	'src/userspace.c',
										# 	'src/vreg.c',
										# )
										# if CONFIG_SMP
										# 	arch_src += files(
										# 		'src/smp/apic.c',
										# 		'src/smp/ap.S',
										# 		'src/smp/ipi.c',
										# 		'src/smp/mps.c',
										# 		'src/smp/smp.c',
										# 	)
										# 'include/arch/boot/memmap_struct.check.c',
										# 'include/arch/context_struct.check.c',
										# 'include/arch/istate_struct.check.c',
										# 'include/arch/kseg_struct.check.c',
##############################################################################
# Defines genarch_src. 填充genarch_src的内容。
subdir('genarch')
#################################################################################
#							变量表
#	genarch_src
#
#################################################################################
# Defines generic_src, instrumentable_src.
subdir('generic')
#################################################################################
#							变量表
#	generic_src
#	instrumentable_src
#################################################################################
# Defines test_src
subdir('test')
#################################################################################
#							变量表
#	test_src
#
#################################################################################

# 定义内核编译头文件目录。
kernel_include_dirs = include_directories(
	'generic/include',
	'genarch/include',
	'arch' / KARCH / 'include',
	'..' / 'abi' / 'arch' / KARCH / 'include',
	'..' / 'abi' / 'include',
	'..' / 'common' / 'include',
	'test',
)

# 一系列编译时的宏定义，用于控制内核编译的行为和特性。
kernel_defs = [
	'-imacros', meson.build_root() / 'config.h',
	'-D_HELENOS_SOURCE',							# -D_HELENOS_SOURCE 表示这是一个 HelenOS 内核的源代码
	'-DKERNEL',										# -DKERNEL 表示正在编译的是内核代码
	'-DHELENOS_RELEASE=' + HELENOS_RELEASE,
	'-DHELENOS_COPYRIGHT=' + HELENOS_COPYRIGHT,
	'-DHELENOS_CODENAME=' + HELENOS_CODENAME,
	'-D__@0@_BITS__'.format(meson.get_cross_property('bits')),
]

# Preprocess linker script using C preprocessor.
# 包含了链接时需要的参数，如 -Wl,--nmagic 表示生成的 ELF 文件不包含符号表和重定位表等信息，以减小文件大小。
# 通过 C 预处理器预处理链接脚本文件 _link.ld.in。预处理后的文件将被命名为 _link.ld，并用于后续的链接过程。
kernel_ldscript = custom_target('_link.ld',
	input: 'arch' / KARCH / '_link.ld.in',
	output: '_link.ld',
	command: [
		cc.cmd_array(),
		arch_kernel_c_args,
		kernel_defs,
		'-I' + meson.current_source_dir() / 'arch' / KARCH / 'include',
		'-D__ASSEMBLER__',
		'-D__LINKER__',
		'-E',
		'-P',
		'-x', 'c',
		'@INPUT@',
	],
	capture: true,
	build_by_default: true,
)

kernel_link_args = arch_kernel_link_args + [
	'-Wl,--nmagic',
	'-T', meson.current_build_dir() / '_link.ld',
]
# The kernel is built as ELF but then copied as a blob of bytes and
# the permissions are not relevant anyway (needed for binutils 2.39+).
kernel_link_args += ldflags_ignore_rwx_segments

# 如果配置了 LTO（Link Time Optimization），则会添加 -flto 参数以启用链接时优化。
if CONFIG_LTO
	kernel_link_args += [ '-flto' ]
endif

# 编译 C 代码时需要的参数，如 -ffreestanding 表示编译为独立的代码，不依赖于标准库。
kernel_c_args = arch_kernel_c_args + kernel_defs + [
	'-ffreestanding',
	'-fdebug-prefix-map=../../kernel/=',
	'-fdebug-prefix-map=../../../kernel/=',

	cc.get_supported_arguments([
		# TODO: remove this flag
		'-Wno-cast-function-type',

		# When accessing specific memory addresses that are below
		# normal page size, the compiler may assume that we actually
		# dereferenced NULL pointer and warns us about that.
		# But in kernel we often need to access these addresses
		# directly hence we need to ignore these warnings.
		#
		# TODO: might make more sense to disable this selectively
		# in specific files (or better yet, for specific lines).
		'--param=min-pagesize=0',
	]),
]

if not CONFIG_LINE_DEBUG
	# Keep the debug info needed to get file names for kernel stack traces.
	kernel_c_args += [ '-gdwarf-5', '-g1', '-gno-statement-frontiers' ]
endif

# 如果配置了 LTO，同样会添加 -flto 参数。
if CONFIG_LTO
	kernel_c_args += [ '-flto' ]
endif

# 如果使用的是 Clang 编译器，还会添加一些特定的参数，如 -fno-stack-protector 禁用栈保护等.
if cc.get_id() == 'clang'
	kernel_c_args += [
		'-fno-stack-protector',
		'-fno-PIC',
		'-mllvm', '-asm-macro-max-nesting-depth=1000',
	]
endif

# instrumentables 和 noninstrumentables 是两个静态库目标，分别用于构建需要和不需要进行函数调用跟踪的代码模块。

# instrumentables 静态库
#名称: instrumentables
#源文件: instrumentable_src
#包含目录: kernel_include_dirs
#隐式包含目录: false（不自动包含源文件所在目录）
#C 编译参数: kernel_c_args，如果 CONFIG_TRACE 为真，则添加 -finstrument-functions 参数。这个参数用于启用函数调用跟踪，通常用于调试和性能分析。
#位置无关代码: false（不生成位置无关代码，通常用于静态库）
instrumentables = static_library('instrumentables', instrumentable_src,
	include_directories: kernel_include_dirs,
	implicit_include_directories: false,
	c_args: kernel_c_args + (CONFIG_TRACE ? [ '-finstrument-functions' ] : []),
	pic: false,
)

# noninstrumentables 静态库
#名称: noninstrumentables
#源文件: arch_src, genarch_src, generic_src, test_src
#包含目录: kernel_include_dirs
#隐式包含目录: false
#C 编译参数: kernel_c_args
#位置无关代码: false
noninstrumentables = static_library('noninstrumentables', arch_src, genarch_src, generic_src, test_src,
	include_directories: kernel_include_dirs,
	implicit_include_directories: false,
	c_args: kernel_c_args,
	pic: false,
)

all_kernel_objects = [ instrumentables, noninstrumentables ]

kernel_name = 'kernel.elf'
kernel_map_name = kernel_name + '.map'
kernel_map_path = meson.current_build_dir() / kernel_map_name

# kernel_elf 是一个可执行文件目标，用于构建最终的内核 ELF 文件。它会将所有静态库链接在一起，并生成一个包含符号表的映射文件 kernel.map。
# kernel_elf 是最终的内核 ELF 文件，它将所有静态库链接在一起，并生成一个包含符号表的映射文件 kernel.map，用于调试和分析内核的符号和内存布局.
#******** 详细解析 ************
# 	编译内核的选项：
#		1. 	include文件目录： kernel_include_dirs
#		2.  编译c文件的参数 ： kernel_c_args
#		3.  链接参数		  ： kernel_c_args + kernel_linl_args + -Wl,-Map /home/zyc/code/build/amd64_dbg/kernel/kernel.elf.map
#		4.	链接依赖		  ： _link.ld
#		5. 	链接整个静态库  :  all_kernel_objects（包括 instrumentables 和 noninstrumentables）
#		6.  位置无关代码	  :  false（不生成位置无关代码）
##############################
kernel_elf = executable(kernel_name,
	include_directories: kernel_include_dirs,
	implicit_include_directories: false,
	c_args: kernel_c_args,
	link_args: kernel_c_args + kernel_link_args + [
		'-Wl,-Map,' + kernel_map_path,
	],
	link_depends: kernel_ldscript,
	link_whole: all_kernel_objects,
	pie: false,
)


# kernel_dbg 是一个自定义目标，用于从内核 ELF 文件中提取调试信息，生成 kernel.dbg 文件。
kernel_dbg = custom_target('kernel.dbg',
	output: 'kernel.dbg',
	input: kernel_elf,
	command: [
		objcopy,
		'--only-keep-debug',
		'@INPUT@',
		'@OUTPUT@',
	],
)

# kernel_elf_stripped 是一个自定义目标，用于从内核 ELF 文件中剥离不必要的调试信息，生成精简版的 kernel.elf.stripped 文件。
kernel_elf_stripped = custom_target(kernel_name + '.stripped',
	output: kernel_name + '.stripped',
	input: kernel_elf,
	command: [
		objcopy,
		'--strip-unneeded',
		'@INPUT@',
		'@OUTPUT@',
	],
)

# rd_init_binaries 和 install_files 用于将构建好的内核文件和相关文件添加到分发包中，并安装到指定目录。
rd_init_binaries += [[ kernel_elf_stripped, 'boot/kernel.elf' ]]
install_files += [[ 'boot', kernel_elf_stripped.full_path(), 'kernel.elf' ]]
# install_deps 用于指定安装时依赖的目标。
install_deps += [ kernel_elf_stripped ]

if CONFIG_SYMTAB
	rd_init_binaries += [[ kernel_dbg, 'kernel.dbg' ]]
	install_files += [[ 'boot', kernel_dbg.full_path(), 'kernel.dbg' ]]
	install_deps += [ kernel_dbg ]
endif

# kernel_disasm 是一个自定义目标，用于生成内核 ELF 文件的反汇编文件 kernel.elf.disasm，以便于调试和分析内核代码.
kernel_disasm = custom_target('kernel.elf.disasm',
	command: [ objdump, '-S', '@INPUT@' ],
	input: kernel_elf,
	output: 'kernel.elf.disasm',
	capture: true,
	build_by_default: true,
)

# TODO: Add configuration option for installing debug files
if false
	install_files += [[ 'boot', kernel_disasm.full_path(), 'kernel.elf.disasm' ]]
	install_deps += [ kernel_disasm ]
endif
##############################################################################
#							变量表
#	kernel_elf							executable(...);
#	kernel_elf_stripped					custom_target(... input: kernel_elf, ...);
#	kernel_dbg							custom_target(... input: kernel_elf, ...);
#	rd_init_binaries					[[ kernel_elf_stripped, 'boot/kernel.elf' ] , [ kernel_dbg, 'kernel.dbg' ], ]
#	install_files						[[ 'boot', kernel_elf_stripped.full_path(), 'kernel.elf' ] ,
#										 [ 'boot', kernel_dbg.full_path(), 'kernel.dbg' ]]
#	install_deps						[kernel_elf_stripped , kernel_dbg]
################################################################################