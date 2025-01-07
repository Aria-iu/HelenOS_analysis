#
# Copyright (c) 2019 Jiří Zárevúcky
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

# TODO: Use vcs_tag() to generate version string
# TODO: jobfile
# TODO: lto in uspace
# TODO: fix clang build

############################################################
#					-- 前置变量表 --
#	PLATFORM						configure.sh
#	MACHINE							configure.sh
#	cross_target					configure.sh
#	cross_def						configure.sh
#	cc_arch							configure.sh
# 	compname						configure.sh
#	crossfile						configure.sh
#############################################################

project(
    'HelenOS',
    [ 'c', 'cpp' ],
    default_options : ['buildtype=plain', 'c_std=gnu11', 'cpp_std=c++17', 'warning_level=2', 'werror=false', 'b_staticpic=false', 'prefix=/' ],
    meson_version: '>=0.55.0',
)

debug_options = false
debug_shell_scripts = false

# 指定项目需要C和C++语言，并指定语言标准版本。
# 设置编译器警告级别为 2，显示所以警告。
# 设置编译器警告为错误的选项为false，不将其视为错误。
# 设置是否生成静态位置无关代码为false。？？？
# 设置prefix为/根目录，通常指定安装目录。
# 设置debug_options为false。
# 设置debug_shell_scripts为false。

################################################################################################################
#						变量表
#	debug_options					false
# 	debug_shell_scripts				false
################################################################################################################
subdir('meson/part/tools')
#################################################################################################################
#						变量表
#	debug_options					false
# 	debug_shell_scripts				false
#	_tools_dir						D:\code\helenos/tools
#	basename						find_program('basename')
#	cc  							meson.get_compiler('c')
#	config_py  						find_program(_tools_dir / 'config.py')
#	cp 								find_program('cp')
#	dirname							find_program('dirname')
#	doxygen							find_program('doxygen', required: false)
#	find 							find_program('find')
#	grep 							find_program('grep')
#	mkarray 						find_program(_tools_dir / 'mkarray_for_meson.sh')
#	mkext4 							find_program(_tools_dir / 'mkext4.py')
#	mkfat 							find_program(_tools_dir / 'mkfat.py')
#	mkuimage 						find_program(_tools_dir / 'mkuimage.py')
#	objcopy 						find_program('objcopy')
#	objdump 						find_program('objdump')
#	patch 							find_program('patch')
#	sed 							find_program('sed')
#	unzip 							find_program('unzip')
#	which 							find_program('which')
#	cpc 							find_program(_tools_dir / 'cc.sh')
#	cppcheck 						find_program('cppcheck', required: false)
#	sh								[ find_program('sh'), '-u', '-e' ]
# 	fs  							import('fs')
#	genisoimage 					find_program('genisoimage', required: false)
#	_mkisofs  						find_program('mkisofs', required: false)
#  	autocheck 						generator(find_program(_tools_dir / 'autocheck.awk'),arguments: [ '@INPUT@' ],output: '@PLAINNAME@.check.c',capture: true,)
# 	gzip							[ find_program('gzip'), '--no-name', '-9', '--stdout', '@INPUT@' ]
#	tar = 							[	find_program('tar'),
									#	'-c',
									#	'-f', '@OUTPUT@',
									#	'--mtime=1970-01-01 00:00:00Z',
									#	'--group=0',
									#	'--owner=0',
									#	'--numeric-owner',
									#	'--mode=go=rX,u+rw,a-s',
									#	'--no-acls',
									#	'--no-selinux',
									#	'--no-xattrs',
									#	'--format=ustar',
									#	'--transform', 's:@OUTDIR@/::',
									#	'@INPUT@',]
