#include <types.h>
#include <malloc.h>
#include <memory.h>
#include <vmm.h>
#include <kmalloc.h>

static struct virtual_mem kvm;
static struct mem_list k_free_mem;
static struct mem_list k_allocated_mem;

void init_kmalloc()
{
	init_vmm(&kvm);
	init_malloc(&k_free_mem, &k_allocated_mem);
}

void *kmalloc(size_t size)
{ 
  return __malloc(&kvm, &k_free_mem, &k_allocated_mem, size); 
}

int kfree(void *p)
{
	return __free(p, &k_free_mem, &k_allocated_mem);
}

/* XXX ÜBER MOCHE, mais au moins ça doit marcher... */
void *kmalloc_one_aligned_page() {
	uint32_t tmp;
	allocate_new_pages(&kvm, 2, (void **) &tmp);
	
	return (void *) memory_align_page_sup((paddr_t) tmp);
}

