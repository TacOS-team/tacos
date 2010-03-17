#ifndef _MSR_H_
#define _MSR_H

#define MSR_SYSENTER_CS		0x0174
#define MSR_SYSENTER_ESP	0x0175
#define MSR_SYSTENTE_EIP	0x0176

#define write_msr(reg, msr_low, msr_high) 	\
		__asm__ volatile (					\
			"wrmsr\n\t"						\
			: /* No output */				\
			: "c" (reg), "a" (msr_low), "d" (msr_high))
			
#define read_msr(reg, msr_low, msr_high) 		\
		__asm__ volatile (						\
			"rdmsr\n\t"							\
			: "=a" (*msr_low), "=d" (*msr_high)	\
			: "c" (reg))
	
		

#endif
