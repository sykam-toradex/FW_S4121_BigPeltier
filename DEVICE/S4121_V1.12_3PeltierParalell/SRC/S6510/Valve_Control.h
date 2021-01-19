#ifndef S435_VALVE_CONTROL_H
#define S435_VALVE_CONTROL_H


__task void    TSK_ValveControl(void);
__task void    TSK_Counter(void);

extern ushort   u16_JobIndex;
extern OS_MBX   MBX_SensorFlag;
void Digital_IO(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio);


#endif
