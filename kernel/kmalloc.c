#include <types.h>
#include <malloc.h>
#include <memory.h>
#include <vmm.h>
#include <kmalloc.h>

static struct virtual_mem kvm;

void init_kmalloc()
{
	init_vmm(&kvm);
	init_malloc();
}

void *kmalloc(size_t size)
{ 
  return __malloc(&kvm, size); 
}

int kfree(void *p)
{
	return free(p);
}

/* XXX ÜBER MOCHE, mais au moins ça doit marcher... */
void *kmalloc_one_aligned_page() {
	uint32_t tmp;
	allocate_new_pages(&kvm, 2, (void **) &tmp);
	
	return (void *) memory_align_page_sup((paddr_t) tmp);
}

struct virtual_mem *get_kvm() {
	return &kvm;
}

