/*
 * Copyright (c) 2005 Jakub Jermar
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

/** @addtogroup kernel_genarch
 * @{
 */

/**
 * @file
 * @brief Advanced Configuration and Power Interface (ACPI) initialization.
 */

#include <genarch/acpi/acpi.h>
#include <genarch/acpi/madt.h>
#include <arch/bios/bios.h>
#include <debug.h>
#include <mm/page.h>
#include <mm/km.h>
#include <log.h>
#include <memw.h>

#define RSDP_SIGNATURE      "RSD PTR "
#define RSDP_REVISION_OFFS  15

#define CMP_SIGNATURE(left, right) \
	(((left)[0] == (right)[0]) && \
	((left)[1] == (right)[1]) && \
	((left)[2] == (right)[2]) && \
	((left)[3] == (right)[3]))

struct acpi_rsdp *acpi_rsdp = NULL;
struct acpi_rsdt *acpi_rsdt = NULL;
struct acpi_xsdt *acpi_xsdt = NULL;

struct acpi_signature_map signature_map[] = {
	{
		(uint8_t *) "APIC",
		(void *) &acpi_madt,
		"Multiple APIC Description Table"
	}
};

static int rsdp_check(uint8_t *_rsdp)
{
	struct acpi_rsdp *rsdp = (struct acpi_rsdp *) _rsdp;
	uint8_t sum = 0;
	uint32_t i;

	for (i = 0; i < 20; i++)
		sum = (uint8_t) (sum + _rsdp[i]);

	if (sum)
		return 0; /* bad checksum */

	if (rsdp->revision == 0)
		return 1; /* ACPI 1.0 */

	for (; i < rsdp->length; i++)
		sum = (uint8_t) (sum + _rsdp[i]);

	return !sum;
}

int acpi_sdt_check(uint8_t *sdt)
{
	struct acpi_sdt_header *hdr = (struct acpi_sdt_header *) sdt;
	uint8_t sum = 0;
	unsigned int i;

	for (i = 0; i < hdr->length; i++)
		sum = (uint8_t) (sum + sdt[i]);

	return !sum;
}

static struct acpi_sdt_header *map_sdt(struct acpi_sdt_header *psdt)
{
	struct acpi_sdt_header *vhdr;
	struct acpi_sdt_header *vsdt;

	/* Start with mapping the header only. */
	vhdr = (struct acpi_sdt_header *) km_map((uintptr_t) psdt,
	    sizeof(struct acpi_sdt_header), KM_NATURAL_ALIGNMENT,
	    PAGE_READ | PAGE_NOT_CACHEABLE);

	/* Now we can map the entire structure. */
	vsdt = (struct acpi_sdt_header *) km_map((uintptr_t) psdt,
	    vhdr->length, KM_NATURAL_ALIGNMENT,
	    PAGE_WRITE | PAGE_NOT_CACHEABLE);

	// TODO: do not leak vtmp

	return vsdt;
}

static void configure_via_rsdt(void)
{
	size_t i;
	size_t j;
	size_t cnt = (acpi_rsdt->header.length - sizeof(struct acpi_sdt_header)) /
	    sizeof(uint32_t);

	for (i = 0; i < cnt; i++) {
		for (j = 0; j < sizeof(signature_map) /
		    sizeof(struct acpi_signature_map); j++) {
			struct acpi_sdt_header *hdr =
			    (struct acpi_sdt_header *) (sysarg_t) acpi_rsdt->entry[i];

			struct acpi_sdt_header *vhdr = map_sdt(hdr);
			if (CMP_SIGNATURE(vhdr->signature, signature_map[j].signature)) {
				if (!acpi_sdt_check((uint8_t *) vhdr))
					break;

				*signature_map[j].sdt_ptr = vhdr;
				LOG("%p: ACPI %s", *signature_map[j].sdt_ptr,
				    signature_map[j].description);
			}
		}
	}
}