#
#####################################################################################################################
subdir('meson/part/read_config')
#################################################################################################################
#						变量表
#	debug_options					false
# 	debug_shell_scripts				false
#	_tools_dir						D:\code\helenos/tools
#	basename						find_program('basename')
#	cc  							meson.get_compiler('c')
#	config_py  						find_program(_tools_dir / 'config.py')
#	cp 								find_program('cp')
#	dirname							find_program('dirname')
#	doxygen							find_program('doxygen', required: false)
#	find 							find_program('find')
#	grep 							find_program('grep')
#	mkarray 						find_program(_tools_dir / 'mkarray_for_meson.sh')
#	mkext4 							find_program(_tools_dir / 'mkext4.py')
#	mkfat 							find_program(_tools_dir / 'mkfat.py')
#	mkuimage 						find_program(_tools_dir / 'mkuimage.py')
#	objcopy 						find_program('objcopy')
#	objdump 						find_program('objdump')
#	patch 							find_program('patch')
#	sed 							find_program('sed')
#	unzip 							find_program('unzip')
#	which 							find_program('which')
#	cpc 							find_program(_tools_dir / 'cc.sh')
#	cppcheck 						find_program('cppcheck', required: false)
#	sh								[ find_program('sh'), '-u', '-e' ]
# 	fs  							import('fs')
#	genisoimage 					find_program('genisoimage', required: false)
#	_mkisofs  						find_program('mkisofs', required: false)
#  	autocheck 						generator(find_program(_tools_dir / 'autocheck.awk'),arguments: [ '@INPUT@' ],output: '@PLAINNAME@.check.c',capture: true,)
# 	gzip							[ find_program('gzip'), '--no-name', '-9', '--stdout', '@INPUT@' ]
#	tar = 							[	find_program('tar'),
									#	'-c',
									#	'-f', '@OUTPUT@',
									#	'--mtime=1970-01-01 00:00:00Z',
									#	'--group=0',
									#	'--owner=0',
									#	'--numeric-owner',
									#	'--mode=go=rX,u+rw,a-s',
									#	'--no-acls',
									#	'--no-selinux',
									#	'--no-xattrs',
									#	'--format=ustar',
									#	'--transform', 's:@OUTDIR@/::',
									#	'@INPUT@',]
# 	_config_file					configure_file(input: meson.build_root() / 'Makefile.config',output: 'Makefile.config',copy: true,)
#	_version_file 					files(meson.source_root() / 'version')
#	_config_variables				[
									#	# Uspace and kernel
									#	'CONFIG_BAREBONE',
									#	'CONFIG_BUILD_SHARED_LIBS',
									#	'CONFIG_DEBUG',
									#	'CONFIG_DEVEL_FILES',
									#	'CONFIG_FPU',
									#	'CONFIG_LINE_DEBUG',
									#	'CONFIG_LTO',
									#	'CONFIG_PCUT_SELF_TESTS',
									#	'CONFIG_PCUT_TESTS',
									#	'CONFIG_RTLD',
									#	'CONFIG_STRIP_BINARIES',
									#	'CONFIG_UBSAN',
									#	'CONFIG_USE_SHARED_LIBS',
									#	'CROSS_TARGET',
									#	'MACHINE',
									#	'OPTIMIZATION',
									#	'PLATFORM',
									#	'PROCESSOR',
									#	'QUADFLOAT',
									#	'RDFMT',
									#
									#	# Kernel only
									#	'CONFIG_ACPI',
									#	'CONFIG_AM335X_TIMERS',
									#	'CONFIG_ASID',
									#	'CONFIG_ASID_FIFO',
									#	'CONFIG_AT_KBD',
									#	'CONFIG_BCM2835_MAILBOX',
									#	'CONFIG_DSRLNIN',
									#	'CONFIG_DSRLNOUT',
									#	'CONFIG_EGA',
									#	'CONFIG_FB',
									#	'CONFIG_GICV2',
									#	'CONFIG_I8042',
									#	'CONFIG_I8259',
									#	'CONFIG_IOMAP_BITMAP',
									#	'CONFIG_IOMAP_DUMMY',
									#	'CONFIG_KCONSOLE',
									#	'CONFIG_MAC_KBD',
									#	'CONFIG_MULTIBOOT',
									#	'CONFIG_NS16550',
									#	'CONFIG_OFW_PCI',
									#	'CONFIG_OFW_TREE',
									#	'CONFIG_OMAP_UART',
									#	'CONFIG_PAGE_HT',
									#	'CONFIG_PAGE_PT',
									#	'CONFIG_PC_KBD',
									#	'CONFIG_PL011_UART',
									#	'CONFIG_PL050',
									#	'CONFIG_S3C24XX_IRQC',
									#	'CONFIG_S3C24XX_UART',
									#	'CONFIG_SMP',
									#	'CONFIG_SOFTINT',
									#	'CONFIG_SRLN',
									#	'CONFIG_SUN_KBD',
									#	'CONFIG_SYMTAB',
									#	'CONFIG_TEST',
									#	'CONFIG_TRACE',
									#	'CONFIG_TSB',
									#	'CONFIG_UDEBUG',
									#	'CONFIG_VIA_CUDA',
									#	'MEMORY_MODEL',
									#
									#	'UARCH',
									#	'KARCH',
									#	'BARCH',
									#	'GRUB_ARCH',
									#	'UIMAGE_OS',
									#	'CONFIG_COMPRESSED_INIT',
									#]
