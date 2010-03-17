#ifndef _MSR_H_
#define _MSR_H

#define MSR_SYSENTER_CS		0x0174
#define MSR_SYSENTER_ESP	0x0175
#define MSR_SYSTENTE_EIP	0x0176

void write_msr(uint32_t reg, uint32_t msr_high, uint32_t msr_low);
void read_msr(uint32_t reg, uint32_t* msr_high, uint32_t msr_low);

#endif
