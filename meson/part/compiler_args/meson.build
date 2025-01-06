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

# This file sets the architecture-independent compiler flags used throughout
# this repository.
# For architecture-specific flags, see $srcroot/meson/cross/$arch.

# 将 config.h 文件作为宏定义文件包含到编译过程中。这意味着 config.h 中定义的所有宏都会被包含在编译中。这些参数适用于 C 语言编译。
add_project_arguments(
	# TODO: Remove from project arguments and only use where necessary.
	# Any change in config forces everything to rebuild due to lack of granularity here.
	'-imacros', meson.build_root() / 'config.h',
	language : [ 'c' ],
)

# 用于为项目添加链接参数。
# -Wl,--gc-sections：告诉链接器丢弃未使用的代码段，以减小最终可执行文件的大小.
# -Wl,--warn-common：在链接时警告重复的公共符号.
# -Wl,--fatal-warnings：将所有链接警告视为错误.
# -Wl,-z,text：确保所有代码段都是只读的.
# language : [ 'c', 'cpp' ]：指定这些参数适用于 C 和 C++ 语言编译.
add_project_link_arguments(
	'-Wl,--gc-sections',
	'-Wl,--warn-common',
	'-Wl,--fatal-warnings',
	'-Wl,-z,text',
	language : [ 'c', 'cpp' ],
)

# TODO: enable more warnings
# FIXME: -fno-builtin-strftime works around seemingly spurious format warning.
# We should investigate what's going on there.

# 这是一个包含多个编译标志的列表，用于增强编译器的警告和优化功能.
extra_common_flags = [
	'-O' + OPTIMIZATION,				# 设置优化级别
	'-fexec-charset=UTF-8',				# 指定执行和输入字符集为 UTF-8.
	'-finput-charset=UTF-8',

	'-D_HELENOS_SOURCE',				# 定义一个宏 _HELENOS_SOURCE

	'-Wa,--fatal-warnings',				# 这些标志分别用于汇编器和链接器，确保警告被视为错误和代码段只读.
	'-Wl,-z,text',

	'-Wall',							# 启用更多的编译警告.
	'-Wextra',
	'-Wwrite-strings',
	'-Wunknown-pragmas',

	'-Wno-unused-parameter',			# 忽略未使用的函数参数警告.

	'-pipe',							# 使用管道而不是临时文件进行编译过程中的通信，可以提高编译速度.

	'-ffunction-sections',				# 将每个函数和数据分配到单独的段，以便链接器可以丢弃未使用的段.
	'-fdata-sections',
	'-fno-common',						# 不使用公共符号，避免符号重定义问题.
	'-fdebug-prefix-map=' + meson.source_root() + '/=',		# 设置调试信息的路径映射，以便调试器可以正确找到源文件.允许你在生成的调试信息中将源文件路径从一个位置映射到另一个位置
										# 果源代码根目录是 /path/to/source，那么源文件 /path/to/source/subdir/file.c 在调试信息中会被表示为 subdir/file.c.
	'-fdebug-prefix-map=../../=',		# 如果你的源文件路径是 ../../../subdir/file.c，使用这个选项后，调试信息中的路径会被表示为 subdir/file.c.
										# 这种映射可以帮助调试器更准确地定位源文件，特别是在复杂的目录结构中.
]

# 判断用于检查当前使用的编译器是否不是 Clang.
if cc.get_id() != 'clang'
	# Clang's own headers emit macro redefinition warnings. Clang 的系统头文件可能会导致宏重定义警告，因此在使用 Clang 时不添加这个标志.
	extra_common_flags += '-Wsystem-headers'	# 不是 Clang，添加 -Wsystem-headers 标志，该标志用于警告系统头文件中的宏重定义问题.
endif

if UARCH != 'ia64'
	extra_common_flags += [ '-fvar-tracking-assignments' ] # 如果不是 IA-64，则添加 -fvar-tracking-assignments 标志，用于跟踪变量的赋值情况，帮助调试.
endif

# 判断是否启用了调试配置
if CONFIG_DEBUG
	extra_common_flags += [ '-Werror' ]				# 如果启用了调试配置，则添加 -Werror 标志，将所有警告视为错误，确保代码质量.
endif

# 判断是否启用了行调试配置.
if CONFIG_LINE_DEBUG
	extra_common_flags += [ '-gdwarf-5', '-g3' ]	# 如果启用了行调试配置，则添加 -gdwarf-5 和 -g3 标志，用于生成详细的调试信息.
endif

# 定义了一个包含多个 C 语言编译标志的列表.
extra_cflags = extra_common_flags + [
	'-Wmissing-prototypes',
	'-Werror-implicit-function-declaration',

	'-Wno-missing-braces',
	'-Wno-missing-field-initializers',
	'-Wno-unused-command-line-argument',
	'-Wno-unused-parameter',
	'-Wno-typedef-redefinition',
	'-Wno-clobbered',
	'-Wno-nonnull-compare',

	'-fno-builtin-strftime',
]

if CONFIG_UBSAN
	extra_cflags += '-fsanitize=undefined'
endif

# 定义了一个包含多个 C++ 语言编译标志的列表.
extra_cppflags = extra_common_flags + [
	'-fno-exceptions',
	'-Wno-misleading-indentation',
	'-frtti',
]

# 将 C 和 C++ 语言的编译标志分别存储在一个字典中.
w_flags = {
	'c': extra_cflags,
	'cpp': extra_cppflags,
}

# TODO: To remove noise in Meson output, we may want to cut down on
#       the explicitly checked flags and just enable those supported by
#       both gcc and clang unconditionally.

# Process flags. Only sets those that compiler supports.
foreach lang : [ 'c', 'cpp' ]
	# get_supported_arguments 方法获取编译器支持的编译标志.
	extra_cflags = meson.get_compiler(lang).get_supported_arguments(w_flags.get(lang))
	# 使用 add_project_arguments 和 add_project_link_arguments 函数将支持的编译标志添加到项目中.
	add_project_arguments(extra_cflags, language : [ lang ])
	add_project_link_arguments(extra_cflags, language : [ lang ])
endforeach

# This flag is needed at several places, hence we define it here.
#
# For backwards compatibility we try to detect --no-warn-rwx-segments.
# However, the autodetection done by Meson also results in
# "cannot find entry symbol _start; defaulting to 00000000004000b0"
# thus the option is never supported alone. So when detecting we also
# specify --entry=main so that the stub source provided by Meson is build
# correctly.
# 定义了一个用于存储链接标志的列表.
ldflags_ignore_rwx_segments = []
# 使用 cc.has_link_argument 方法检查编译器是否支持 -Wl,--no-warn-rwx-segments,--entry=main 链接标志.
if cc.has_link_argument('-Wl,--no-warn-rwx-segments,--entry=main')
	# 如果支持，则将 -Wl,--no-warn-rwx-segments 添加到 ldflags_ignore_rwx_segments 列表中.这个标志用于忽略 RWX 段的警告，确保链接过程的顺利进行.
    ldflags_ignore_rwx_segments += ['-Wl,--no-warn-rwx-segments']
endif
