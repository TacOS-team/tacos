#include <types.h>

void write_msr(uint32_t reg, uint32_t msr_high, uint32_t msr_low)
{
	/* 
	 * 
	 *	reg -> EXC
	 *	msr_high -> EDX
	 *	msr_low -> EAX
	 *	WRMSR
	 * 
	 */
	
	__asm__ volatile (
		"mov %0, %%ecx\n\t"
        "mov %1, %%edx\n\t"
        "mov %2, %%eax\n\t"
        "WRMSR\n\t"
        : // No input parameters
        :"m" (reg),"m" (msr_high), "m"(msr_low)     // Input parameters
        );
	
}

void read_msr(uint32_t reg, uint32_t* msr_high, uint32_t msr_low)
{
	__asm__ volatile (
		"mov %0, %%ecx\n\t"
		"RDMSR\n\t"
		"mov %%edx, %1\n\t"
		"mov %%eax, %2\n\t"
		:"=m"(msr_high), "=m"(msr_low)
		:"m"(reg)
		);
}

