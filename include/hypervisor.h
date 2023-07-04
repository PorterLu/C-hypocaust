#ifndef __HYPERVISOR_H__
#define __HYPERVISOR_H__

#include "types.h"

void init_hypervisor();
bool detect_h_extension();

#define concat(a, b) a ## b

#endif
