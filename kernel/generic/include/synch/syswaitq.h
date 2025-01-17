/*
 * Copyright (c) 2018 Jakub Jermar
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

/** @addtogroup kernel_sync
 * @{
 */
/** @file
 */
// 操作系统内核中的等待队列（wait queue）模块，主要用于进程或线程间的同步

#ifndef KERN_SYS_WAITQ_H_
#define KERN_SYS_WAITQ_H_

#include <typedefs.h>
#include <abi/cap.h>
#include <cap/cap.h>

extern kobject_ops_t waitq_kobject_ops;

// 初始化用户等待队列子系统
extern void sys_waitq_init(void);

// 清理退出任务所持有的所有等待队列能力
extern void sys_waitq_task_cleanup(void);

// 当前任务创建一个等待队列
extern sys_errno_t sys_waitq_create(uspace_ptr_cap_waitq_handle_t);
// 使当前任务在指定的等待队列中睡眠，直到被唤醒或者超时。
extern sys_errno_t sys_waitq_sleep(cap_waitq_handle_t, uint32_t, unsigned int);
// 用于唤醒在指定等待队列中睡眠的一个任务
extern sys_errno_t sys_waitq_wakeup(cap_waitq_handle_t);
// 销毁一个等待队列
extern sys_errno_t sys_waitq_destroy(cap_waitq_handle_t);

#endif

/** @}
 */
