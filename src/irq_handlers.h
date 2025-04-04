#ifndef _IRQ_HANDLERS_H_
#define _IRQ_HANDLERS_H_


void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);


#endif // _IRQ_HANDLERS_H_