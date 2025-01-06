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

# 找到Meson需要的所以程序和工具。
# 无论什么时候需要一个外部命令，首先将那个命令找到，并赋值在这里的一个变量中，不要直接使用。
#
# Find all the programs and tools used by meson.
# Whenever you use an external command anywhere,
# first find and assign it to a variable here. Don't use its name directly.
# This way it's obvious what programs are used, and for some we can use
# the same argument list everywhere.

############################################################
#						变量表
#	debug_options					false
# 	debug_shell_scripts				false
#############################################################

_tools_dir = meson.source_root() / 'tools'

basename = find_program('basename')
cc = meson.get_compiler('c')
config_py = find_program(_tools_dir / 'config.py')
cp = find_program('cp')
dirname = find_program('dirname')
doxygen = find_program('doxygen', required: false)
find = find_program('find')
grep = find_program('grep')
mkarray = find_program(_tools_dir / 'mkarray_for_meson.sh')
mkext4 = find_program(_tools_dir / 'mkext4.py')
mkfat = find_program(_tools_dir / 'mkfat.py')
mkuimage = find_program(_tools_dir / 'mkuimage.py')
objcopy = find_program('objcopy')
objdump = find_program('objdump')
patch = find_program('patch')
sed = find_program('sed')
unzip = find_program('unzip')
which = find_program('which')
cpc = find_program(_tools_dir / 'cc.sh')
cppcheck = find_program('cppcheck', required: false)

sh = [ find_program('sh'), '-u', '-e' ]
if debug_shell_scripts
	sh += '-x'
endif

# On some distributions (e.g. Arch Linux), genisoimage is actually a symbolic
# link to mkisofs. However, they do not accept exactly the same options.
# Thus we check if genisoimage is a symbolic link to mkisofs and if it is so,
# we switch to mkisofs as that is the native application available.
# 用于处理一些与文件系统相关的构建任务，特别是与生成 ISO 文件和压缩文件相关的任务。

fs = import('fs')

# 检查 genisoimage 和 mkisofs 的可用性：
#首先，脚本尝试查找 genisoimage 和 mkisofs 程序。
#如果 genisoimage 存在且不是指向 mkisofs 的符号链接，则使用 genisoimage。
#如果 genisoimage 是指向 mkisofs 的符号链接且 mkisofs 可用，则使用 mkisofs。
#如果两者都不可用，则尝试使用 xorriso 作为替代方案，通过 -as genisoimage 选项来模拟 genisoimage 的行为.
#如果以上工具都不可用，则报错提示需要 genisoimage、mkisofs 或 xorriso 中的一个.
genisoimage = find_program('genisoimage', required: false)
_mkisofs = find_program('mkisofs', required: false)
if genisoimage.found() and not (fs.is_symlink(genisoimage.full_path()) and _mkisofs.found() and fs.is_samepath(genisoimage.full_path(), _mkisofs.full_path()))
	genisoimage_type = 'genisoimage'
else
	genisoimage = _mkisofs
	if genisoimage.found()
		genisoimage_type = 'mkisofs'
	else
		xorriso = find_program('xorriso', required: false)
		if xorriso.found()
			genisoimage = [ xorriso, '-as', 'genisoimage' ]
			genisoimage_type = 'genisoimage'
		else
			error('Need genisoimage, mkisofs or xorriso.')
		endif
	endif
endif

# 定义 autocheck 生成器：
# 使用 autocheck.awk 脚本生成 .check.c 文件。这个生成器会将输入文件作为参数传递给 autocheck.awk 脚本，并捕获输出生成 .check.c 文件.
autocheck = generator(find_program(_tools_dir / 'autocheck.awk'),
	arguments: [ '@INPUT@' ],
	output: '@PLAINNAME@.check.c',
	capture: true,
)

# TODO: bug in Meson
#gzip = generator(find_program('gzip'),
#	arguments: [ '--no-name', '-9', '--stdout', '@INPUT@' ],
#	output: '@PLAINNAME@.gz',
#	capture: true,
#)

# 定义 gzip 命令：
# 由于 Meson 的 generator 功能存在 bug，所以这里直接定义了一个 gzip 命令列表，用于压缩文件。该命令使用 gzip 程序，并设置了一些参数以确保压缩文件的可重复性.
gzip = [ find_program('gzip'), '--no-name', '-9', '--stdout', '@INPUT@' ]

# Tar's arguments make sure that the archive is reproducible.
# 定义 tar 命令：
# 定义了一个 tar 命令列表，用于创建一个可重复的 tar 归档文件。命令中包含了一系列参数，用于确保归档文件的时间戳、权限、所有者等信息是固定的，从而使得生成的归档文件是可重复的.
tar = [
	find_program('tar'),
	'-c',
	'-f', '@OUTPUT@',
	'--mtime=1970-01-01 00:00:00Z',
	'--group=0',
	'--owner=0',
	'--numeric-owner',
	'--mode=go=rX,u+rw,a-s',
	'--no-acls',
	'--no-selinux',
	'--no-xattrs',
	'--format=ustar',
	'--transform', 's:@OUTDIR@/::',
	'@INPUT@',
]

############################################################
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

#############################################################