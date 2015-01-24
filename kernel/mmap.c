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
	while (aux && aux->addr > cell->addr) {
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
		klog("%x -- %x", aux->addr, aux->addr + aux->length);
		aux = aux->next;
	}
}

int is_mmaped(vaddr_t addr) {
	process_t *process = get_current_process();
	struct mmap_region* aux = process->list_regions;
	while (aux && aux->addr > addr) {
		aux = aux->next;
	}

	if (aux) {
		if (aux->addr + aux->length > addr) {
			vaddr_t page_addr = ALIGN_PAGE_INF(addr);

			map(memory_reserve_page_frame(), page_addr, 1, (aux->prot & PROT_WRITE) > 0);
			if (aux->flags & MAP_ANONYMOUS) {
				memset((void*)page_addr, 0, PAGE_SIZE);
				return 1;
			} else if(aux->fd > -1) {
				open_file_descriptor* ofd = process->fd[aux->fd];
				if (ofd) {
					asm("sti");			/* Et on tente de revenir au choses normales */
					ofd->f_ops->seek(ofd, aux->offset + page_addr - aux->addr, SEEK_SET);
					ofd->f_ops->read(ofd, (void*) page_addr, PAGE_SIZE);
					asm("cli");
					return 1;
				} else {
					return 0;
				}
			}
		}
	}

	return 0;
}

SYSCALL_HANDLER2(sys_mmap, struct mmap_data* data, void** ret) {
	klog("sys mmap %d", data->length);

	process_t *process = get_current_process();
	vaddr_t current = USER_PROCESS_MMAP;

	if (data->flags & MAP_FIXED) {
		if ((vaddr_t)data->addr != (vaddr_t)ALIGN_PAGE_INF(data->addr)) {
			*ret = NULL;
			return;
		}

		struct mmap_region* aux = process->list_regions;
		while (aux && aux->addr + aux->length > (vaddr_t) data->addr) {
			current = aux->addr;
			aux = aux->next;
		}
		if ((vaddr_t)data->addr + data->length > current) {
			*ret = NULL;
			return;
		}
		*ret = data->addr;
	} else {

		struct mmap_region* aux = process->list_regions;
		while (aux) {
			vaddr_t last = ALIGN_PAGE_SUP(aux->addr + aux->length);

			if (current - last >= data->length) {
				break;
			}

			current = aux->addr;
			aux = aux->next;
		}
		
		*ret = (void*) ALIGN_PAGE_INF(current - data->length);
	}

	vaddr_t top_heap = process->vm->vmm_top;
	if ((vaddr_t)*ret < top_heap) {
		klog("out of mem");
		*ret = NULL;
		return;
	}

	add_region(process, (vaddr_t)*ret, data);

	print_regions(process);
}

