#ifndef __TRAP_H__
#define __TRAP_H__
#include "context.h"

void set_kernel_trap_entry();

void set_user_trap_entry();

void trap_return();

void trap_from_kernel(TrapContext *ctx);

void trap_handler();

#endif