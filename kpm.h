//kpem_defines.h
#ifndef KPM_H
#define KPM_H

#include "types.h"

void Kpm_init(void);
u32  colscan(void);
u32  Row_Check(void);
u32  Col_Check(void);
u32  Key_Scan(void);
u32  ReadNum(void);

#endif // KPM_H

