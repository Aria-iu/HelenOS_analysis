/*
 * Copyright (c) 2001-2004 Jakub Jermar
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** @addtogroup kernel_amd64_mm
 * @{
 */
/** @file
 */

#include <arch/mm/page.h>
#include <genarch/mm/page_pt.h>
#include <arch/mm/frame.h>
#include <mm/page.h>
#include <mm/frame.h>
#include <mm/as.h>
#include <arch/asm.h>
#include <config.h>
#include <interrupt.h>
#include <panic.h>
#include <align.h>
#include <macros.h>

void page_arch_init(void)
{
	// �����AP Boot��д��cr3�Ĵ���֮�󷵻ؼ��ɡ�
	if (config.cpu_active > 1) {
		write_cr3((uintptr_t) AS_KERNEL->genarch.page_table);
		return;
	}

	uintptr_t cur;
	unsigned int identity_flags =
	    PAGE_GLOBAL | PAGE_CACHEABLE | PAGE_EXEC | PAGE_WRITE | PAGE_READ;

	page_mapping_operations = &pt_mapping_operations;

	page_table_lock(AS_KERNEL, true);

	/*
	 * PA2KA(identity) mapping for all low-memory frames.
	 * ������low-memory��֡ӳ�䵽���ڵ��ں������ַ��
	 */
	// config.identity_size = 0x80000000
    // QEMU�������ڴ�Ľ��ޣ��ο���ͼmem2.png�������п����ڴ棨aviliable����� 7fdf000
	// config.physmem_end   = 7fdf000
    // ���ｫ 0-7fdf000������FRAME_SIZE��С��0-7fdf�����뵽AS_KERNEL�ں˵�ַ�ռ��ӳ���С�
	for (cur = 0; cur < min(config.identity_size, config.physmem_end);
	    cur += FRAME_SIZE)
		page_mapping_insert(AS_KERNEL, PA2KA(cur), cur, identity_flags);

	page_table_unlock(AS_KERNEL, true);

	// ע��Page Fault�Ĵ�������
	exc_register(VECTOR_PF, "page_fault", true, (iroutine_t) page_fault);
	// ˢ��ҳ��
	// ��д�Ĵ����ĺ���ʵ���� /kernel/arch/'UARCH'/include/arch/asm.h��ʹ�ú궨����������
	write_cr3((uintptr_t) AS_KERNEL->genarch.page_table);
}

void page_fault(unsigned int n, istate_t *istate)
{
	// �ҵ�ҳ����������ַ��
	uintptr_t badvaddr = read_cr2();

	if (istate->error_word & PFERR_CODE_RSVD)
		panic("Reserved bit set in page table entry.");

	pf_access_t access;

	if (istate->error_word & PFERR_CODE_RW)
		access = PF_ACCESS_WRITE;
	else if (istate->error_word & PFERR_CODE_ID)
		access = PF_ACCESS_EXEC;
	else
		access = PF_ACCESS_READ;

	// ����ҳ����
	(void) as_page_fault(badvaddr, access, istate);
}

/** @}
 */
