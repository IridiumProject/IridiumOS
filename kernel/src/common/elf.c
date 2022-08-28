#include <common/elf.h>
#include <common/string.h>
#include <proc/proc.h>
#include <mm/kheap.h>
#include <mm/vmm.h>
#include <fs/initrd.h>
#include <stdint.h>

extern struct Process* current_task;
static Elf64_Phdr* progHeaders;

void* elf_get_entry(const char* path, size_t* phdrs_size) {
    // Open up path to ELF from initrd.
    char* elf_ptr = initrd_open(path);

    if (elf_ptr == NULL) {
        return NULL;
    }

    const char* const ORIG_ELF_PTR = elf_ptr;              // Original state.
    
    // Create header and make a variable
    // holding the address of the header var.
    Elf64_Ehdr header;
    char* header_ptr = (char*)&header;

    // Copy Ehdr over.
    for (uint64_t i = 0; i < sizeof(header); ++i) {
        header_ptr[i] = elf_ptr[i];
    }

    if (strncmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 || 
            header.e_ident[EI_CLASS] != ELFCLASS64 || 
            header.e_type != ET_EXEC || 
            header.e_machine != EM_X86_64 ||
            header.e_version != EV_CURRENT) {
        return NULL;
    }

    // Allocate memory for the program headers.
    const uint64_t PHDRS_SIZE = header.e_phnum * header.e_phentsize;
    Elf64_Phdr* prog_headers = kmalloc(PHDRS_SIZE);
    *phdrs_size = PHDRS_SIZE;

    // Fetch program headers.
    char* phdrs_ptr = (char*)prog_headers;
    elf_ptr = (char*)ORIG_ELF_PTR + header.e_phoff;
 
    for (uint64_t i = 0; i < PHDRS_SIZE; ++i) {
        phdrs_ptr[i] = elf_ptr[i];
    }

    for (Elf64_Phdr* phdr = prog_headers; (char*)phdr < (char*)prog_headers + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)) {
        if (phdr->p_type == PT_LOAD) {
            int n_pages = (phdr->p_memsz + 0x1000 - 1);
            Elf64_Addr segment = phdr->p_paddr;

            for (uint64_t i = segment; i < (segment + n_pages)*2; i += 0x1000) {
                vmm_map_page((PML4*)current_task->context[PCTX_CR3], i, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
            }

            elf_ptr = (char*)ORIG_ELF_PTR + phdr->p_offset;
            char* segment_ptr = (char*)segment;

            for (uint64_t i = 0; i < phdr->p_filesz; ++i) {
                segment_ptr[i] = elf_ptr[i];
            }
        }
    }

    void(*entry)(void) = ((__attribute__((sysv_abi))void(*)(void))header.e_entry);
    return entry;
}
