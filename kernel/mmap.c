#include <klog.h>
#include <mmap.h>
#include <scheduler.h>
#include <kprocess.h>
#include <kmalloc.h>

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
		klog("%d -- %d", aux->addr, aux->addr + aux->length);
		aux = aux->next;
	}
}

SYSCALL_HANDLER2(sys_mmap, struct mmap_data* data, void** ret) {
	klog("sys mmap %d", data->length);

	process_t *process = get_current_process();

	size_t real_alloc_size;
	sys_vmm(data->length, ret, &real_alloc_size);

	add_region(process, (vaddr_t)*ret, data);

	print_regions(process);
}