#
#####################################################################################################################

# UARCH变量在 subdir('meson/part/read_config') 之后被定义。。。
subdir('meson/part/compiler_args')
subdir('meson/part/initrd_manifest')
subdir('meson' / 'arch' / UARCH)

## The at-sign
#
# The `atsign` variable holds the ASCII character representing the at-sign
# ('@') used in various $(AS) constructs (e.g. @progbits). On architectures that
# don't use '@' for starting a comment, `atsign` is merely '@'. However, on
# those that do use it for starting a comment (e.g. arm32), `atsign` must be
# defined as the percentile-sign ('%') in the architecture-dependent files.
#
# 如果没有定义，则将其设置为 '@'。这个变量可能用于在某些情况下需要使用 @ 符号的地方，例如在文件路径或命令行参数中.
if not is_variable('atsign')
	atsign = '@'
endif

## Some architectures need a particular string at the beginning of assembly files.
# 如果没有定义，则将其设置为空字符串 ''。这个变量可能用于在用户空间的汇编文件中添加特定的前导字符串，某些架构可能需要这样的前导字符串以确保汇编文件的正确性.
if not is_variable('uspace_as_prolog')
	uspace_as_prolog = ''
endif

# The following variables are filled in by code in the kernel and uspace subdirs.

# References to the actual binary artefacts that will be part of initrd image.
# 初始化一个空列表，用于存储将被包含在 initrd 镜像中的实际二进制文件的引用.这些二进制文件通常是系统启动时需要加载的关键组件.
rd_init_binaries = []
# Installed files.
# 初始化两个空列表，分别用于存储安装文件和安装依赖项。
install_files = []
install_deps = []

# Exported libraries and include files
# Format: [ type, source_file, destination ]
# See meson/part/exports/copy-export.sh for supported file types.
# 定义需要导出的开发文件。每个元素是一个包含三个部分的列表：文件类型、源文件和目标位置.
exported_devel_files = [
    [ 'includenamedsymlink', 'libc', 'common' ]	# 这个元素表示需要导出一个名为 libc 的头文件符号链接，目标位置为 common 目录。这种导出通常用于确保开发环境中的库和头文件能够被正确地引用和使用.
]

# Install script for headers.
# TODO: Make a list of directories and turn into script later.
# 初始化一个空列表，用于存储用户空间库的安装脚本文本。这个脚本将用于在构建过程中安装库文件和相关文件.
uspace_lib_install_script_text = []

subdir('kernel')
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
subdir('uspace')

subdir('meson/part/exports')
subdir('meson/part/initrd')

subdir('boot')

# Copy the final image to the build root,
# and write its name to a text file `image_path`.

if is_variable('POST_INPUT')
	image = custom_target(POST_OUTPUT,
		output: POST_OUTPUT,
		input: POST_INPUT,
		command: [ cp, '@INPUT@', '@OUTPUT@' ],
	)

	custom_target('image_path',
		output: 'image_path',
		input: image,
		command: [ 'echo', '@INPUT@' ],
		capture: true,
	)
else
	custom_target('image_path',
		output: 'image_path',
		command: [ 'echo' ],
		capture: true,
	)
endif

subdir('meson/part/extra_targets')
