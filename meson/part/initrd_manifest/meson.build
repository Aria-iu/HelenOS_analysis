#
# Copyright (c) 2024 Jiri Svoboda
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

# Init binaries. These are actually baked into the bootloader.
# 在启动时加载的可执行文件。实际上是嵌入到引导加载程序bootloader中的。
rd_init = [
	# ELF file with DWARF debug sections for kernel.
	# Kernel detects this entry by name.
	'kernel.dbg',	# 包含 DWARF 调试信息的 ELF 文件，用于内核调试.内核通过文件名来检测这个条目.

	# IMPORTANT: The order of entries is important for bootloader! 系统启动时需要加载的服务和应用程序.
	'srv/ns',
	'srv/loader',
	'app/init',
	'srv/locsrv',
	'srv/bd/rd',
	'srv/vfs',
	'srv/logger',
	'srv/fs/' + RDFMT,
]

# Binaries allowed on the initrd image when CONFIG_BAREBONE is enabled.
# 包含了在启用 CONFIG_BAREBONE 配置时允许在 initrd 镜像中包含的基础可执行文件.
rd_essential = [
	'app/bdsh',
	'app/getterm',
	'app/kio',
	'app/tester',

	'srv/devman',
	'srv/fs/locfs',
	'srv/hid/console',
	'srv/hid/input',
	'srv/hid/output',
	'srv/klog',
	'srv/system',
]

# 包含了基础驱动程序，如根设备驱动程序（drv/root/root、drv/root/virt）和帧缓冲驱动程序（drv/fb/kfb）.
rd_essential_drv = [
	'drv/root/root',
	'drv/root/virt',
	'drv/fb/kfb',
]
# rd_drv 被设置为 rd_essential_drv，表示在默认情况下，只有这些基础驱动程序会被包含在 initrd 中.
rd_drv = rd_essential_drv

if CONFIG_FB
	# 如果 CONFIG_FB 被启用，则将一些与帧缓冲相关的可执行文件添加到 rd_essential 列表中，包括 app/taskbar、app/terminal、srv/hid/display 等.
	rd_essential += [
		'app/taskbar',
		'app/terminal',

		'srv/hid/display',
	]
endif
