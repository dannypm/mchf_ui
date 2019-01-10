#ifndef __WATCHDOG_H
#define __WATCHDOG_H

void watchdog_check_reset(void);
void watchdog_init(void);
void watchdog_refresh(void);

#endif
