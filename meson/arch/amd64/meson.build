#
# Copyright (c) 2021 Jiri Svoboda
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

# 用户空间编译参数
arch_uspace_c_args = [
	'-D__LE__',						# 定义一个宏 __LE__，通常用于指示目标平台是小端字节序
	'-fno-omit-frame-pointer',		# 告诉编译器不要省略帧指针（frame pointer）。保留帧指针有助于调试和堆栈跟踪，因为它允许调试器更容易地回溯函数调用堆栈.
]

arch_kernel_c_args = arch_uspace_c_args + [
	# 禁用 SSE 和 SSE2 指令集。这可能是因为内核需要在不支持这些指令集的硬件上运行，或者是为了确保内核代码的兼容性和可移植性.
	'-mno-sse',
	'-mno-sse2',
	'-mcmodel=' + MEMORY_MODEL, # 设置代码模型（code model）
	'-mno-red-zone',			# 禁用红区（red zone）
	'-fno-unwind-tables',		# 不生成异常处理的展开表（unwind tables）。这可以减少内核的大小和复杂性，因为内核通常不使用 C++ 的异常处理机制.
]

# 如果 PROCESSOR 变量的值是 opteron
if PROCESSOR == 'opteron'
	arch_kernel_c_args += '-march=opteron'	# 添加 '-march=opteron' 参数，指定针对 Opteron 架构进行优化.
endif

arch_kernel_link_args = [ '-Wl,-z,max-page-size=0x1000', '-nostdlib' ]				# 设置最大页面大小为 4KB（0x1000）。这确保了内核在链接时使用与系统页面大小一致的页面大小.
																					# 不链接标准库。内核通常不依赖于标准库，因为它需要直接与硬件交互并提供底层功能.
arch_uspace_link_args = [ '-Wl,-z,max-page-size=0x1000', '-nostdlib', '-lgcc' ]		# '-lgcc'：链接 GCC 的运行时库。用户空间程序通常需要链接 GCC 的运行时库以支持一些基本的运行时功能.

# Binaries to be included in a barebone build
# 基础构建（barebone build）中，除了之前定义的基础可执行文件外，还额外包含了 app/edit（文本编辑器）、app/mixerctl（音频混音器控制）、app/wavplay（WAV 文件播放器）等应用程序，以及 srv/audio/hound（音频服务）.
rd_essential += [
	'app/edit',
	'app/mixerctl',
	'app/wavplay',

	'srv/audio/hound',
]

# Drivers to be included in a barebone build
# 在基础构建中，除了之前定义的基础驱动程序外，还额外包含了多个与 PC 平台相关的驱动程序，如中断控制器驱动（drv/intctl/apic、drv/intctl/i8259）、
# 平台驱动（drv/platform/pc）、IDE 硬盘驱动（drv/block/isa-ide）、PCI 总线驱动（drv/bus/pci/pciintel）、
# ISA 总线驱动（drv/bus/isa）、声霸卡音频驱动（drv/audio/sb16）、键盘和鼠标驱动（drv/hid/ps2mouse、drv/hid/xtkbd）等.
rd_essential_drv += [
	'drv/intctl/apic',
	'drv/intctl/i8259',
	'drv/platform/pc',
	'drv/block/isa-ide',
	'drv/bus/pci/pciintel',
	'drv/bus/isa',
	'drv/audio/sb16',
	'drv/char/i8042',
	'drv/hid/ps2mouse',
	'drv/hid/xtkbd',
]

# Drivers to be included in a regular build
# 常规构建中，除了基础构建中的驱动程序外，还包含了更多的驱动程序，如高级音频驱动（drv/audio/hdaudio）、AHCI 硬盘驱动（drv/block/ahci）、
# USB 相关驱动（多个 USB 总线和设备驱动）、网络接口卡驱动（多个以太网和无线网卡驱动）、串口和并口驱动（drv/char/ns8250、drv/char/pc-lpt）、
# 时钟驱动（drv/time/cmos-rtc）等. 这些驱动程序扩展了系统的硬件支持范围，使得系统能够在更多类型的硬件上运行，并提供更丰富的功能.
rd_drv += [
	'drv/audio/hdaudio',
	'drv/audio/sb16',
	'drv/block/ahci',
	'drv/block/isa-ide',
	'drv/block/pc-floppy',
	'drv/block/pci-ide',
	'drv/block/usbmast',
	'drv/block/virtio-blk',
	'drv/bus/isa',
	'drv/bus/pci/pciintel',
	'drv/bus/usb/ehci',
	'drv/bus/usb/ohci',
	'drv/bus/usb/uhci',
	'drv/bus/usb/usbdiag',
	'drv/bus/usb/usbflbk',
	'drv/bus/usb/usbhub',
	'drv/bus/usb/usbmid',
	'drv/bus/usb/vhc',
	'drv/bus/usb/xhci',
	'drv/char/i8042',
	'drv/char/ns8250',
	'drv/char/pc-lpt',
	'drv/hid/ps2mouse',
	'drv/hid/xtkbd',
	'drv/hid/usbhid',
	'drv/intctl/apic',
	'drv/intctl/i8259',
	'drv/nic/ar9271',
	'drv/nic/e1k',
	'drv/nic/ne2k',
	'drv/nic/rtl8139',
	'drv/nic/rtl8169',
	'drv/nic/virtio-net',
	'drv/platform/pc',
	'drv/time/cmos-rtc',
]
