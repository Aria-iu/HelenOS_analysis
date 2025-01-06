/*
 * Copyright (c) 2015 Petr Pavlu
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

/** @addtogroup libcarm64
 * @{
 */
/** @file
 * @brief Thread-local storage.
 */

#ifndef _LIBC_arm64_TLS_H_
#define _LIBC_arm64_TLS_H_

#define CONFIG_TLS_VARIANT_1

/** Offsets for accessing thread-local variables are shifted 16 bytes higher. */
#define ARCH_TP_OFFSET  (sizeof(tcb_t) - 16)

/** TCB (Thread Control Block) struct.
 *
 * TLS starts just after this struct.
 */
typedef struct {
	/** Fibril data. */
	void *fibril_data;
} tcb_t;

static inline void __tcb_raw_set(void *tls)
{
	asm volatile ("msr tpidr_el0, %[tls]" : : [tls] "r" (tls));
}

static inline void *__tcb_raw_get(void)
{
	void *retval;
	asm volatile ("mrs %[tls], tpidr_el0" : [tls] "=r" (retval));
	return retval;
}

#endif

/** @}
 */