static void configure_via_xsdt(void)
{
	size_t i;
	size_t j;
	size_t cnt = (acpi_xsdt->header.length - sizeof(struct acpi_sdt_header)) /
	    sizeof(uint64_t);

	for (i = 0; i < cnt; i++) {
		for (j = 0; j < sizeof(signature_map) /
		    sizeof(struct acpi_signature_map); j++) {
			struct acpi_sdt_header *hdr =
			    (struct acpi_sdt_header *) ((uintptr_t) acpi_xsdt->entry[i]);

			// 该条目的物理地址映射到虚拟内存中，返回指向该 ACPI 数据表头部的指针
			struct acpi_sdt_header *vhdr = map_sdt(hdr);
			if (CMP_SIGNATURE(vhdr->signature, signature_map[j].signature)) {
				if (!acpi_sdt_check((uint8_t *) vhdr))
					break;
				
				// 将映射的 ACPI 数据表的虚拟地址保存到 signature_map 中对应的指针位置。
				*signature_map[j].sdt_ptr = vhdr;
				// 打印日志，说明找到并成功映射了一个 ACPI 数据表。
				LOG("%p: ACPI %s", *signature_map[j].sdt_ptr,
				    signature_map[j].description);
			}
		}
	}
}

static uint8_t *search_rsdp(uint8_t *base, size_t len)
{
	for (size_t i = 0; i < len; i += 16) {
		if (!__builtin_memcmp(&base[i], RSDP_SIGNATURE, 8) &&
		    rsdp_check(&base[i]))
			return &base[i];
	}

	return NULL;
}

// 通过查找 RSDP（Root System Description Pointer） 来获取系统的 ACPI 数据表信息，
// 并进一步加载相关数据表，如 RSDT（Root System Description Table） 和 XSDT（Extended System Description Table）
void acpi_init(void)
{
	/*
	 * Find Root System Description Pointer
	 * 1. search first 1K of EBDA
	 * 2. search 128K starting at 0xe0000
	 */

	// ACPI 信息保存在计算机的内存中，RSDP 是指向 ACPI 数据表的指针
	uint8_t *rsdp = NULL;

	if (ebda)
		rsdp = search_rsdp((uint8_t *) PA2KA(ebda), 1024);

	if (!rsdp)
		rsdp = search_rsdp((uint8_t *) PA2KA(0xe0000), 128 * 1024);

	if (!rsdp)
		return;

	// 找到 RSDP 后，获取 RSDT 和 XSDT 地址
	acpi_rsdp = (struct acpi_rsdp *) rsdp;

	LOG("%p: ACPI Root System Description Pointer", acpi_rsdp);

	uintptr_t acpi_rsdt_p = (uintptr_t) acpi_rsdp->rsdt_address;
	uintptr_t acpi_xsdt_p = 0;

	if (acpi_rsdp->revision)
		acpi_xsdt_p = (uintptr_t) acpi_rsdp->xsdt_address;

	// 如果找到了 RSDT 或 XSDT 的地址，代码会将其映射到内存中
	if (acpi_rsdt_p)
		acpi_rsdt = (struct acpi_rsdt *) map_sdt(
		    (struct acpi_sdt_header *) acpi_rsdt_p);

	if (acpi_xsdt_p)
		acpi_xsdt = (struct acpi_xsdt *) map_sdt(
		    (struct acpi_sdt_header *) acpi_xsdt_p);

	// ACPI 的每个数据表都有一个校验和（checksum），该校验和确保数据表的完整性。
	// 代码会检查 RSDT 和 XSDT 的校验和是否正确
	if ((acpi_rsdt) && (!acpi_sdt_check((uint8_t *) acpi_rsdt))) {
		log(LF_ARCH, LVL_ERROR, "RSDT: bad checksum");
		return;
	}

	if ((acpi_xsdt) && (!acpi_sdt_check((uint8_t *) acpi_xsdt))) {
		log(LF_ARCH, LVL_ERROR, "XSDT: bad checksum");
		return;
	}

	// 根据是否找到 XSDT 或 RSDT，选择不同的配置方法
	if (acpi_xsdt)
		configure_via_xsdt();
	else if (acpi_rsdt)
		configure_via_rsdt();
}

/** @}
 */
