
int check_vm86()
{
	int ret = 0;
	asm volatile(
		"smsw %%ax\n\t"
		"and $1, %%eax\n\t"
		"mov %%eax, %0"
		:"=m"(ret));
	return ret;
}
	
					
