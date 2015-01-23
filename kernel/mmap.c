#include <klog.h>
#include <mmap.h>
#include <scheduler.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <memory.h>
#include <klibc/string.h>

struct mmap_region {
	vaddr_t addr;
	size_t length;
	int prot;
	int flags;
	int fd;
	off_t offset;

	struct mmap_region* next;
};


static void add_region(process_t *process, vaddr_t addr, struct mmap_data *data) {
	struct mmap_region* cell = kmalloc(sizeof(struct mmap_region));

	cell->addr = addr;
	cell->length = data->length;
	cell->prot = data->prot;
	cell->flags = data->flags;
	cell->fd = data->fd;
	cell->offset = data->offset;

	struct mmap_region* aux = process->list_regions;
	struct mmap_region* prec = NULL;
	while (aux && aux->addr < cell->addr) {
		prec = aux;
		aux = aux->next;
	}

	if (prec == NULL) {
		cell->next = process->list_regions;
		process->list_regions = cell;
	} else {
		prec->next = cell;
		cell->next = aux;
	}
}

void print_regions(process_t *process) {
	struct mmap_region* aux = process->list_regions;
	while (aux) {
		klog("%u -- %u", aux->addr, aux->addr + aux->length);
		aux = aux->next;
	}
}

int is_mmaped(vaddr_t addr) {
	process_t *process = get_current_process();
	struct mmap_region* aux = process->list_regions;
	while (aux && aux->addr <= addr) {
		if (aux->addr + aux->length > addr) {
			int page_addr = addr & ~(PAGE_SIZE - 1);
			// mmap anonyme pour le moment :
			map(memory_reserve_page_frame(), page_addr, 1, (aux->prot & PROT_WRITE) > 0);
			memset((void*)page_addr, 0, PAGE_SIZE);
			return 1;
		}
		aux = aux->next;
	}

	return 0;
}

SYSCALL_HANDLER2(sys_mmap, struct mmap_data* data, void** ret) {
	klog("sys mmap %d", data->length);

	process_t *process = get_current_process();

	vaddr_t current = USER_PROCESS_MMAP;
	vaddr_t top_heap = process->vm->vmm_top;

	struct mmap_region* aux = process->list_regions;
	while (aux) {
		vaddr_t last = aux->addr + aux->length;

		if (current - last > data->length) {
			break;
		}

		current = aux->addr;
		aux = aux->next;
	}

	if (current - top_heap < data->length) {
		klog("out of mem");
		*ret = NULL;
		return;
	}

	*ret = (void*) (current - data->length);

	add_region(process, (vaddr_t)*ret, data);

	print_regions(process);
}

