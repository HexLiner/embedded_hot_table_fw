//  ***************************************************************************
/// @file    sysclk.h
/// @brief   System clock driver
//  ***************************************************************************
#ifndef _SYSCLK_H_
#define _SYSCLK_H_


#include "common/mcu.h"
#include "common/lib_base.h"


extern void sysclk_init(const sysclk_extcfg_t *ext_cfg);

extern void sysclk_enable_peripheral(const peripheral_t peripheral);
extern void sysclk_disable_peripheral(const peripheral_t peripheral);
extern void sysclk_enable_peripheral_in_lp_mode(const peripheral_t peripheral);
extern void sysclk_disable_peripheral_in_lp_mode(const peripheral_t peripheral);

extern void sysclk_get_bus_freq(sysclk_bus_t bus, uint32_t *freq);
extern void sysclk_get_peripheral_freq(const peripheral_t peripheral, uint32_t *freq);



#endif // _SYSCLK_H_
