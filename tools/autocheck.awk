#!/usr/bin/awk -f
#
# Copyright (c) 2018 CZ.NIC, z.s.p.o.
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

# Authors:
# 	Jiří Zárevúcky <jiri.zarevucky@nic.cz>

# 这段代码是一个用 awk 编写的脚本，主要用于处理 C 语言的结构体定义文件，
# 并生成一些静态断言来验证结构体的大小和成员的偏移量。
# 
#	输入: 一个包含 C 语言结构体定义的文件。
# 	输出: 生成一些静态断言（_Static_assert），用于验证结构体的大小和成员的偏移量是否符合预期。
#
# 	输入：example_structs.h
#	typedef struct example_struct {
#	    int a;
#	    char b;
#	    double c;
#	} example_struct_t;
#	输出：example_structs.check.c
#	// Generated file. Fix the included header if static assert fails.
#	// Inlined "example_structs.h"
#	typedef struct example_struct {
#		int a;
#		char b;
#		double c;
#	} example_struct_t;
#
#	_Static_assert(EXAMPLE_STRUCT_SIZE == sizeof(struct example_struct), "");
#	_Static_assert(EXAMPLE_STRUCT_OFFSET_A == __builtin_offsetof(struct example_struct, a), "");
#	_Static_assert(EXAMPLE_STRUCT_SIZE_A == sizeof(((struct example_struct){ }).a), "");
#	_Static_assert(EXAMPLE_STRUCT_OFFSET_B == __builtin_offsetof(struct example_struct, b), "");
#	_Static_assert(EXAMPLE_STRUCT_SIZE_B == sizeof(((struct example_struct){ }).b), "");
#	_Static_assert(EXAMPLE_STRUCT_OFFSET_C == __builtin_offsetof(struct example_struct, c), "");
#	_Static_assert(EXAMPLE_STRUCT_SIZE_C == sizeof(((struct example_struct){ }).c), "");


BEGIN {
	filename = ARGV[1]
	output_lines = 0
	print "// Generated file. Fix the included header if static assert fails."
	print "// Inlined \"" filename "\""
}

{
	print $0
}

/}.*;/ {
	pattern = "}( __attribute__\\(.*\\))? (" struct_name "_t)?;"
	if ($0 !~ pattern) {
		print("Bad struct ending: " $0) > "/dev/stderr"
		exit 1
	}
	macro_name = toupper(struct_name) "_SIZE"
	output[output_lines++] = "_Static_assert(" macro_name " == sizeof(struct " struct_name "), \"\");"
	struct_name = ""
}

/;$/ {
	if (struct_name != "") {
		# Remove array subscript, if any.
		sub("(\\[.*\\])?;", "", $0)
		member = $NF

		macro_name = toupper(struct_name) "_OFFSET_" toupper(member)
		output[output_lines++] = "_Static_assert(" macro_name " == __builtin_offsetof(struct " struct_name ", " member "), \"\");"

		macro_name = toupper(struct_name) "_SIZE_" toupper(member)
		output[output_lines++] = "#ifdef " macro_name
		output[output_lines++] = "_Static_assert(" macro_name " == sizeof(((struct " struct_name "){ })." member "), \"\");"
		output[output_lines++] = "#endif"
	}
}

/^typedef struct .* \{/ {
	struct_name = $3
}

END {
	for ( i = 0; i < output_lines; i++ ) {
		print output[i]
	}
}
